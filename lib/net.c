#include "net.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.h"
#include "mem.h"
#include "status.h"

status__err connection__close(bpsp__connection* conn) {
    // should we lock when close connection?

    ASSERT_ARG(conn, BPSP_OK);

    status__err s = BPSP_OK;

    if (conn->sockfd > 0) {
        log__info("Closing connection %s:%d", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));
        close(conn->sockfd);
    }

    conn->sockfd = 0;
    return s;
}

void connection__free(bpsp__connection* conn) {
    if (!conn) {
        return;
    }

    connection__close(conn);

    if (conn->addr) {
        mem__free(conn->addr);
        conn->addr = NULL;
    }

    /* pthread_mutex_destroy(&conn->net_mutex); */

    mem__free(conn);
}

bpsp__connection* connection__create(int sockfd, struct sockaddr_in* addr, net__state state, net__type type) {
    bpsp__connection* conn = (bpsp__connection*)mem__malloc(sizeof(bpsp__connection));
    if (!conn) {
        return NULL;
    }
    /* memset(conn, 0, sizeof(conn)); */

    conn->sockfd = sockfd;

    conn->addr = addr;
    conn->state = state;
    conn->type = type;
    /* pthread_mutex_init(&conn->net_mutex, NULL); */

    return conn;
}

bpsp__connection* connection__init(const char* host, uint16_t port, net__state state, net__type type) {
    struct sockaddr_in* addr = (struct sockaddr_in*)mem__malloc(sizeof(struct sockaddr_in));

    if (!addr) {
        return NULL;
    }

    memset(addr, 0, sizeof(*addr));

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(host);
    addr->sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("socket()");
        mem__free(addr);

        return NULL;
    }

    bpsp__connection* conn = connection__create(sockfd, addr, state, type);

    if (!conn) {
        mem__free(addr);

        return NULL;
    }

    return conn;
}

bpsp__connection* net__dup(const bpsp__connection* conn) {
    if (!conn) {
        return NULL;
    }

    struct sockaddr_in* addr = (struct sockaddr_in*)mem__malloc(sizeof(struct sockaddr_in));

    if (!addr) {
        log__error("Cannot malloc() addr.");
        return NULL;
    }

    mem__memmove(addr, conn->addr, sizeof(*addr));

    bpsp__connection* _conn = connection__create(conn->sockfd, addr, conn->state, conn->type);

    if (!_conn) {
        mem__free(addr);
        return NULL;
    }

    return _conn;
}

bpsp__connection* net__connect(const char* host, uint16_t port) {
    bpsp__connection* conn = connection__init(host, port, NET_S_CONNECTING, NET_T_ACTIVE);

    if (!conn) {
        log__error("Cannot create socket.");
        return NULL;
    }

    log__info("Connecting to broker %s:%d ...\n", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    if (connect(conn->sockfd, (struct sockaddr*)conn->addr, sizeof(*(conn->addr))) < 0) {
        log__error("Cannot connect socket.");
        perror("connect()");

        goto RET_ERROR;
    }

    conn->state = NET_S_CONNECTED;
    log__info("Connected to broker %s:%d !", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    return conn;

RET_ERROR:
    connection__free(conn);

    return NULL;
}

bpsp__connection* net__listen(const char* host, uint16_t port) {
    bpsp__connection* conn = connection__init(host, port, NET_S_CONNECTING, NET_T_ACTIVE);

    if (!conn) {
        return NULL;
    }

    int option = 1;
    if (setsockopt(conn->sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option))) {
        log__error("Cannot set `SO_REUSEADDR`.");
        perror("setsockopt()");

        goto RET_ERROR;
    }

    log__info("Creating broker listen on %s:%d ...\n", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    socklen_t addr_len = sizeof(*(conn->addr));

    log__info("Binding...");
    if (bind(conn->sockfd, (struct sockaddr*)conn->addr, addr_len) < 0) {
        log__error("Cannot bind socket.");
        perror("bind()");

        goto RET_ERROR;
    }

    log__info("Listen...");
    if (listen(conn->sockfd, BPSP_NET_BACKLOG) < 0) {
        log__error("Cannot listen socket.");
        perror("listen()");

        goto RET_ERROR;
    }

    log__info("Broker listening on %s:%d", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    return conn;

RET_ERROR:
    connection__free(conn);

    return NULL;
}

bpsp__connection* net__accept(bpsp__connection* listener) {
    assert(listener);

    struct sockaddr_in* addr = (struct sockaddr_in*)mem__malloc(sizeof(struct sockaddr_in));
    if (!addr) {
        return NULL;
    }
    memset(addr, 0, sizeof(*addr));
    socklen_t addr_len = sizeof(*addr);

    int sockfd = -1;
    // accept new connection, return new socketfd
    while (sockfd < 0) {
        addr_len = sizeof(*addr);
        sockfd = accept(listener->sockfd, (struct sockaddr*)addr, &addr_len);
        if (sockfd < 0) {
            /* if (errno == EINTR) { */
            /*     continue; */
            /* } */

            log__error("Cannot accept socket.");
            perror("accpet()");

            goto RET_ERROR;
        }
    }

    log__info("Broker accept connection from %s:%d", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));

    bpsp__connection* conn = connection__create(sockfd, addr, NET_S_CONNECTED, NET_T_ACTIVE);

    if (!conn) {
        mem__free(addr);

        return NULL;
    }

    return conn;

RET_ERROR:
    mem__free(addr);

    return NULL;
}

void net__free(bpsp__connection* conn) {
    if (!conn) {
        return;
    }

    connection__free(conn);
}

void net__destroy(bpsp__connection* conn) {
    if (!conn) {
        return;
    }

    net__free(conn);
}

status__err net__close(bpsp__connection* conn) {
    ASSERT_ARG(conn, BPSP_OK);

    status__err s = connection__close(conn);

    return s;
}

status__err net__read(bpsp__connection* conn, void* buf, ssize_t size, ssize_t* n_read, uint8_t block) {
    status__err s = BPSP_OK;
    *n_read = 0;

    if (size <= 0) {
        return s;
    }

    assert(buf);
    ssize_t n = 0;

    while (*n_read < size) {
        n = recv(conn->sockfd, buf + *n_read, size - *n_read, 0);

        /* log__debug("Received %d bytes", n); */

        if (n == 0) {
            s = BPSP_CONNECTION_CLOSED;
            break;
        }

        if (n < 0) {
            log__error("IO Error net__read()");
            s = BPSP_IO_ERROR;
            break;
        }

        *n_read += n;

        if (!block) {
            break;
        }
    }

    return s;
}

status__err net__write(bpsp__connection* conn, void* buf, ssize_t size, ssize_t* n_write, uint8_t block) {
    ASSERT_ARG(conn, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;
    *n_write = 0;

    if (size <= 0) {
        return s;
    }

    ASSERT_ARG(buf, BPSP_INVALID_ARG);
    ssize_t n = 0;

    while (*n_write < size) {
        n = send(conn->sockfd, buf + *n_write, size - *n_write, MSG_NOSIGNAL);

        /* log__debug("Send %d bytes", n); */

        if (n == 0) {
            s = BPSP_CONNECTION_CLOSED;
            break;
        }

        if (n < 0) {
            s = BPSP_IO_ERROR;
            break;
        }

        *n_write += n;

        if (!block) {
            break;
        }
    }
    return s;
}

status__err net__read_lock(bpsp__connection* conn, void* buf, ssize_t size, ssize_t* n_read, uint8_t block) {
    ASSERT_ARG(conn, BPSP_INVALID_ARG);

    /* pthread_mutex_lock(&conn->net_mutex); */
    status__err s = net__read(conn, buf, size, n_read, block);
    /* pthread_mutex_unlock(&conn->net_mutex); */

    return s;
}

status__err net__write_lock(bpsp__connection* conn, void* buf, ssize_t size, ssize_t* n_write, uint8_t block) {
    ASSERT_ARG(conn, BPSP_INVALID_ARG);

    /* pthread_mutex_lock(&conn->net_mutex); */
    status__err s = net__read(conn, buf, size, n_write, block);
    /* pthread_mutex_unlock(&conn->net_mutex); */

    return s;
}
