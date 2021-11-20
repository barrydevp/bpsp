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
#define BPSP_CLIENT_ID_LEN 6

struct bpsp__subscriber {
    char* _id;             // need malloc; _id = client_id + '/' + topic => 'Sa123u/locationA/sensorA'
    bpsp__client* client;  // reference

    topic__node* node;  // reference to node
};

struct subscriber__hash {
    char* key;
    bpsp__subscriber* sub;

    /** uthash.h **/
    UT_hash_handle hh;
};

void subscriber__ctor(void* sub);
bpsp__subscriber* subscriber__new(char* topic, bpsp__client* client, topic__node* node);
void subscriber__copy(void* _dst, const void* _src);
void subscriber__dtor(void* _elt);
void subscriber__free(bpsp__subscriber* sub);
void subscriber__free_hash_elt(subscriber__hash* hsh);
subscriber__hash* subscriber__new_hash_elt(char* key, bpsp__subscriber* sub);
char* subscriber__get_topic(bpsp__subscriber* sub);

struct bpsp__client {
    // core
    char _id[BPSP_CLIENT_ID_LEN + 1];
    bpsp__broker* broker;
    /* UT_array* subs; */
    subscriber__hash* subs;
    bpsp__connection* conn;

    // synchonization
    bpsp__uint16 ref_count;
    pthread_cond_t ref_cond;
    pthread_mutex_t cli_mutex;
    pthread_rwlock_t rw_lock;  // multiple thread may write same time but we assume only one thread read at a time,
                                // so we use this mutex to lock write only

    // frame
    bpsp__frame* in_frame;
    bpsp__frame* out_frame;

    /** uthash.h **/
    UT_hash_handle hh;
};

void client__init(void* elt);
bpsp__client* client__new(bpsp__connection* conn, bpsp__broker* broker);
void client__copy(void* _dst, const void* _src);
void client__dtor(void* _elt);
void client__free(bpsp__client* client);
status__err client__close(bpsp__client* client);
status__err client__destroy(bpsp__client* client);
status__err client__recv(bpsp__client* client);
status__err client__send(bpsp__client* client);
status__err client__read(bpsp__client* client);
status__err client__write(bpsp__client* client, bpsp__frame* frame);
status__err client__sub(bpsp__client* client, char* topic, uint8_t lock);
status__err client__unsub(bpsp__client* client, char* topic, uint8_t lock);

static UT_icd bpsp__client_icd = {sizeof(bpsp__client), NULL, &client__copy, &client__dtor};
static UT_icd bpsp__subscriber_icd = {sizeof(bpsp__subscriber), NULL, &subscriber__copy, &subscriber__dtor};

#endif  // _CLIENT_H_
