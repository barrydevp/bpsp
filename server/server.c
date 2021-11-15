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
#include "status.h"

#define DEFAULT_PORT 29010
/* #define DEFAULT_ADDR "127.0.0.1" */
#define DEFAULT_ADDR "0.0.0.0"
/* #define DEFAULT_ADDR "192.168.0.129" */

bpsp__broker* broker = NULL;

int pexit(const char* str) {
    perror(str);
    exit(1);
}

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

        if (s != BPSP_OK) {
            perror("net_read()");
            break;
        }
        s = net__write(client->conn, "test", 4, &n_write, 1);
        if (s != BPSP_OK) {
            perror("net__write()");
            break;
        }
    }

    client__free(client);

    return NULL;
}

void server__loop() {
    if (!broker) {
        errno = EINVAL;
        pexit("server__loop()");
    }

    status__err s = BPSP_OK;
    bpsp__client* client = NULL;
    pthread_t tid;

    while (s == BPSP_OK) {
        client = broker__accept(broker);

        pthread_create(&tid, NULL, &server__handle_client, (void*)client);
    }
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

    // Signals
    signal(SIGPIPE, SIG_IGN);

    // create
    broker = broker__new(listen_addr, (uint16_t)listen_port);

    // loop
    server__loop();

    // destroy
    broker__destroy(broker);

    return 0;
}
