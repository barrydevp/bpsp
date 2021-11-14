#include "client.h"

#include "assert.h"
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

void client__init(void* _elt) {
    bpsp__client* elt = (bpsp__client*)_elt;

    utarray_new(elt->subs, &bpsp__subscriber_icd);
}

bpsp__client* client__new(bpsp__connection* conn) {
    assert(conn);

    bpsp__client* c = mem__malloc(sizeof(bpsp__client));

    if (!c) {
        return NULL;
    }

    client__init((void*)c);
    c->conn = conn;

    return c;
}

void client__copy(void* _dst, const void* _src) {
    bpsp__client* dst = (bpsp__client*)_dst;
    const bpsp__client* src = (bpsp__client*)_src;

    utarray_new(dst->subs, &bpsp__subscriber_icd);
    utarray_concat(dst->subs, src->subs);

    if (src->conn) {
        dst->conn = net__dup(src->conn);
    }
}

void client__dtor(void* _elt) {
    bpsp__client* elt = (bpsp__client*)_elt;

    // TODO: remove sub from topic_tree(prune if need), and free subs array
    if (elt) {
        net__free(elt->conn);
        mem__free(elt);
    }
}

void client__free(bpsp__client* client) {
    // TODO: remove sub from topic_tree(prune if need), and free subs array
    if (!client) {
        return;
    }

    net__free(client->conn);
    client__dtor((void*)client);

    mem__free(client);
}

status__err client__close(bpsp__client* client) {
    status__err s = BPSP_OK;

    if (!client) {
        return s;
    }

    s = net__close(client->conn);

    if (s != BPSP_OK) {
        return s;
    }

    return s;
}
