#ifndef _TOPIC_H_
#define _TOPIC_H_

#include "client.h"
#include "pthread.h"
#include "uthash.h"

#define BPSP_TOK_END_TOPIC "/"

typedef struct topic__hash_node topic__hash_node;
typedef struct topic__node topic__node;

/**
 *  Hash Table point to next node level by appropriate token
 *
 */
struct topic__hash_node {
    char* token;
    topic__node* node;

    /** uthash.h **/
    UT_hash_handle hh;
};

/**
 *  Node topic in Tree
 *
 */
struct topic__node {
    bpsp__subscriber* subs;  // all subscribers match until end current token

    topic__node* sl_node;  // single-level node
    topic__node* ml_node;  // multi-level node

    /** node hash table **/
    topic__hash_node* nodes;
};

typedef struct {
    pthread_mutex_t _mutex;
    topic__node root;
} bpsp__topic_tree;

#endif  // _TOPIC_H_
