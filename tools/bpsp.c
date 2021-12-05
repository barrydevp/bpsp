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
#include "client.h"
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

void __publish(bpsp__connection* conn) {
    if (!conn) {
        errno = EINVAL;
        pexit("__loop()");
    }

    status__err s = BPSP_OK;

    bpsp__frame* out = frame__new();

    frame__empty(out);

    char* msg = "hello from server";
    char* topic = "locationA/sensorA";
    frame__CONNECT(out, (bpsp__byte*)msg, strlen(msg));
    s = frame__send(conn, out);
    frame__PUB(out, (char*)topic, 0, NULL, 0, (bpsp__byte*)msg, strlen(msg));
    s = frame__send(conn, out);
    frame__SUB(out, (char*)topic, 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, NULL, 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, (char*)"locationA/*", 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, (char*)"locationA/+/alo", 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, (char*)"locationB/sensorB", 0, NULL, 0);
    s = frame__send(conn, out);
    frame__PUB(out, (char*)topic, 0, NULL, 0, (bpsp__byte*)msg, strlen(msg));
    s = frame__send(conn, out);
    frame__UNSUB(out, (char*)topic, 0);
    s = frame__send(conn, out);
    frame__UNSUB(out, (char*)"locationA/+/alo", 0);
    s = frame__send(conn, out);
    frame__UNSUB(out, (char*)"locationB/sensorB", 0);
    s = frame__send(conn, out);
    frame__UNSUB(out, NULL, 0);
    s = frame__send(conn, out);
    frame__PUB(out, (char*)topic, FL_ACK | FL_PUB_ECHO, NULL, 0, (bpsp__byte*)msg, strlen(msg));
    s = frame__send(conn, out);
    /* s = echo(conn, out); */
    /* frame__OK(out, 0, "Nhiet do: 100*C, do am: 30%"); */
    /* s = echo(conn, out); */
    /* frame__ERR(out, 0, BPSP_TIMEOUT, msg); */
    /* s = echo(conn, out); */
    /* frame__ERR(out, 0, BPSP_DRAINING, NULL); */
    /* s = echo(conn, out); */
    frame__free(out);
}

void* __loop(void* arg) {
    bpsp__connection* conn = (bpsp__connection*)arg;
    if (!conn) {
        errno = EINVAL;
        pexit("__loop()");
    }

    status__err s = BPSP_OK;

    bpsp__frame* in = frame__new();

    while (s == BPSP_OK) {
        s = frame__recv(conn, in);

        IFN_OK(s) {
            //
            log__error("frame__read() %s", ERR_TEXT(s));
            break;
        }

        /* if (in->opcode == OP_MSG) { */
        frame__print(in);
        /* } */
    }

    frame__free(in);

    return 0;
}

static void usage() {
    printf(
        "Usage: bpsp <options> command [arguments...]           \n"
        "  command:                                             \n"
        "    sub <topics...>            Subscribe on topics     \n"
        "    pub <topics...>            Publish on topics       \n"
        "                                                       \n"
        "  options:                                             \n"
        "    -h, --host             Broker host address         \n"
        "    -p, --port             Broker port address         \n"
        "    -v, --version          Print version details       \n"
        "    -h, --help             Print usage                 \n"
        "                                                       \n"
        "  Basic Publish Subscribe Protocol CLI Tools           \n"
        "  Written by barrydevp(barrydevp@gmail.com)            \n");
}

int main(int argc, char* argv[]) {
    // setting logging
    log__timestamps = 0;
    log__stack_trace = 1;

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

    pthread_t tid;
    // loop
    pthread_create(&tid, NULL, &__loop, (void*)connection);

    // publish
    __publish(connection);

    pthread_join(tid, NULL);

    // destroy
    net__destroy(connection);

    return 0;
}
