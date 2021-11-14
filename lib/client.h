#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "net.h"
#include "status.h"
#include "utarray.h"

typedef struct bpsp__client bpsp__client;
typedef struct bpsp__subscriber bpsp__subscriber;

struct bpsp__subscriber {
    char* topic;           // need malloc
    bpsp__client* client;  // reference
};

void subscriber__copy(void* _dst, const void* _src);
void subscriber__dtor(void* _elt);
void subscriber__free(bpsp__subscriber* sub);

UT_icd bpsp__subscriber_icd = {sizeof(bpsp__subscriber), NULL, &subscriber__copy, &subscriber__dtor};

struct bpsp__client {
    UT_array* subs;
    bpsp__connection* conn;
};

void client__init(void* _elt);
bpsp__client* client__new(bpsp__connection* conn);
void client__copy(void* _dst, const void* _src);
void client__dtor(void* _elt);
void client__free(bpsp__client* client);
status__err client__close(bpsp__client* client);

UT_icd bpsp__client_icd = {sizeof(bpsp__client), NULL, &client__copy, &client__dtor};

#endif  // _CLIENT_H_
