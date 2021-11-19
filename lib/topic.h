#ifndef _TOPIC_H_
#define _TOPIC_H_

#include "bpsp.h"
#include "pthread.h"
#include "status.h"
#include "utarray.h"
#include "uthash.h"

#define BPSP_TOK_END_TOPIC "/"

typedef struct topic__hash_node topic__hash_node;
typedef struct topic__node topic__node;
typedef struct bpsp__topic_tree bpsp__topic_tree;

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
    /* UT_array* subs;  // all subscribers match until end current token */
    subscriber__hash* subs;
    int last_sub_pos;

    topic__node* sl_node;  // single-level node
    topic__node* ml_node;  // multi-level node
    topic__node* parent;   // ref to parent

    /** node hash table **/
    topic__hash_node* nodes;
};

struct bpsp__topic_tree {
    pthread_mutex_t mutex;
    pthread_rwlock_t rw_lock;
    topic__node root;
};

// testing
status__err topic__extract_token(char* topic, int* n_tok, char** first_tok);

void topic__free_hash_node_elt(topic__hash_node* hsh);
status__err topic__init_node(topic__node* node);
topic__node* topic__new_node();
void topic__dtor_node(topic__node* node);
void topic__free_node(topic__node* node);
status__err topic__init_tree(bpsp__topic_tree* tree);
bpsp__topic_tree* topic__new_tree();
void topic__dtor_tree(bpsp__topic_tree* tree);
void topic__free_tree(bpsp__topic_tree* tree);
status__err topic__add_subscriber(bpsp__topic_tree* tree, bpsp__subscriber* sub);
status__err topic__del_subscriber(bpsp__topic_tree* tree, bpsp__subscriber* sub, uint8_t lock);
UT_array* topic__tree_find_subscribers(bpsp__topic_tree* tree, char* topic, uint8_t lock);

void topic__print_tree(bpsp__topic_tree* tree);

#endif  // _TOPIC_H_
