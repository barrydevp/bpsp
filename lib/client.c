#include "client.h"

#include <assert.h>

#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"
#include "utarray.h"

void subscriber__copy(void* _dst, const void* _src) {
    bpsp__subscriber* dst = (bpsp__subscriber*)_dst;
    const bpsp__subscriber* src = (bpsp__subscriber*)_src;

    if (src->topic) {
        dst->topic = mem__strdup(src->topic);
    }

    dst->client = src->client;
}

void subscriber__dtor(void* _elt) {
    bpsp__subscriber* elt = (bpsp__subscriber*)_elt;

    if (!elt) {
        return;
    }

    mem__free(elt->topic);
}

void subscriber__free(bpsp__subscriber* sub) {
    if (!sub) {
        return;
    }

    subscriber__dtor((void*)sub);
    mem__free(sub->topic);
}

void client__init(void* _elt) { bpsp__client* c = (bpsp__client*)_elt; }

bpsp__client* client__new(bpsp__connection* conn) {
    assert(conn);

    bpsp__client* c = mem__malloc(sizeof(bpsp__client));

    if (!c) {
        log__error("Cannot malloc() client");

        return NULL;
    }

    client__init((void*)c);

    utarray_new(c->subs, &bpsp__subscriber_icd);

    bpsp__byte* inbound = (bpsp__byte*)mem__malloc(sizeof(bpsp__byte) * BPSP_CLIENT_BUFFER_SIZE);
    if (!inbound) {
        log__error("Cannot malloc outbound()");
        goto RET_ERROR;
    }

    bpsp__byte* outbound = (bpsp__byte*)mem__malloc(sizeof(bpsp__byte) * BPSP_CLIENT_BUFFER_SIZE);
    if (!outbound) {
        log__error("Cannot malloc outbound()");
        mem__free(inbound);
        goto RET_ERROR;
    }

    memset(inbound, 0, sizeof(bpsp__byte) * BPSP_CLIENT_BUFFER_SIZE);
    memset(outbound, 0, sizeof(bpsp__byte) * BPSP_CLIENT_BUFFER_SIZE);

    c->conn = conn;
    c->inbound = inbound;
    c->n_inbound = 0;
    c->outbound = outbound;
    c->n_outbound = 0;

    return c;

RET_ERROR:
    utarray_free(c->subs);
    mem__free(c);
    return NULL;
}

void client__copy(void* _dst, const void* _src) {
    bpsp__client* dst = (bpsp__client*)_dst;
    const bpsp__client* src = (bpsp__client*)_src;

    utarray_new(dst->subs, &bpsp__subscriber_icd);
    utarray_concat(dst->subs, src->subs);

    if (src->conn) {
        dst->conn = net__dup(src->conn);
    }

    bpsp__byte* inbound = (bpsp__byte*)mem__malloc(sizeof(bpsp__byte) * BPSP_CLIENT_BUFFER_SIZE);
    if (!inbound) {
        log__error("Cannot malloc outbound()");
    } else {
        mem__memcpy(inbound, src->inbound, src->n_inbound * sizeof(*inbound));
    }

    bpsp__byte* outbound = (bpsp__byte*)mem__malloc(sizeof(bpsp__byte) * BPSP_CLIENT_BUFFER_SIZE);
    if (!outbound) {
        log__error("Cannot malloc outbound()");
    } else {
        mem__memcpy(outbound, src->outbound, src->n_outbound * sizeof(*outbound));
    }

    dst->inbound = inbound;
    dst->n_inbound = 0;
    dst->outbound = outbound;
    dst->n_outbound = 0;
}

void client__dtor(void* _elt) {
    bpsp__client* elt = (bpsp__client*)_elt;

    if (!elt) {
        return;
    }

    // TODO: remove sub from topic_tree(prune if need), and free subs array
    net__free(elt->conn);

    if (elt->subs) {
        utarray_free(elt->subs);
    }

    if (elt->inbound) {
        mem__free(elt->inbound);
        elt->inbound = NULL;
    }

    if (elt->outbound) {
        mem__free(elt->outbound);
        elt->outbound = NULL;
    }
}

void client__free(bpsp__client* client) {
    if (!client) {
        return;
    }

    client__dtor((void*)client);

    mem__free(client);
}

status__err client__close(bpsp__client* client) {
    status__err s = BPSP_OK;

    if (!client) {
        return s;
    }

    s = net__close(client->conn);

    IFN_OK(s) {
        return s;
    }

    return s;
}
