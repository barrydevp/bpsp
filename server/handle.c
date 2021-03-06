#include "handle.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "bpsp.h"
#include "broker.h"
#include "client.h"
#include "frame.h"
#include "log.h"
#include "status.h"
#include "utarray.h"
#include "uthash.h"

status__err handle__reply(bpsp__client* client, bpsp__frame* in, status__err s, char* msg) {
    if (flag__is_set(in->flag, FL_ACK)) {
        IFN_OK(s) {
            //
            s = frame__ERR(client->out_frame, in, s, msg);
        }
        else {
            //
            s = frame__OK(client->out_frame, in, msg);
        }

        ASSERT_BPSP_OK(s);

        s = client__send(client, client->out_frame, 1);

    } else {
        // reset ok
        s = BPSP_OK;
    }

    return s;
}

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

    char* msg = "PUB OK.";

    bpsp__var_header* topic_hdr = frame__get_var_header(in, "x-topic");

    if (topic_hdr) {
        UT_array* subs = broker__find_subs(client->broker, topic_hdr->value, 1);

        if (subs && utarray_len(subs)) {
            bpsp__subscriber* p = (bpsp__subscriber*)utarray_front(subs);

            bpsp__frame* deliver_frame = frame__new();

            s = frame__MSG(deliver_frame, in);

            IF_OK(s) {
                for (; p != NULL; p = (bpsp__subscriber*)utarray_next(subs, p)) {
                    // avoid echo when echo is not set
                    if (p->client == client && !flag__is_set(in->flag, FL_PUB_ECHO)) {
                        continue;
                    }

                    // set sub_tag & origin topic
                    s = frame__set_var_header(deliver_frame, "x-sub-tag", p->sub_tag);
                    // TODO: Handle when cannot set_var_header
                    IFN_OK(s) {
                        //
                        continue;
                    }
                    s = frame__set_var_header(deliver_frame, "x-origin-topic", topic_hdr->value);
                    // TODO: Handle when cannot set_var_header
                    IFN_OK(s) {
                        //
                        continue;
                    }
                    printf("%s", p->_id);
                    s = broker__deliver_msg(client, p, deliver_frame);
                    // TODO: inspect error to determine if we can continue deliver msg or not.
                    /* IFN_OK(s) { */
                    /*     // */
                    /*     break; */
                    /* } */
                }

                frame__free(deliver_frame);
                // reset status
                s = BPSP_OK;
            }
        }

        if (subs) {
            utarray_free(subs);
        }

        IFN_OK(s) {
            //
            msg = "Deliver msg fail.";
        }

    } else {
        s = BPSP_INVALID_TOPIC;
        msg = "Missing header `x-topic`.";
    }

    ASSERT_BPSP_OK(s);

    s = handle__reply(client, in, s, msg);

    IFN_OK(s) {
        perror("client__send()");
        return s;
    }

    return s;
}

status__err handle__SUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    bpsp__frame* in = client->in_frame;

    /* frame__print(in); */

    bpsp__var_header* topic_hdr = frame__get_var_header(in, "x-topic");
    bpsp__var_header* sub_tag_hdr = frame__get_var_header(in, "x-sub-tag");

    char* topic = topic_hdr ? topic_hdr->value : NULL;
    char* sub_tag = sub_tag_hdr ? sub_tag_hdr->value : NULL;

    char* msg = "SUB OK.";
    if (!sub_tag) {
        msg = "SUB OK. WARNING: You need specify sub_tag, default to `_0`.";
    }

    if (topic) {
        status__err s = BPSP_OK;

        bpsp__subscriber* sub = subscriber__new(topic, sub_tag, client, NULL);

        if (!sub) {
            s = BPSP_NO_MEMORY;
        } else {
            s = client__sub0(client, sub, 1);
        }

        IFN_OK(s) {
            //
            msg = "Cannot add subsciber into Tree.";
        }

    } else {
        s = BPSP_INVALID_TOPIC;
        msg = "Missing header `x-topic`.";
    }

    s = handle__reply(client, in, s, msg);

    IFN_OK(s) {
        perror("client__send()");
        return s;
    }

    return s;
}

status__err handle__UNSUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    bpsp__frame* in = client->in_frame;

    bpsp__var_header* topic_hdr = frame__get_var_header(in, "x-topic");
    bpsp__var_header* sub_tag_hdr = frame__get_var_header(in, "x-sub-tag");

    char* topic = topic_hdr ? topic_hdr->value : NULL;
    char* sub_tag = sub_tag_hdr ? sub_tag_hdr->value : NULL;

    char* msg = "UNSUB OK.";
    if (!sub_tag) {
        msg = "UNSUB OK. WARNING: You need specify sub_tag, default to `_0`.";
    }

    if (topic) {
        s = client__unsub0(client, topic, sub_tag, 1);

        IFN_OK(s) {
            //
            msg = "Cannot remove subsciber from Tree.";
        }

    } else {
        s = BPSP_INVALID_TOPIC;
        msg = "Missing header `x-topic`.";
    }

    s = handle__reply(client, in, s, msg);

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
            break;
        }

        s = handle__in_frame(client);

        IFN_OK(s) {
            perror("hanle()");
            log__info("Client `%s` loop closing, (%s)", client->_id, ERR_TEXT(s));
            break;
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
