#include "topic.h"

#include <string.h>

#include "bpsp.h"
#include "client.h"
#include "log.h"
#include "mem.h"
#include "status.h"
#include "utarray.h"
#include "uthash.h"

int topic__node_num_child_nodes(topic__node* node) {
    ASSERT_ARG(node, 0);

    int num = HASH_COUNT(node->nodes);

    if (node->ml_node) {
        num++;
    }

    if (node->sl_node) {
        num++;
    }

    return num;
}

int topic__node_num_subs(topic__node* node) {
    ASSERT_ARG(node, 0);

    return HASH_COUNT(node->subs);
}

uint8_t topic__node_is_empty(topic__node* node) {
    ASSERT_ARG(node, 1);

    if (topic__node_num_subs(node) == 0 && topic__node_num_child_nodes(node) == 0) {
        return 1;
    }

    return 0;
}

void topic__free_hash_node_elt(topic__hash_node* hsh) {
    ASSERT_ARG(hsh, __EMPTY__);

    /* if (hsh->node) { */
    /*     topic__free_node(hsh->node); */
    /* } */

    if (hsh->token) {
        mem__free(hsh->token);
    }

    mem__free(hsh);
}

void topic__dtor_node(topic__node* node) {
    ASSERT_ARG(node, __EMPTY__);

    if (node->ml_node) {
        topic__free_node(node->ml_node);
        node->ml_node = NULL;
    }

    if (node->sl_node) {
        topic__free_node(node->sl_node);
        node->sl_node = NULL;
    }

    /* if (node->subs) { */
    /*     utarray_free(node->subs); */
    /* } */

    if (node->subs) {
        subscriber__hash *_sub, *tmp;
        HASH_ITER(hh, node->subs, _sub, tmp) {
            HASH_DEL(node->subs, _sub);       // delete it (users advances to next)
            subscriber__free_hash_elt(_sub);  // free it
        }

        HASH_CLEAR(hh, node->subs);
    }

    if (node->nodes) {
        topic__hash_node *_node, *tmp;
        HASH_ITER(hh, node->nodes, _node, tmp) {
            HASH_DEL(node->nodes, _node);  // delete it (users advances to next)
            topic__free_node(_node->node);
            topic__free_hash_node_elt(_node);  // free it
        }

        HASH_CLEAR(hh, node->nodes);
    }
}

void topic__free_node(topic__node* node) {
    ASSERT_ARG(node, __EMPTY__);

    topic__dtor_node(node);
    mem__free(node);
}

status__err topic__init_node(topic__node* node) {
    ASSERT_ARG(node, BPSP_INVALID_ARG);

    memset(node, 0, sizeof(*node));

    status__err s = BPSP_OK;

    /* utarray_new(node->subs, &bpsp__subscriber_icd);  // all subscribers match until end current token */

    return s;
}

topic__node* topic__new_node() {
    topic__node* node = (topic__node*)mem__malloc(sizeof(topic__node));

    ASSERT_ARG(node, NULL);
    status__err s = topic__init_node(node);

    IFN_OK(s) {
        mem__free(node);

        return NULL;
    }

    return node;
}

status__err topic__node_add_sub(topic__node* node, bpsp__subscriber* sub) {
    ASSERT_ARG(node, BPSP_INVALID_ARG);
    ASSERT_ARG(sub, BPSP_INVALID_ARG);

    sub->node = node;
    /* utarray_push_back(node->subs, sub); */
    subscriber__hash* hsh_sub = NULL;

    HASH_FIND_STR(node->subs, sub->_id, hsh_sub);

    if (1 || !hsh_sub) {
        subscriber__hash* hsh_sub = subscriber__new_hash_elt(sub->_id, sub);
        ASSERT_ARG(hsh_sub, BPSP_NO_MEMORY);

        HASH_ADD_STR(node->subs, key, hsh_sub);
    } else {
        return BPSP_TREE_DUP_SUBSCRIBER;
    }

    /* bpsp__subscriber* _sub = (bpsp__subscriber*)utarray_back(node->subs); */

    return BPSP_OK;
}

topic__hash_node* topic__new_hash_node(char* token) {
    topic__hash_node* hsh_node = (topic__hash_node*)mem__malloc(sizeof(topic__hash_node));

    ASSERT_ARG(hsh_node, NULL);

    topic__node* node = topic__new_node();

    if (!node) {
        topic__free_hash_node_elt(hsh_node);

        return NULL;
    }

    hsh_node->node = node;
    hsh_node->token = mem__strdup(token);

    return hsh_node;
}

void topic__dtor_tree(bpsp__topic_tree* tree) {
    ASSERT_ARG(tree, __EMPTY__);

    topic__dtor_node(&tree->root);

    pthread_mutex_destroy(&tree->mutex);
    pthread_rwlock_destroy(&tree->rw_lock);
}

void topic__free_tree(bpsp__topic_tree* tree) {
    ASSERT_ARG(tree, __EMPTY__);

    topic__dtor_tree(tree);

    mem__free(tree);
}

status__err topic__init_tree(bpsp__topic_tree* tree) {
    ASSERT_ARG(tree, BPSP_INVALID_ARG);
    memset(tree, 0, sizeof(*tree));

    pthread_mutex_init(&tree->mutex, NULL);
    pthread_rwlock_init(&tree->rw_lock, NULL);

    status__err s = topic__init_node(&tree->root);

    IFN_OK(s) {
        pthread_mutex_destroy(&tree->mutex);
        pthread_rwlock_destroy(&tree->rw_lock);
    }

    return s;
}

bpsp__topic_tree* topic__new_tree() {
    bpsp__topic_tree* tree = (bpsp__topic_tree*)mem__malloc(sizeof(bpsp__topic_tree));

    ASSERT_ARG(tree, NULL);

    status__err s = topic__init_tree(tree);

    IFN_OK(s) {
        mem__free(tree);

        return NULL;
    }

    return tree;
}

status__err topic__extract_token(char* topic, int* n_tok, char** first_tok) {
    // invalid topic, must be at least one character
    ASSERT_ARG(strlen(topic), BPSP_INVALID_TOPIC);

    // invalid topic, cannot start and end with '/'
    ASSERT_ARG(*topic != '/' && *(topic + strlen(topic) - 1) != '/', BPSP_INVALID_TOPIC);

    char* _topic = mem__strdup(topic);
    char* saveptr = NULL;

    char* tok = strtok_r(_topic, "/", &saveptr);
    *first_tok = tok;
    *n_tok = 0;
    char* next_tok;

    while (tok != NULL) {
        *n_tok += 1;
        next_tok = strtok_r(NULL, "/", &saveptr);
        // invalid multilevel topic, must have at most one '*' char and occur at the end of topic
        if (*tok == '*' && strlen(tok) == 1 && next_tok) {
            goto RET_ERROR;
        }
        tok = next_tok;
    }

    // cannot extract token
    /* ASSERT_ARG(*n_tok, BPSP_INVALID_TOPIC); */
    if (!*n_tok) {
        goto RET_ERROR;
    }

    return BPSP_OK;
RET_ERROR:
    *first_tok = NULL;
    mem__free(_topic);

    return BPSP_INVALID_TOPIC;
}

// carefully when using this function, you must check the total of retrive token
char* topic__next_token(char* cur_tok) { return cur_tok + strlen(cur_tok) + 1; }

void topic__token_to_array(char* arr[], char* first_tok, int n_tok) {
    ASSERT_ARG(n_tok, __EMPTY__);
    arr[0] = first_tok;
    for (int i = 1; i < n_tok; i++) {
        first_tok = topic__next_token(first_tok);
        arr[i] = first_tok;
    }
}

status__err topic__add_subscriber(bpsp__topic_tree* tree, bpsp__subscriber* sub) {
    ASSERT_ARG(tree, BPSP_INVALID_ARG);
    ASSERT_ARG(sub, BPSP_INVALID_ARG);
    ASSERT_ARG(sub->_id, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    char* first_tok;
    int n_tok = 0;
    char* topic = subscriber__get_topic(sub);
    s = topic__extract_token(topic, &n_tok, &first_tok);

    ASSERT_BPSP_OK(s);

    /* pthread_mutex_lock(&tree->mutex); */
    int ok = pthread_rwlock_wrlock(&tree->rw_lock);
    printf("%s, n_tok %d\n", topic, n_tok);

    topic__node* cur_node = &tree->root;

    char* cur_tok = first_tok;

    while (n_tok > 0) {
        if (*cur_tok == '*' && strlen(cur_tok) == 1) {
            if (!cur_node->ml_node) {
                topic__node* node = topic__new_node();

                if (!node) {
                    //
                    s = BPSP_NO_MEMORY;
                    goto RET_ERROR;
                }

                cur_node->ml_node = node;
                node->parent = cur_node;
            }
            cur_node = cur_node->ml_node;

            // set n_tok = 1 to end this work and assign subscriber to ml-node because we met the token `*`
            n_tok = 1;
        } else if (*cur_tok == '+' && strlen(cur_tok) == 1) {
            if (!cur_node->sl_node) {
                topic__node* node = topic__new_node();

                if (!node) {
                    //
                    s = BPSP_NO_MEMORY;
                    goto RET_ERROR;
                }

                cur_node->sl_node = node;
                node->parent = cur_node;
            }
            cur_node = cur_node->sl_node;

        } else {
            topic__hash_node* hsh;

            HASH_FIND_STR(cur_node->nodes, cur_tok, hsh);

            if (!hsh) {
                hsh = topic__new_hash_node(cur_tok);
                if (!hsh) {
                    break;
                }
                HASH_ADD_STR(cur_node->nodes, token, hsh);

                hsh->node->parent = cur_node;
            }
            cur_node = hsh->node;
        }

        n_tok--;
        cur_tok = topic__next_token(cur_tok);

        if (!n_tok) {
            s = topic__node_add_sub(cur_node, sub);

            IFN_OK(s) {
                //
                goto RET_ERROR;
            }
        }
    }

    mem__free(first_tok);

    /* pthread_mutex_unlock(&tree->mutex); */
    pthread_rwlock_unlock(&tree->rw_lock);

    return s;
RET_ERROR:
    mem__free(first_tok);

    /* pthread_mutex_unlock(&tree->mutex); */
    pthread_rwlock_unlock(&tree->rw_lock);

    return s;
}

void topic__tree_prune_node(bpsp__topic_tree* tree, topic__node* leaf_node, char* tokens[], int n_tok, uint8_t lock) {
    topic__node* cur_node = leaf_node;
    topic__node* to_del = NULL;

    for (int i = n_tok - 1; i >= 0 && topic__node_is_empty(cur_node); i--) {
        to_del = cur_node;
        cur_node = cur_node->parent;

        char* tok = tokens[i];
        if (*tok == '*' && strlen(tok) == 1) {
            cur_node->ml_node = NULL;
        } else if (*tok == '+' && strlen(tok) == 1) {
            cur_node->sl_node = NULL;
        } else {
            topic__hash_node* hsh_node = NULL;
            HASH_FIND_STR(cur_node->nodes, tok, hsh_node);
            if (hsh_node) {
                HASH_DEL(cur_node->nodes, hsh_node);
                topic__free_hash_node_elt(hsh_node);
            } else {
                log__warn("Unexpected prune node, cannot found node in parent hash node tbl.");
            }
        }

        topic__free_node(to_del);
    }
}

status__err topic__del_subscriber(bpsp__topic_tree* tree, bpsp__subscriber* sub, uint8_t lock) {
    status__err s = BPSP_OK;

    ASSERT_ARG(sub->node, s);

    topic__node* node = sub->node;
    if (!sub->_id) {
        log__error("Illegal subscriber to delete");
        return BPSP_INVALID_SUBSCRIBER;
    }

    char* first_tok;
    int n_tok = 0;
    char* topic = subscriber__get_topic(sub);
    s = topic__extract_token(topic, &n_tok, &first_tok);
    char* tokens[n_tok];
    topic__token_to_array(tokens, first_tok, n_tok);

    /* ASSERT_BPSP_OK(s); */

    if (lock) {
        /* pthread_mutex_lock(&tree->mutex); */
        pthread_rwlock_wrlock(&tree->rw_lock);
    }
    /* utarray_erase(node->subs, sub->idx_on_node, 1); */
    subscriber__hash* hsh = NULL;
    HASH_FIND_STR(node->subs, sub->_id, hsh);

    if (hsh) {
        HASH_DEL(node->subs, hsh);
        sub->node = NULL;
        subscriber__free_hash_elt(hsh);
        topic__tree_prune_node(tree, node, tokens, n_tok, 0);
    }

    if (lock) {
        /* pthread_mutex_unlock(&tree->mutex); */
        pthread_rwlock_unlock(&tree->rw_lock);
    }

    mem__free(first_tok);

    return s;
}

void print_node(topic__node* node, char* token, int deep) {
    if (node) {
        for (int i = 0; i < deep; i++) {
            printf("   │");
        }
        /* printf("─ %s (%d)\n", token, utarray_len(node->subs)); */
        printf("─ %s (%d)\n", token, topic__node_num_subs(node));

        if (node->ml_node) {
            print_node(node->ml_node, "*", deep + 1);
        }

        if (node->sl_node) {
            print_node(node->sl_node, "+", deep + 1);
        }

        topic__hash_node *_node, *tmp;
        HASH_ITER(hh, node->nodes, _node, tmp) {
            //
            print_node(_node->node, _node->token, deep + 1);
        }
    }
}

void topic__print_tree(bpsp__topic_tree* tree) {
    printf("[TREE]\n");

    if (tree) {
        print_node(&tree->root, "#root", 0);

    } else {
        printf("NULL\n");
    }
    printf("\n");
}

status__err hash__subscriber_copy_to_array(UT_array* arr, subscriber__hash* subs) {
    ASSERT_ARG(arr, BPSP_INVALID_ARG);

    ASSERT_ARG(subs, BPSP_OK);

    subscriber__hash *_sub, *tmp;
    HASH_ITER(hh, subs, _sub, tmp) {
        //

        utarray_push_back(arr, _sub);
    }

    return BPSP_OK;
}

UT_array* topic__node_find_subscribers(topic__node* node, char* first_tok, int n_tok) {
    ASSERT_ARG(node, NULL);
    ASSERT_ARG(first_tok, NULL);
    ASSERT_ARG(n_tok >= 0, NULL);

    UT_array* subs = NULL;
    utarray_new(subs, &bpsp__subscriber_icd);

    if (n_tok == 0) {
        // n_tok == 0 so we has match all token, return all subscribers of this node.

        hash__subscriber_copy_to_array(subs, node->subs);

        return subs;
    }

    char* cur_tok = first_tok;
    topic__node* cur_node = node;
    topic__hash_node* hsh_node = NULL;

    status__err s = BPSP_OK;

    while (s == BPSP_OK && cur_node && n_tok > 0) {
        // check multi-level node first
        if (topic__node_num_subs(cur_node->ml_node)) {
            s = hash__subscriber_copy_to_array(subs, cur_node->ml_node->subs);
            IFN_OK(s) {
                log__warn("Cannot convert subscriber__hash of multi-level node to array");
                break;
            }
        }

        // get node in hash node if match, but no check or do smthing else
        HASH_FIND_STR(cur_node->nodes, cur_tok, hsh_node);

        cur_tok = topic__next_token(cur_tok);
        n_tok--;

        // find and add all subscriber from single-level node
        if (cur_node->sl_node) {
            UT_array* tmp_subs = topic__node_find_subscribers(cur_node->sl_node, cur_tok, n_tok);
            if (tmp_subs) {
                utarray_concat(subs, tmp_subs);
                utarray_free(tmp_subs);
            }
        }

        // after find in single-level node, we get back to operate on last found hash node
        if (!hsh_node) {
            // we not found node hash match the curent token, just break and end up finding
            cur_node = NULL;
            break;
        } else {
            // if we found, we dig into next level
            cur_node = hsh_node->node;
        }
    }

    // end up match all token, if cur_node != NULL, so this cur_node is the node which match the topic
    // put all subs of it into result
    if (cur_node && cur_node->subs) {
        hash__subscriber_copy_to_array(subs, cur_node->subs);
    }

    return subs;
}

UT_array* topic__tree_find_subscribers(bpsp__topic_tree* tree, char* topic, uint8_t lock) {
    ASSERT_ARG(tree, NULL);
    ASSERT_ARG(topic, NULL);

    /* utarray_new(subs, &bpsp__subscriber_icd); */

    char* first_tok;
    int n_tok = 0;
    status__err s = topic__extract_token(topic, &n_tok, &first_tok);

    if (lock) {
        /* pthread_mutex_lock(&tree->mutex); */
        pthread_rwlock_rdlock(&tree->rw_lock);
    }

    UT_array* subs = topic__node_find_subscribers(&tree->root, first_tok, n_tok);

    if (lock) {
        /* pthread_mutex_unlock(&tree->mutex); */
        pthread_rwlock_unlock(&tree->rw_lock);
    }

    mem__free(first_tok);

    return subs;
}
