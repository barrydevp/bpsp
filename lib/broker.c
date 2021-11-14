#include "broker.h"

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

    if (!pthread_mutex_init(&broker->clients_mutex, NULL)) {
        log__error("Broker cannot init `clients_mutext`");

        goto RET_ERROR;
    }

    if (!pthread_mutex_init(&((broker->topic_tree)._mutex), NULL)) {
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

status__err broker__free(bpsp__broker* broker) {
    status__err s = BPSP_OK;

    if(!broker) {
        return s;
    }

    s = broker__close(broker);

    pthread_mutex_destroy(&broker->clients_mutex);

    if (s != BPSP_OK) {
        return s;
    }

    // is it necessary to holding lock?
    pthread_mutex_lock(&broker->clients_mutex);

    utarray_clear(broker->clients);

    pthread_mutex_unlock(&broker->clients_mutex);

    mem__free(broker);

    return s;
}
