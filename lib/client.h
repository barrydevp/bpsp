#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "datatype.h"
#include "net.h"
#include "status.h"
#include "utarray.h"
#include "frame.h"

#define BPSP_CLIENT_BUFFER_SIZE 2

typedef struct bpsp__client bpsp__client;
typedef struct bpsp__subscriber bpsp__subscriber;

struct bpsp__subscriber {
    char* topic;           // need malloc
    bpsp__client* client;  // reference
};

void subscriber__copy(void* _dst, const void* _src);
void subscriber__dtor(void* _elt);
void subscriber__free(bpsp__subscriber* sub);

static UT_icd bpsp__subscriber_icd = {sizeof(bpsp__subscriber), NULL, &subscriber__copy, &subscriber__dtor};

struct bpsp__client {
    UT_array* subs;
    bpsp__connection* conn;

    // buffer
    bpsp__byte* inbound;
    uint32_t n_inbound;
    bpsp__byte* outbound;
    uint32_t n_outbound;

    // frame
    bpsp__frame* in_frame;
    bpsp__frame* out_frame;
};

void client__init(void* _elt);
bpsp__client* client__new(bpsp__connection* conn);
void client__copy(void* _dst, const void* _src);
void client__dtor(void* _elt);
void client__free(bpsp__client* client);
status__err client__close(bpsp__client* client);
status__err client__read(bpsp__client* client);
status__err client__write(bpsp__client* client);

static UT_icd bpsp__client_icd = {sizeof(bpsp__client), NULL, &client__copy, &client__dtor};

#endif  // _CLIENT_H_
