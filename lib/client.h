#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>

#include "bpsp.h"
#include "datatype.h"
#include "frame.h"
#include "net.h"
#include "status.h"
#include "topic.h"
#include "utarray.h"

#define BPSP_CLIENT_BUFFER_SIZE 2

struct bpsp__subscriber {
    char* topic;           // need malloc
    bpsp__client* client;  // reference

    topic__node* node;  // reference to node
};

void subscriber__ctor(void* sub);
bpsp__subscriber* subscriber__new(char* topic, bpsp__client* client, topic__node* node);
void subscriber__copy(void* _dst, const void* _src);
void subscriber__dtor(void* _elt);
void subscriber__free(bpsp__subscriber* sub);

struct bpsp__client {
    // core
    bpsp__broker* broker;
    UT_array* subs;
    bpsp__connection* conn;

    // synchonization
    bpsp__uint16 ref_count;
    pthread_cond_t ref_cond;
    pthread_mutex_t cli_mutex;  // multiple thread write but only one thread read at a time,
                                // so we use this mutex to lock write

    // frame
    bpsp__frame* in_frame;
    bpsp__frame* out_frame;
};

void client__init(void* elt);
bpsp__client* client__new(bpsp__connection* conn);
void client__copy(void* _dst, const void* _src);
void client__dtor(void* _elt);
void client__free(bpsp__client* client);
status__err client__close(bpsp__client* client);
status__err client__destroy(bpsp__client* client);
status__err client__read(bpsp__client* client);
status__err client__write(bpsp__client* client);

static UT_icd bpsp__client_icd = {sizeof(bpsp__client), NULL, &client__copy, &client__dtor};
static UT_icd bpsp__subscriber_icd = {sizeof(bpsp__subscriber), NULL, &subscriber__copy, &subscriber__dtor};

#endif  // _CLIENT_H_
