#include "handle.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "bpsp.h"
#include "broker.h"
#include "client.h"
#include "log.h"
#include "status.h"
#include "utarray.h"
#include "uthash.h"

status__err handle__ECHO(bpsp__client* client) {
    status__err s = BPSP_OK;

    s = client__send(client, client->in_frame, 1);

    IFN_OK(s) {
        perror("client__send()");
        return s;
    }

    return s;
}

status__err handle__CONNECT(bpsp__client* client) {
    status__err s = BPSP_OK;

    s = frame__INFO(client->out_frame, client->broker);

    ASSERT_BPSP_OK(s);

    s = client__send(client, client->out_frame, 1);

    IFN_OK(s) {
        perror("client__send()");
        return s;
    }

    return s;
}

status__err handle__PUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    bpsp__frame* in = client->in_frame;

    bpsp__var_header* topic_hdr = NULL;

    HASH_FIND_STR(in->var_headers, "x-topic", topic_hdr);

    if (topic_hdr) {
        UT_array* subs = broker__find_subs(client->broker, topic_hdr->value, 1);

        if (subs && utarray_len(subs)) {
            bpsp__subscriber* p = (bpsp__subscriber*)utarray_front(subs);

            bpsp__frame* deliver_frame = frame__new();

            s = frame__MSG(deliver_frame, in);

            IF_OK(s) {
                for (; p != NULL; p = (bpsp__subscriber*)utarray_next(subs, p)) {
                    printf("%s - %s", p->_id, p->client_id);
                    s = broker__deliver_msg(client, p, deliver_frame);
                    IFN_OK(s) {
                        //
                        break;
                    }
                }

                frame__free(deliver_frame);
            }
        }

        if (subs) {
            utarray_free(subs);
        }

        IFN_OK(s) {
            //
            s = frame__ERR(client->out_frame, 0, s, "Deliver msg fail.");
        }
        else {
            s = frame__OK(client->out_frame, 0, "PUB OK. Deliver to %d subscriber");
        }

    } else {
        s = frame__ERR(client->out_frame, 0, BPSP_INVALID_TOPIC, "Missing header `x-topic`.");
    }

    ASSERT_BPSP_OK(s);

    s = client__send(client, client->out_frame, 1);

    IFN_OK(s) {
        perror("client__send()");
        return s;
    }

    return s;
}

status__err handle__SUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    bpsp__frame* in = client->in_frame;

    bpsp__var_header* topic_hdr = NULL;

    HASH_FIND_STR(in->var_headers, "x-topic", topic_hdr);

    if (topic_hdr) {
        s = client__sub(client, topic_hdr->value, 1);

        IFN_OK(s) {
            //
            s = frame__ERR(client->out_frame, 0, s, "Cannot add subsciber into Tree.");
        }
        else {
            s = frame__OK(client->out_frame, 0, "SUB OK.");
        }

    } else {
        s = frame__ERR(client->out_frame, 0, BPSP_INVALID_TOPIC, "Missing header `x-topic`.");
    }

    /* topic__print_tree(client->broker->topic_tree); */

    ASSERT_BPSP_OK(s);

    s = client__send(client, client->out_frame, 1);

    IFN_OK(s) {
        perror("client__send()");
        return s;
    }

    return s;
}

status__err handle__UNSUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    bpsp__frame* in = client->in_frame;

    bpsp__var_header* topic_hdr = NULL;

    HASH_FIND_STR(in->var_headers, "x-topic", topic_hdr);

    if (topic_hdr) {
        s = client__unsub(client, topic_hdr->value, 1);

        IFN_OK(s) {
            //
            s = frame__ERR(client->out_frame, 0, s, "Cannot remove subsciber from Tree.");
        }
        else {
            s = frame__OK(client->out_frame, 0, "UNSUB OK.");
        }

    } else {
        s = frame__ERR(client->out_frame, 0, BPSP_INVALID_TOPIC, "Missing header `x-topic`.");
    }

    /* topic__print_tree(client->broker->topic_tree); */

    ASSERT_BPSP_OK(s);

    s = client__send(client, client->out_frame, 1);

    IFN_OK(s) {
        perror("client__send()");
        return s;
    }

    return s;
}

status__err handle__in_frame(bpsp__client* client) {
    status__err s = frame__is_completed(client->in_frame);
    ASSERT_BPSP_OK(s);

    switch (client->in_frame->opcode) {
        case OP_CONNECT:
            s = handle__CONNECT(client);
            break;
        case OP_PUB:
            s = handle__PUB(client);
            break;
        case OP_SUB:
            s = handle__SUB(client);
            break;
        case OP_UNSUB:
            s = handle__UNSUB(client);
            break;
        case OP_NOOP:
        default:
            s = handle__ECHO(client);
            break;
    }

    return s;
}

status__err handle__client_loop(bpsp__client* client) {
    status__err s = BPSP_OK;

    while (s == BPSP_OK) {
        s = client__read(client);

        IFN_OK(s) {
            perror("client__read()");
            log__info("Client `%s` loop closing, (%s)", client->_id, ERR_TEXT(s));
            return s;
        }

        s = handle__in_frame(client);

        IFN_OK(s) {
            perror("hanle()");
            log__info("Client `%s` loop closing, (%s)", client->_id, ERR_TEXT(s));
            return s;
        }
    }

    return s;
}

void* server__handle_client(void* arg) {
    bpsp__client* client = (bpsp__client*)arg;

    pthread_t tid = pthread_self();
    pthread_detach(tid);
    bpsp__broker* broker = client->broker;

    status__err s = handle__client_loop(client);

    broker__destroy_client(broker, client, 1);

    topic__print_tree(broker->topic_tree);

    return NULL;
}
