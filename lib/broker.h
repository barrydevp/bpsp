#ifndef _BROKER_H_
#define _BROKER_H_

#include "bpsp.h"
#include "client.h"
#include "datatype.h"
#include "net.h"
#include "pthread.h"
#include "status.h"
#include "topic.h"
#include "utarray.h"

#define BROKER_VERION_LEN 12  // major(3).minor(3).patch(3) eg: 1.0.0
#define BROKER_MAX_NAME_LEN 256
#define BROKER_DEFAULT_VERION "1.0.0"
#define BROKER_DEFAULT_NAME "BPSP - Basic Publish Subscribe Broker"

struct broker__info {
    char* version;
    char* name;
};

struct bpsp__broker {
    broker__info* info;
    bpsp__connection* listener;
    uint8_t is_close;

    pthread_mutex_t mutex;
    pthread_rwlock_t cli_rw_lock;
    bpsp__client* clients;
    /* UT_array* clients; */

    bpsp__topic_tree* topic_tree;
};

bpsp__broker* broker__new(const char* host, uint16_t port);
bpsp__broker* broker__new_with_info(const char* host, uint16_t port, broker__info* info);
void broker__dtor(bpsp__broker* broker);
void broker__free(bpsp__broker* broker);
void broker__destroy(bpsp__broker* broker);
status__err broker__close(bpsp__broker* broker, uint8_t lock);
bpsp__client* broker__accept(bpsp__broker* broker);
status__err broker__destroy_client(bpsp__broker* broker, bpsp__client* client, uint8_t lock);
status__err broker__deliver_msg(bpsp__client* source_client, bpsp__subscriber* sub, bpsp__frame* frame);
status__err broker__add_sub(bpsp__broker* broker, bpsp__subscriber* sub, uint8_t lock);
status__err broker__del_sub(bpsp__broker* broker, bpsp__subscriber* sub, uint8_t lock);
UT_array* broker__find_subs(bpsp__broker* broker, char* topic, uint8_t lock);

#endif  // _BROKER_H_
