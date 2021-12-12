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
#define VERSION "1.0.0"
#define MAX_BUFFER 65535

struct config {
    char* host;
    int port;
} cfg;

struct command {
    char* name;
    int argc;
    char** argv;
} cmd;

int pexit(const char* str) {
    perror(str);
    exit(1);
}

int get_input(char* buf, int buf_size) {
    while (1) {
        printf("enter: ");
        // read line from stdin (screen)
        if (fgets(buf, buf_size, stdin) == NULL) {
            fprintf(stderr, "Cannot read from stdin\n");
            return -1;
        }

        // replace new line with null character '\0'
        char* last_ch = strrchr(buf, '\n');
        if (last_ch == NULL) {
            fprintf(stderr, "Unexpected input.\n");
            continue;
        }
        *last_ch = '\0';
        break;
    }

    return 0;
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
    frame__SUB(out, (char*)topic, NULL, 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, NULL, NULL, 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, (char*)"locationA/*", NULL, 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, (char*)"locationA/+/alo", NULL, 0, NULL, 0);
    s = frame__send(conn, out);
    frame__SUB(out, (char*)"locationB/sensorB", NULL, 0, NULL, 0);
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

bpsp__connection* _connect() {
    // Signals
    signal(SIGPIPE, SIG_IGN);

    // connect
    bpsp__connection* connection = net__connect(cfg.host, (uint16_t)cfg.port);

    if (!connection) {
        goto ERROR_2;
    }

    // connect handshaking
    bpsp__frame* frame = frame__new();

    if (!frame) {
        goto ERROR_2;
    }

    char* msg = "BPSP Tools.";
    status__err s = frame__CONNECT(frame, (bpsp__byte*)msg, strlen(msg));

    IFN_OK(s) {
        //
        goto ERROR_1;
    }

    s = frame__send(connection, frame);

    IFN_OK(s) {
        //
        goto ERROR_1;
    }

    s = frame__recv(connection, frame);

    IFN_OK(s) {
        //
        goto ERROR_1;
    }

    printf("Broker %s\n\n", frame->payload_size ? (char*)frame->payload : "NO RESPONSE");

    frame__free(frame);

    return connection;

ERROR_1:
    frame__free(frame);
ERROR_2:
    fprintf(stderr, "Cannot established connection with broker.");
    exit(1);
}

void _close(bpsp__connection* connection) {
    // destroy
    net__destroy(connection);
}

void handle_pub() {
    if (cmd.argc <= 0) {
        fprintf(stderr, "Invalid arguments for `pub` command. You must specify at least one topic.");
        exit(1);
    }

    bpsp__connection* conn = _connect();

    status__err s = BPSP_OK;

    bpsp__frame* in = frame__new();
    bpsp__frame* out = frame__new();
    char buffer[MAX_BUFFER + 1];

    while (s == BPSP_OK) {
        if (get_input(buffer, MAX_BUFFER) < 0) {
            exit(1);
        }

        if (strcasecmp(buffer, "exit") == 0) {
            break;
        }

        int argi = 0;
        while (argi < cmd.argc) {
            char* topic = cmd.argv[argi++];
            s = frame__PUB(out, topic, FL_ACK, NULL, 0, (bpsp__byte*)buffer, strlen(buffer));

            IFN_OK(s) {
                //
                log__error("frame__PUB() %s", ERR_TEXT(s));
                fprintf(stderr, "%s\n", ERR_TEXT(s));
                continue;
            }

            s = frame__send(conn, out);

            IFN_OK(s) {
                //
                log__error("frame__read() %s", ERR_TEXT(s));
                fprintf(stderr, "%s\n", ERR_TEXT(s));
                break;
            }

            s = frame__recv(conn, in);

            IFN_OK(s) {
                //
                log__error("frame__read() %s", ERR_TEXT(s));
                fprintf(stderr, "%s\n", ERR_TEXT(s));
                break;
            }

            printf("Published %u bytes to \"%s\" .\n", out->data_size, topic);
        }
    }

    frame__free(in);
    frame__free(out);

    _close(conn);
}

void handle_sub() {
    if (cmd.argc <= 0) {
        fprintf(stderr, "Invalid arguments for `sub` command. You must specify at least one topic.");
        exit(1);
    }

    bpsp__connection* conn = _connect();

    status__err s = BPSP_OK;

    bpsp__frame* in = frame__new();
    bpsp__frame* out = frame__new();
    char buffer[MAX_BUFFER + 1];

    int argi = 0;
    while (argi < cmd.argc) {
        char* topic = cmd.argv[argi++];
        s = frame__SUB(out, topic, "_99", FL_ACK, NULL, 0);

        IFN_OK(s) {
            //
            log__error("frame__SUB() %s", ERR_TEXT(s));
            fprintf(stderr, "Cannot subscribe %s\n", ERR_TEXT(s));
            break;
        }

        s = frame__send(conn, out);

        IFN_OK(s) {
            //
            log__error("frame__send() %s", ERR_TEXT(s));
            fprintf(stderr, "Cannot subscribe %s\n", ERR_TEXT(s));
            break;
        }

        s = frame__recv(conn, in);

        IFN_OK(s) {
            //
            log__error("frame__recv() %s", ERR_TEXT(s));
            fprintf(stderr, "Cannot subscribe %s\n", ERR_TEXT(s));
            break;
        }

        printf("Subscribing on %s .\n", topic);
    }

    while (s == BPSP_OK) {
        s = frame__recv(conn, in);

        IFN_OK(s) {
            //
            log__error("frame__read() %s", ERR_TEXT(s));
            break;
        }

        bpsp__var_header* hdr_from = frame__get_var_header(in, "x-from");
        bpsp__var_header* hdr_topic = frame__get_var_header(in, "x-topic");

        char* from = hdr_from ? hdr_from->value : "unknown";
        char* topic = hdr_topic ? hdr_topic->value : "unknown";
        if (in->data_size) {
            mem__memcpy(buffer, in->payload, in->data_size);
        }
        buffer[in->data_size] = '\0';

        printf("%s ->> \"%s\" Received %u bytes : %s\n", from, topic, in->data_size, buffer);
    }

    frame__free(in);
    frame__free(out);
    _close(conn);
}

int handle_cmd(struct command* cmd) {
    if (strcasecmp(cmd->name, "pub") == 0) {
        handle_pub();
    } else if (strcasecmp(cmd->name, "sub") == 0) {
        handle_sub();
    } else {
        fprintf(stderr, "The command `%s` is currenly not suppoorted.\n\n", cmd->name);

        return 1;
    }

    return 0;
}

static struct option longopts[] = {{"host", required_argument, NULL, 'h'},
                                   {"port", required_argument, NULL, 'p'},
                                   {"logs", no_argument, NULL, 'l'},
                                   {"timestamps", no_argument, NULL, 't'},
                                   {"version", no_argument, NULL, 'v'},
                                   {"help", no_argument, NULL, '?'},
                                   {NULL, 0, NULL, 0}};

static int parse_args(struct config* cfg, struct command* cmd, int argc, char** argv) {
    char c;
    memset(cfg, 0, sizeof(struct config));
    cfg->host = DEFAULT_ADDR;
    cfg->port = DEFAULT_PORT;

    // parse option
    while ((c = getopt_long(argc, argv, "h:p:ltv?", longopts, NULL)) != -1) {
        switch (c) {
            case 'h':
                cfg->host = optarg;
                break;
            case 'p':
                cfg->port = atoi(optarg);
                if (cfg->port < 1023 || cfg->port > 65535) {
                    fprintf(stderr, "Invalid port number: %s. Port number MUST be in range (1023-65535)\n\n", optarg);
                    return -1;
                }
                break;
            case 'l':
                log__enable = 1;
                break;
            case 't':
                log__timestamps = 1;
                break;
            case 'v':
                printf("Basic Publish Subscribe Tools %s\n", VERSION);
            case '?':
            case ':':
            default:
                return -1;
        }
    }

    memset(cmd, 0, sizeof(struct command));

    // parse command
    if (optind < argc) {
        cmd->name = argv[optind++];
        cmd->argc = argc - optind;
        cmd->argv = argv + optind;
    } else {
        fprintf(stderr, "Invalid command, command must not be empty.\n\n");
        return -1;
    }

    return 0;
}

static void usage() {
    printf(
        "Usage: bpsp <options> command [arguments...]           \n"
        "  command:                                             \n"
        "    sub <topics...>        Subscribe on topics         \n"
        "    pub <topics...>        Publish on topics           \n"
        "                                                       \n"
        "  options:                                             \n"
        "    -h, --host             Broker host address         \n"
        "    -p, --port             Broker port address         \n"
        "    -l, --logs             Show logs                   \n"
        "    -t, --timestamps       Show logs timestamps        \n"
        "    -v, --version          Print version details       \n"
        "    -?, --help             Print usage                 \n"
        "                                                       \n"
        "Basic Publish Subscribe Protocol CLI Tools             \n");
}

int main(int argc, char* argv[]) {
    // setting default
    log__timestamps = 0;
    log__stack_trace = 0;
    log__enable = 0;

    if (parse_args(&cfg, &cmd, argc, argv)) {
        usage();
        exit(1);
    }

    return handle_cmd(&cmd);
}
