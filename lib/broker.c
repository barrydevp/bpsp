#include "broker.h"

#include <assert.h>

#include "client.h"
#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"
#include "utarray.h"

status__err broker__close(bpsp__broker* broker) {
    ASSERT_ARG(broker, BPSP_OK);

    status__err s = net__close(broker->listener);

    IFN_OK(s) {
        log__error("Unexpected close broker: %s", ERR_TEXT(s));

        return s;
    }

    return s;
}

void broker__free(bpsp__broker* broker) {
    ASSERT_ARG(broker, __EMPTY__);

    broker__close(broker);

    // is it necessary to holding lock?
    pthread_mutex_lock(&broker->clients_mutex);

    if (broker->clients) {
        utarray_free(broker->clients);
    }

    pthread_mutex_unlock(&broker->clients_mutex);

    pthread_mutex_destroy(&broker->clients_mutex);

    topic__free_tree(broker->topic_tree);

    mem__free(broker);
}

bpsp__broker* broker__new(const char* host, uint16_t port) {
    bpsp__broker* broker = (bpsp__broker*)mem__malloc(sizeof(bpsp__broker));

    if (!broker) {
        return NULL;
    }

    if (pthread_mutex_init(&broker->clients_mutex, NULL)) {
        log__error("Broker cannot init `clients_mutex`");

        goto RET_ERROR;
    }

    /*     if (pthread_mutex_init(&((broker->topic_tree).tree_mutex), NULL)) { */
    /*         log__error("Broker cannot init `topic_tree.tree_mutext`"); */
    /*  */
    /*         goto RET_ERROR; */
    /*     } */

    utarray_new(broker->clients, &bpsp__client_icd);

    bpsp__topic_tree* tree = topic__new_tree();

    if (!tree) {
        goto RET_ERROR;
    }

    broker->topic_tree = tree;

    bpsp__connection* listener = net__listen(host, port);

    if (!listener) {
        log__error("Broker cannot listen on %s:%u", host, port);

        goto RET_ERROR;
    }

    broker->listener = listener;

    return broker;
RET_ERROR:
    broker__free(broker);

    return NULL;
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

    bpsp__client* client = client__new(c_conn, broker);

    if (!client) {
        log__error("NULL client__new()");
        goto RET_ERROR;
    }

    return client;
RET_ERROR:
    net__free(c_conn);

    return NULL;
}

/* statsu__err broker__remove(bpsp__client* client) {} */
