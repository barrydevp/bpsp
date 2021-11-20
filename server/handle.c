#include "handle.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "bpsp.h"
#include "broker.h"
#include "client.h"
#include "log.h"
#include "status.h"

status__err handle__ECHO(bpsp__client* client) {
    status__err s = BPSP_OK;

    s = client__write(client, client->in_frame);

    IFN_OK(s) {
        perror("client__write()");
        return s;
    }

    return s;
}

status__err handle__CONNECT(bpsp__client* client) {
    status__err s = BPSP_OK;

    s = frame__INFO(client->out_frame, client->broker);

    ASSERT_BPSP_OK(s);

    /* s = client__write(client, client->out_frame); */
    s = client__send(client);

    IFN_OK(s) {
        perror("client__write()");
        return s;
    }

    return s;
}

status__err handle__PUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    s = client__write(client, client->in_frame);

    IFN_OK(s) {
        perror("client__write()");
        return s;
    }

    return s;
}

status__err handle__SUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    s = client__write(client, client->in_frame);

    IFN_OK(s) {
        perror("client__write()");
        return s;
    }

    return s;
}

status__err handle__UNSUB(bpsp__client* client) {
    status__err s = BPSP_OK;

    s = client__write(client, client->in_frame);

    IFN_OK(s) {
        perror("client__write()");
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
            log__info("Client `%s` loop closing, by# %s", client->_id, ERR_TEXT(s));
            return s;
        }

        s = handle__in_frame(client);

        IFN_OK(s) {
            perror("hanle()");
            log__info("Client `%s` loop closing, by# %s", client->_id, ERR_TEXT(s));
            return s;
        }
    }

    return s;
}

void* server__handle_client(void* arg) {
    bpsp__client* client = (bpsp__client*)arg;

    pthread_t tid = pthread_self();
    pthread_detach(tid);

    status__err s = handle__client_loop(client);

    broker__destroy_client(client->broker, client, 1);

    return NULL;
}
