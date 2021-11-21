#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "broker.h"
#include "log.h"
#include "mem.h"
#include "status.h"
#include "uthash.h"

#define DEFAULT_PORT 29010
#define DEFAULT_ADDR "127.0.0.1"
/* #define DEFAULT_ADDR "0.0.0.0" */
/* #define DEFAULT_ADDR "192.168.0.129" */

int pexit(const char* str) {
    perror(str);
    exit(1);
}

// callback
typedef void (*sub_callback)(bpsp__frame*);

typedef struct {
    // key for hash table
    char* key;

    bpsp__connection* conn;
    sub_callback callback;

    /** uthash.h **/
    UT_hash_handle hh;
} subscriber;

void* server__handle_client(void* arg) {
    bpsp__client* client = (bpsp__client*)arg;

    pthread_t tid = pthread_self();
    pthread_detach(tid);

    status__err s = BPSP_OK;

    ssize_t n_write = 0;
    ssize_t n_read = 0;
    bpsp__byte buf[100];

    while (s == BPSP_OK) {
        s = net__read(client->conn, buf, 100, &n_read, 0);

        IFN_OK(s) {
            perror("net_read()");
            log__error("Fine");
            break;
        }

        s = net__write(client->conn, "test", 4, &n_write, 1);

        IFN_OK(s) {
            perror("net__write()");
            break;
        }
    }

    client__free(client);

    return NULL;
}

status__err echo(bpsp__connection* conn, bpsp__frame* frame) {
    status__err s = frame__send(conn, frame);

    IFN_OK(s) {
        log__error("frame__write() %s", ERR_TEXT(s));

        return s;
    }

    s = frame__recv(conn, frame);

    IFN_OK(s) {
        log__error("frame__read() %s", ERR_TEXT(s));

        return s;
    }

    frame__print(frame);

    return s;
}

void __loop(bpsp__connection* conn) {
    if (!conn) {
        errno = EINVAL;
        pexit("__loop()");
    }

    status__err s = BPSP_OK;
    pthread_t tid;

    bpsp__frame* out = frame__new();

    frame__empty(out);

    char* msg = "hello from server";
    char* topic = "locationA/sensorA";
    /* frame__CONNECT(out, (bpsp__byte*)msg, strlen(msg)); */
    /* s = echo(conn, out); */
    /* frame__PUB(out, (char*)topic, 0, NULL, 0, (bpsp__byte*)msg, strlen(msg)); */
    /* s = echo(conn, out); */
    /* frame__SUB(out, (char*)topic, 0, NULL, 0); */
    /* s = echo(conn, out); */
    frame__SUB(out, NULL, 0, NULL, 0);
    s = echo(conn, out);
    frame__SUB(out, (char*)"locationA/*", 0, NULL, 0);
    s = echo(conn, out);
    frame__SUB(out, (char*)"locationA/+/alo", 0, NULL, 0);
    s = echo(conn, out);
    frame__SUB(out, (char*)"locationB/sensorB", 0, NULL, 0);
    s = echo(conn, out);
    /* frame__UNSUB(out, (char*)topic, 0); */
    /* s = echo(conn, out); */
    frame__UNSUB(out, (char*)"locationA/+/alo", 0);
    s = echo(conn, out);
    frame__UNSUB(out, (char*)"locationB/sensorB", 0);
    s = echo(conn, out);
    frame__UNSUB(out, NULL, 0);
    s = echo(conn, out);
    /* frame__OK(out, 0, "Nhiet do: 100*C, do am: 30%"); */
    /* s = echo(conn, out); */
    /* frame__ERR(out, 0, BPSP_TIMEOUT, msg); */
    /* s = echo(conn, out); */
    /* frame__ERR(out, 0, BPSP_DRAINING, NULL); */
    /* s = echo(conn, out); */

    /* int count = 0; */

    /*     while (s == BPSP_OK && count < 100) { */
    /*         s = frame__write(conn, out); */
    /*  */
    /*         IFN_OK(s) { */
    /*             log__error("frame__write() %s", ERR_TEXT(s)); */
    /*  */
    /*             break; */
    /*         } */
    /*  */
    /*         s = frame__read(conn, out); */
    /*  */
    /*         frame__set_opcode(out, count + 1); */
    /*  */
    /*         IFN_OK(s) { */
    /*             log__error("frame__read() %s", ERR_TEXT(s)); */
    /*  */
    /*             break; */
    /*         } */
    /*  */
    /*         frame__print(out); */
    /*  */
    /*         count++; */
    /*  */
    /* pthread_create(&tid, NULL, &server__handle_client, (void*)client); */
    /*     } */

    frame__free(out);
}

int main(int argc, char* argv[]) {
    // declare socket attribute
    int listen_port = DEFAULT_PORT;
    const char* listen_addr = DEFAULT_ADDR;

    // parsing command line arguments
    if (argc > 1) {
        // parsing port_number at args[1]
        listen_port = atoi(argv[1]);
    }
    if (argc > 2) {
        listen_addr = argv[2];
    }

    bpsp__uint16 value = 65535;

    // Signals
    signal(SIGPIPE, SIG_IGN);
    /* signal(SIGINT, SIG_IGN); */

    // connect
    bpsp__connection* connection = net__connect(listen_addr, (uint16_t)listen_port);

    // loop
    __loop(connection);

    // destroy
    net__destroy(connection);

    return 0;
}
