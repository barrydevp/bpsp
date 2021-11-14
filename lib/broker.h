#ifndef _BROKER_H_
#define _BROKER_H_

#include "client.h"
#include "datatype.h"
#include "net.h"
#include "pthread.h"
#include "status.h"
#include "topic.h"
#include "utarray.h"

typedef struct bpsp__broker bpsp__broker;

struct bpsp__broker {
    bpsp__connection* listener;

    pthread_mutex_t clients_mutex;
    UT_array* clients;

    bpsp__topic_tree topic_tree;
};

bpsp__broker* broker__new(const char* host, uint16_t port);
status__err broker__free(bpsp__broker* broker);
status__err broker__close(bpsp__broker* broker);

#endif  // _BROKER_H_
