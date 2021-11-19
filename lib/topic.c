#include "topic.h"

#include <string.h>

#include "bpsp.h"
#include "client.h"
#include "log.h"
#include "mem.h"
#include "status.h"
#include "utarray.h"
#include "uthash.h"

void topic__free_hash_node_elt(topic__hash_node* hsh) {
    ASSERT_ARG(hsh, __EMPTY__);

    if (hsh->node) {
        topic__free_node(hsh->node);
    }

    if (hsh->token) {
        mem__free(hsh->token);
    }

    mem__free(hsh);
}

void topic__dtor_node(topic__node* node) {
    ASSERT_ARG(node, __EMPTY__);

    if (node->ml_node) {
        topic__free_node(node->ml_node);
    }

    if (node->sl_node) {
        topic__free_node(node->sl_node);
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

        HASH_CLEAR(hh, node->nodes);
    }

    if (node->nodes) {
        topic__hash_node *_node, *tmp;
        HASH_ITER(hh, node->nodes, _node, tmp) {
            HASH_DEL(node->nodes, _node);     /* delete it (users advances to next) */
            topic__free_hash_node_elt(_node); /* free it */
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
    subscriber__hash* hsh_sub = subscriber__new_hash_elt(sub->_id, sub);

    ASSERT_ARG(hsh_sub, BPSP_NO_MEMORY);

    HASH_ADD_STR(node->subs, key, hsh_sub);

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

    pthread_mutex_destroy(&tree->tree_mutex);
}

void topic__free_tree(bpsp__topic_tree* tree) {
    ASSERT_ARG(tree, __EMPTY__);

    topic__dtor_tree(tree);

    mem__free(tree);
}

status__err topic__init_tree(bpsp__topic_tree* tree) {
    ASSERT_ARG(tree, BPSP_INVALID_ARG);
    memset(tree, 0, sizeof(*tree));

    pthread_mutex_init(&tree->tree_mutex, NULL);

    status__err s = topic__init_node(&tree->root);

    IFN_OK(s) { pthread_mutex_destroy(&tree->tree_mutex); }

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

status__err topic__extract_token(char* topic, uint16_t* n_tok, char** first_tok) {
    // invalid topic, must be at least one character
    ASSERT_ARG(strlen(topic), BPSP_INVALID_TOPIC);

    // invalid topic, cannot start and end with '/'
    ASSERT_ARG(*topic != '/' && *(topic + strlen(topic) - 1) != '/', BPSP_INVALID_TOPIC);

    char* _topic = mem__strdup(topic);

    char* tok = strtok(_topic, "/");
    *first_tok = tok;
    *n_tok = 0;
    char* next_tok;

    while (tok != NULL) {
        *n_tok += 1;
        next_tok = strtok(NULL, "/");
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

status__err topic__add_subscriber(bpsp__topic_tree* tree, bpsp__subscriber* sub) {
    ASSERT_ARG(tree, BPSP_INVALID_ARG);
    ASSERT_ARG(sub, BPSP_INVALID_ARG);
    ASSERT_ARG(sub->_id, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    char* first_tok;
    uint16_t n_tok = 0;
    char* topic = sub->_id + sizeof(sub->client->_id) + 1;
    s = topic__extract_token(topic, &n_tok, &first_tok);

    ASSERT_BPSP_OK(s);

    pthread_mutex_lock(&tree->tree_mutex);

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

    pthread_mutex_unlock(&tree->tree_mutex);

    return s;
RET_ERROR:
    mem__free(first_tok);

    pthread_mutex_unlock(&tree->tree_mutex);

    return s;
}

status__err topic__del_subscriber(bpsp__subscriber* sub) {
    status__err s = BPSP_OK;

    if (!sub->node) {
        return s;
    }

    topic__node* node = sub->node;
    if (!sub->_id) {
        log__error("Illegal subscriber to delete");
        return BPSP_INVALID_SUBSCRIBER;
    }

    /* utarray_erase(node->subs, sub->idx_on_node, 1); */
    subscriber__hash* hsh = NULL;
    HASH_FIND_STR(node->subs, sub->_id, hsh);

    if (hsh) {
        HASH_DEL(node->subs, hsh);
        subscriber__free_hash_elt(hsh);
    }

    return s;
}

void print_node(topic__node* node, char* token, int deep) {
    if (node) {
        for (int i = 0; i < deep; i++) {
            printf("   │");
        }
        /* printf("─ %s (%d)\n", token, utarray_len(node->subs)); */
        printf("─ %s (%d)\n", token, HASH_COUNT(node->subs));

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
}
