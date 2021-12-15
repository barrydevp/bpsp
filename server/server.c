#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bpsp.h"
#include "broker.h"
#include "client.h"
#include "handle.h"
#include "log.h"
#include "status.h"

#ifdef TRACE_ALL
#define TRACE_ALL
#endif

#define DEFAULT_PORT 29010
/* #define DEFAULT_ADDR "127.0.0.1" */
#define DEFAULT_ADDR "0.0.0.0"
/* #define DEFAULT_ADDR "192.168.0.129" */

bpsp__broker* broker = NULL;

struct config {
    char* address;
    int port;

} cfg;

int pexit(const char* str) {
    perror(str);
    exit(1);
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

static struct option longopts[] = {{"address", required_argument, NULL, 'a'},
                                   {"port", required_argument, NULL, 'p'},
                                   {"silent", no_argument, &log__enable, 0},
                                   {"timestamps", no_argument, &log__timestamps, 1},
                                   {"stack-trace", no_argument, &log__stack_trace, 1},
                                   {"frame", no_argument, &log__frame, 1},
                                   {"version", no_argument, NULL, 'v'},
                                   {"help", no_argument, NULL, 'h'},
                                   {NULL, 0, NULL, 0}};

static int parse_args(struct config* cfg, int argc, char** argv) {
    char c;
    int option_index;
    memset(cfg, 0, sizeof(struct config));
    cfg->address = DEFAULT_ADDR;
    cfg->port = DEFAULT_PORT;

    // parse option
    while ((c = getopt_long(argc, argv, "a:p:vh", longopts, &option_index)) != -1) {
        switch (c) {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (longopts[option_index].flag != 0) {
                    //
                    break;
                }
            case 'a':
                cfg->address = optarg;
                break;
            case 'p':
                cfg->port = atoi(optarg);
                if (cfg->port < 1023 || cfg->port > 65535) {
                    fprintf(stderr, "Invalid port number: %s. Port number MUST be in range (1023-65535)\n\n", optarg);
                    return -1;
                }
                break;
            case 'v':
                printf("\n");
                printf("Basic Publish Subscribe Broker %s\n", BROKER_DEFAULT_VERION);
                printf("Copyright (C) 2021 Hai Dao(barrydevp)\n\n");
                return 1;
            case 'h':
            case '?':
            case ':':
            default:
                return -1;
        }
    }

    return 0;
}

static void usage() {
    printf(
        "Usage: bpsp_server <options>                           \n"
        "  options:                                             \n"
        "    -a, --address          Broker binding address      \n"
        "    -p, --port             Broker binding port         \n"
        "    --silent               Disable logs                \n"
        "    --timestamps           Show logs timestamps        \n"
        "    --frame                Show logs frame             \n"
        "    --stack-trace          Show error logs stack trace \n"
        "    -v, --version          Print version details       \n"
        "    -h, --help             Print usage                 \n"
        "                                                       \n"
        "Basic Publish Subscribe Protocol Broker Server         \n"
        "Written by barrydevp(barrydevp@gmail.com)              \n"
        "https://github.com/barrydevp/bpsp                      \n");
}

int main(int argc, char* argv[]) {
    /*     // setting logging */
    /*     log__timestamps = 0; */
    /*     log__stack_trace = 1; */
    /*     log__frame = 0; */
    /*  */
    /*     // declare socket attribute */
    /*     int listen_port = DEFAULT_PORT; */
    /*     const char* listen_addr = DEFAULT_ADDR; */
    /*  */
    /*     // parsing command line arguments */
    /*     if (argc > 1) { */
    /*         // parsing port_number at args[1] */
    /*         listen_port = atoi(argv[1]); */
    /*     } */
    /*     if (argc > 2) { */
    /*         listen_addr = argv[2]; */
    /*     } */

    int parse_result = 0;
    if ((parse_result = parse_args(&cfg, argc, argv))) {
        if (parse_result < 0) {
            usage();
            exit(1);
        } else {
            exit(0);
        }
    }

    // Signals
    signal(SIGPIPE, SIG_IGN);
    /* signal(SIGINT, SIG_IGN); */

    // create
    broker = broker__new(cfg.address, (uint16_t)cfg.port);

    // loop
    server__loop();

    // destroy
    broker__destroy(broker);

    return 0;
}
