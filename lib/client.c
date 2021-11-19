#include "client.h"

#include <assert.h>
#include <pthread.h>

#include "broker.h"
#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"
#include "utarray.h"
#include "util.h"

void subscriber__free_hash_elt(subscriber__hash* hsh) {
    ASSERT_ARG(hsh, __EMPTY__);

    if (hsh->key) {
        mem__free(hsh->key);
    }

    mem__free(hsh);
}

subscriber__hash* subscriber__new_hash_elt(char* key, bpsp__subscriber* sub) {
    ASSERT_ARG(key, NULL);
    ASSERT_ARG(sub, NULL);

    subscriber__hash* hsh_sub = (subscriber__hash*)mem__malloc(sizeof(subscriber__hash));

    ASSERT_ARG(hsh_sub, NULL);

    hsh_sub->sub = sub;
    hsh_sub->key = mem__strdup(key);

    return hsh_sub;
}

void subscriber__ctor(void* elt) {
    /* ASSERT_ARG(elt, __EMPTY__); */
    bpsp__subscriber* sub = (bpsp__subscriber*)elt;

    memset(sub, 0, sizeof(*sub));
}

char* subscriber__gen_id(char* topic, bpsp__client* client) {
    size_t cli_id_len = strlen(client->_id);
    size_t topic_len = strlen(topic);
    char* _id = mem__malloc(sizeof(char) * (cli_id_len + topic_len + 2));

    if (!_id) {
        return NULL;
    }

    mem__memmove(_id, client->_id, cli_id_len);
    *(_id + cli_id_len) = '/';
    mem__memmove(_id + cli_id_len + 1, topic, topic_len);
    *(_id + cli_id_len + topic_len + 1) = '\0';

    return _id;
}

bpsp__subscriber* subscriber__new(char* topic, bpsp__client* client, topic__node* node) {
    ASSERT_ARG(topic, NULL);
    ASSERT_ARG(client, NULL);

    bpsp__subscriber* sub = (bpsp__subscriber*)mem__malloc(sizeof(bpsp__subscriber));

    ASSERT_ARG(sub, NULL);

    subscriber__ctor((void*)sub);

    sub->_id = subscriber__gen_id(topic, client);

    if (!sub->_id) {
        mem__free(sub);
        return NULL;
    }

    sub->client = client;
    sub->node = node;

    return sub;
}

void subscriber__copy(void* _dst, const void* _src) {
    bpsp__subscriber* dst = (bpsp__subscriber*)_dst;
    const bpsp__subscriber* src = (bpsp__subscriber*)_src;

    if (src->_id) {
        dst->_id = mem__strdup(src->_id);
    }

    dst->client = src->client;
    dst->node = src->node;
}

void subscriber__dtor(void* _elt) {
    bpsp__subscriber* elt = (bpsp__subscriber*)_elt;

    if (!elt) {
        return;
    }

    if (elt->_id) {
        mem__free(elt->_id);
    }
}

void subscriber__free(bpsp__subscriber* sub) {
    if (!sub) {
        return;
    }

    subscriber__dtor((void*)sub);
    mem__free(sub);
}

void client__init(void* elt) {
    ASSERT_ARG(elt, __EMPTY__);

    bpsp__client* c = (bpsp__client*)elt;
    memset(c, 0, sizeof(*c));

    rand_str(c->_id, BPSP_CLIENT_ID_LEN);
    /* utarray_new(c->subs, &bpsp__subscriber_icd); */
    pthread_cond_init(&c->ref_cond, NULL);
    pthread_mutex_init(&c->cli_mutex, NULL);
    pthread_rwlock_init(&c->rw_lock, NULL);
    c->in_frame = frame__new();
    assert(c->in_frame);
    c->out_frame = frame__new();
    assert(c->out_frame);
}

bpsp__client* client__new(bpsp__connection* conn, bpsp__broker* broker) {
    /* ASSERT_ARG(conn, NULL); */
    /* ASSERT_ARG(broker, NULL); */

    bpsp__client* c = (bpsp__client*)mem__malloc(sizeof(bpsp__client));

    if (!c) {
        log__error("Cannot malloc() client");

        return NULL;
    }

    client__init((void*)c);

    c->conn = conn;
    c->broker = broker;

    return c;
}

void client__copy(void* _dst, const void* _src) {
    bpsp__client* dst = (bpsp__client*)_dst;
    const bpsp__client* src = (bpsp__client*)_src;

    /* utarray_new(dst->subs, &bpsp__subscriber_icd); */
    /* utarray_concat(dst->subs, src->subs); */

    if (src->subs) {
        subscriber__hash *sub, *_sub, *tmp;
        HASH_ITER(hh, src->subs, _sub, tmp) {
            sub = subscriber__new_hash_elt(_sub->key, _sub->sub);
            HASH_ADD_STR(dst->subs, key, sub);
        }
    }

    if (src->conn) {
        /* dst->conn = net__dup(src->conn); */
        dst->conn = src->conn;
    }
}

void client__unsub_all(bpsp__client* client) {
    ASSERT_ARG(client, __EMPTY__);
    ASSERT_ARG(client->broker, __EMPTY__);

    pthread_mutex_lock(&(client->broker->topic_tree->mutex));
    subscriber__hash *_sub, *tmp;
    HASH_ITER(hh, client->subs, _sub, tmp) {
        HASH_DEL(client->subs, _sub);  // delete it (users advances to next)
        topic__del_subscriber(client->broker->topic_tree, _sub->sub, 0);
        subscriber__free(_sub->sub);
        subscriber__free_hash_elt(_sub);  // free it
    }
    pthread_mutex_lock(&(client->broker->topic_tree->mutex));

    HASH_CLEAR(hh, client->subs);
}

void client__dtor(void* _elt) {
    bpsp__client* client = (bpsp__client*)_elt;

    if (!client) {
        return;
    }

    // TODO: remove sub from topic_tree(prune if need), and free subs array
    net__free(client->conn);

    /* if (elt->subs) { */
    /*     utarray_free(elt->subs); */
    /* } */

    if (client->subs) {
        subscriber__hash *_sub, *tmp;
        HASH_ITER(hh, client->subs, _sub, tmp) {
            HASH_DEL(client->subs, _sub);     // delete it (users advances to next)
            subscriber__free_hash_elt(_sub);  // free it
        }

        HASH_CLEAR(hh, client->subs);
    }

    if (client->in_frame) {
        frame__free(client->in_frame);
    }

    if (client->out_frame) {
        frame__free(client->out_frame);
    }

    pthread_cond_destroy(&client->ref_cond);
    pthread_mutex_destroy(&client->cli_mutex);
    pthread_rwlock_destroy(&client->rw_lock);
}

void client__free(bpsp__client* client) {
    if (!client) {
        return;
    }

    client__dtor((void*)client);

    mem__free(client);
}

status__err client__close(bpsp__client* client) {
    status__err s = BPSP_OK;

    ASSERT_ARG(client, s);

    s = net__close(client->conn);

    return s;
}

status__err client__destroy(bpsp__client* client) {
    ASSERT_ARG(client, BPSP_OK);

    status__err s = BPSP_OK;

    pthread_rwlock_wrlock(&client->rw_lock);
    /* pthread_mutex_lock(&client->cli_mutex); */
    /* while (client->ref_count > 0) { */
    /*     pthread_cond_wait(&client->ref_cond, &client->cli_mutex); */
    /* } */

    s = net__close(client->conn);

    /* pthread_mutex_unlock(&client->cli_mutex); */
    pthread_rwlock_unlock(&client->rw_lock);

    ASSERT_BPSP_OK(s);

    client__free(client);

    return s;
}

void client__inc_ref(bpsp__client* client, uint8_t lock) {
    if (lock) {
        pthread_mutex_lock(&client->cli_mutex);
    }
    client->ref_count += 1;

    if (lock) {
        pthread_mutex_unlock(&client->cli_mutex);
    }
}

void client__dec_ref(bpsp__client* client, uint8_t lock) {
    if (lock) {
        pthread_mutex_lock(&client->cli_mutex);
    }
    client->ref_count -= 1;

    if (client->ref_count == 0) {
        pthread_cond_signal(&client->ref_cond);
    }
    if (lock) {
        pthread_mutex_unlock(&client->cli_mutex);
    }
}

status__err client__recv(bpsp__client* client) {
    ASSERT_ARG(client, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;
    pthread_mutex_lock(&client->cli_mutex);

    client__inc_ref(client, 0);

    if (!client->conn) {
        // client has close
        client__dec_ref(client, 0);
        pthread_mutex_unlock(&client->cli_mutex);

        return BPSP_CONNECTION_CLOSED;
    }

    pthread_mutex_unlock(&client->cli_mutex);

    s = frame__recv(client->conn, client->in_frame);

    client__dec_ref(client, 1);

    return s;
}

uint8_t client__is_close(bpsp__client* client) {
    //
    return !client || !client->conn;
}

status__err client__send(bpsp__client* client) {
    ASSERT_ARG(client, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    pthread_rwlock_rdlock(&client->rw_lock);
    /*     pthread_mutex_lock(&client->cli_mutex); */
    /*     client__inc_ref(client, 0); */

    if (client__is_close(client)) {
        // client has close
        /* client__dec_ref(client, 0); */
        /* pthread_mutex_unlock(&client->cli_mutex); */
        pthread_rwlock_wrlock(&client->rw_lock);

        return BPSP_CONNECTION_CLOSED;
    }

    /*     pthread_mutex_unlock(&client->cli_mutex); */

    pthread_mutex_lock(&client->cli_mutex);

    s = frame__send(client->conn, client->out_frame);

    pthread_mutex_unlock(&client->cli_mutex);

    /*     client__dec_ref(client, 1); */

    return s;
}

status__err client__read(bpsp__client* client) {
    //
    return client__recv(client);
}

status__err client__write(bpsp__client* client, bpsp__frame* frame) {
    ASSERT_ARG(client, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    pthread_rwlock_rdlock(&client->rw_lock);
    /*     pthread_mutex_lock(&client->cli_mutex); */
    /*     client__inc_ref(client, 0); */

    if (client__is_close(client)) {
        // client has close
        /* client__dec_ref(client, 0); */
        /* pthread_mutex_unlock(&client->cli_mutex); */
        pthread_rwlock_wrlock(&client->rw_lock);

        return BPSP_CONNECTION_CLOSED;
    }

    pthread_mutex_unlock(&client->cli_mutex);

    s = frame__copy(client->out_frame, frame, 1);

    IFN_OK(s) {
        pthread_mutex_unlock(&client->cli_mutex);

        return s;
    }

    s = frame__send(client->conn, client->out_frame);

    pthread_mutex_unlock(&client->cli_mutex);

    /*     client__dec_ref(client, 1); */

    return s;
}

status__err client_sub(bpsp__client* client, char* topic, uint8_t lock) {
    ASSERT_ARG(client, BPSP_INVALID_ARG);
    ASSERT_ARG(topic, BPSP_INVALID_TOPIC);
    ASSERT_ARG(client->broker->topic_tree, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    bpsp__subscriber* sub = NULL;
    // locking client for modify the shared subs hash table, necessary? we only use one reader at a time, it wasted?
    if (lock) {
        pthread_mutex_lock(&client->cli_mutex);
    }

    subscriber__hash* hsh_sub = NULL;
    HASH_FIND_STR(client->subs, topic, hsh_sub);

    if (!hsh_sub) {
        sub = subscriber__new(topic, client, NULL);
        hsh_sub = subscriber__new_hash_elt(topic, sub);
        HASH_ADD_STR(client->subs, key, hsh_sub);
    }

    if (lock) {
        pthread_mutex_unlock(&client->cli_mutex);
    }

    if (sub) {
        s = topic__add_subscriber(client->broker->topic_tree, sub);
    }

    return s;
}

status__err client_unsub(bpsp__client* client, char* topic, uint8_t lock) {
    ASSERT_ARG(client, BPSP_INVALID_ARG);
    ASSERT_ARG(topic, BPSP_INVALID_TOPIC);
    ASSERT_ARG(client->broker->topic_tree, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    bpsp__subscriber* sub = NULL;
    // locking client for modify the shared subs hash table, necessary? we only use one reader at a time, it wasted?
    if (lock) {
        pthread_mutex_lock(&client->cli_mutex);
    }

    subscriber__hash* hsh_sub = NULL;
    HASH_FIND_STR(client->subs, topic, hsh_sub);

    if (hsh_sub) {
        sub = hsh_sub->sub;
        HASH_DEL(client->subs, hsh_sub);
        subscriber__free_hash_elt(hsh_sub);
    }

    if (lock) {
        pthread_mutex_unlock(&client->cli_mutex);
    }

    if (sub) {
        s = topic__del_subscriber(client->broker->topic_tree, sub, 1);
    }

    subscriber__free(sub);

    return s;
}
