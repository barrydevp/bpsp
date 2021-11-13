#include "net.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.h"
#include "mem.h"
#include "status.h"

void __cleanup(bpsp__connection* conn) {
    if (conn) {
        if (conn->sockfd > 0) {
            close(conn->sockfd);
        }

        if (conn->addr) {
            mem__free(conn->addr);
        }

        mem__free(conn);
    }
}

bpsp__connection* __create_conn(int sockfd, struct sockaddr_in* addr, net__state state, net__type type) {
    bpsp__connection* conn = (bpsp__connection*)mem__malloc(sizeof(bpsp__connection));
    /* memset(&conn, 0, sizeof(conn)); */

    conn->sockfd = sockfd;

    conn->addr = addr;
    conn->state = state;
    conn->type = type;

    return conn;
}

bpsp__connection* __create(const char* host, uint16_t port, net__state state, net__type type) {
    struct sockaddr_in* addr = (struct sockaddr_in*)mem__malloc(sizeof(struct sockaddr_in));

    memset(addr, 0, sizeof(*addr));

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(host);
    addr->sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        mem__free(addr);

        return NULL;
    }

    return __create_conn(sockfd, addr, state, type);
}

bpsp__connection* net__connect(const char* host, uint16_t port) {
    bpsp__connection* conn = __create(host, port, NET_S_CONNECTING, NET_T_ACTIVE);

    if (!conn) {
        log__error("Cannot create socket.");
        return NULL;
    }

    log__info("Connecting to broker %s:%d ...\n", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    if (connect(conn->sockfd, (struct sockaddr*)conn->addr, sizeof(*(conn->addr))) < 0) {
        log__error("Cannot connect socket.");
        goto RET_ERROR;
    }

    conn->state = NET_S_CONNECTED;
    log__info("Connected to broker %s:%d !", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    return conn;

RET_ERROR:
    __cleanup(conn);

    return NULL;
}

bpsp__connection* net__listen(const char* host, uint16_t port) {
    bpsp__connection* conn = __create(host, port, NET_S_CONNECTING, NET_T_ACTIVE);

    if (!conn) {
        return NULL;
    }

    log__info("Creating broker listen on %s:%d ...\n", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    socklen_t addr_len = sizeof(*(conn->addr));

    log__info("Binding...");
    if (bind(conn->sockfd, (struct sockaddr*)conn->addr, addr_len) < 0) {
        log__error("Cannot bind socket.");
        goto RET_ERROR;
    }

    log__info("Listen...");
    if (listen(conn->sockfd, BPSP_NET_BACKLOG) < 0) {
        log__error("Cannot listen socket.");
        goto RET_ERROR;
    }

    log__info("Broker listening on %s:%d", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));

    return conn;

RET_ERROR:
    __cleanup(conn);

    return NULL;
}

bpsp__connection* net__accept(bpsp__connection* listener) {
    assert(listener);

    struct sockaddr_in* addr = (struct sockaddr_in*)mem__malloc(sizeof(struct sockaddr_in));
    memset(addr, 0, sizeof(*addr));
    socklen_t addr_len = sizeof(*addr);

    int sockfd = 0;
    // accept new connection, return new socketfd
    while (sockfd < 0) {
        sockfd = accept(listener->sockfd, (struct sockaddr*)addr, &addr_len);
        if (sockfd < 0) {
            if (errno == EINTR) {
                continue;
            }

            log__error("Cannot accept socket.");
            goto RET_ERROR;
        }
    }

    return __create_conn(sockfd, addr, NET_S_CONNECTED, NET_T_ACTIVE);

RET_ERROR:
    mem__free(addr);

    return NULL;
}

status__err net__close(bpsp__connection* conn) {
    status__err s = BPSP_OK;

    assert(conn);
    log__info("Closing connection %s:%d", inet_ntoa(conn->addr->sin_addr), ntohs(conn->addr->sin_port));
    __cleanup(conn);

    return s;
}

status__err net__read(bpsp__connection* conn, void* buf, size_t size, size_t* n_read, uint8_t block) {
    status__err s = BPSP_OK;
    *n_read = 0;

    if (size <= 0) {
        return s;
    }

    assert(buf);
    size_t n = 0;

    while (*n_read < size) {
        n = recv(conn->sockfd, buf + *n_read, size - *n_read, 0);

        log__debug("Received %d bytes", n);

        if (n == 0) {
            s = BPSP_CONNECTION_CLOSED;
            break;
        }

        if (n < 0) {
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

status__err net__write(bpsp__connection* conn, void* buf, size_t size, size_t* n_write, uint8_t block) {
    status__err s = BPSP_OK;
    *n_write = 0;

    if (size <= 0) {
        return s;
    }

    assert(buf);
    size_t n = 0;

    while (*n_write < size) {
        n = send(conn->sockfd, buf + *n_write, size - *n_write, MSG_NOSIGNAL);

        log__debug("Send %d bytes", n);

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
