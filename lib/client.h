#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "mem.h"
#include "utarray.h"
#include "net.h"

typedef struct bpsp__client bpsp__client;
typedef struct bpsp__subscriber bpsp__subscriber;

struct bpsp__subscriber {
    char* topic;        // need malloc
    bpsp__client* cli;  // reference
};

void subscriber__copy(void* _dst, const void* _src) {
    bpsp__subscriber* dst = (bpsp__subscriber*)_dst;
    const bpsp__subscriber* src = (bpsp__subscriber*)_src;

    if (src->topic) {
        dst->topic = mem__strdup(src->topic);
    }

    dst->cli = src->cli;
}

void subscriber__free(void* _elt) {
    bpsp__subscriber* elt = (bpsp__subscriber*)_elt;

    if (elt) {
        mem__free(elt->topic);
        mem__free(elt);
    }
}

UT_icd bpsp__subscriber_icd = {sizeof(bpsp__subscriber), NULL, &subscriber__copy, &subscriber__free};

struct bpsp__client {
    UT_array* subs;
    bpsp__connection* conn;
};

void client__init(void* _elt) {
    bpsp__client* elt = (bpsp__client*)_elt;

    utarray_new(elt->subs, &bpsp__subscriber_icd);
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

void client__free(void* _elt) {
    bpsp__client* elt = (bpsp__client*)_elt;

    if (elt) {
        net__free(elt->conn);
        mem__free(elt);
    }
}

UT_icd bpsp__client_icd = {sizeof(bpsp__client), NULL};

#endif  // _CLIENT_H_
