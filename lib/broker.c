#include "broker.h"

#include <assert.h>

#include "client.h"
#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"
#include "utarray.h"

bpsp__broker* broker__new(const char* host, uint16_t port) {
    bpsp__broker* broker = (bpsp__broker*)mem__malloc(sizeof(bpsp__broker));

    if (!broker) {
        return NULL;
    }

    if (pthread_mutex_init(&broker->clients_mutex, NULL)) {
        log__error("Broker cannot init `clients_mutex`");

        goto RET_ERROR;
    }

    if (pthread_mutex_init(&((broker->topic_tree)._mutex), NULL)) {
        log__error("Broker cannot init `topic_tree._mutext`");

        goto RET_ERROR;
    }

    utarray_new(broker->clients, &bpsp__client_icd);

    bpsp__connection* listener = net__listen(host, port);

    if (!listener) {
        log__error("Broker cannot listen on %s:%u", host, port);

        goto RET_ERROR;
    }

    broker->listener = listener;

    return broker;
RET_ERROR:
    mem__free(broker);

    return NULL;
}

status__err broker__close(bpsp__broker* broker) {
    status__err s = BPSP_OK;

    if (!broker) {
        return s;
    }

    s = net__close(broker->listener);

    if (s != BPSP_OK) {
        return s;
    }

    return s;
}

void broker__free(bpsp__broker* broker) {
    if (!broker) {
        return;
    }

    broker__close(broker);

    pthread_mutex_destroy(&broker->clients_mutex);

    // is it necessary to holding lock?
    pthread_mutex_lock(&broker->clients_mutex);

    utarray_clear(broker->clients);

    pthread_mutex_unlock(&broker->clients_mutex);

    mem__free(broker);
}

void broker__destroy(bpsp__broker* broker) {
    if (!broker) {
        return;
    }

    broker__free(broker);
}

bpsp__client* broker__accept(bpsp__broker* broker) {
    assert(broker);

    bpsp__connection* c_conn = net__accept(broker->listener);

    if (!c_conn) {
        log__error("NULL client__accept()");
        return NULL;
    }

    bpsp__client* client = client__new(c_conn);

    if (!client) {
        log__error("NULL client__new()");
        goto RET_ERROR;
    }

    return client;
RET_ERROR:
    net__free(c_conn);

    return NULL;
}
