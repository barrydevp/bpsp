#include "client.h"

#include <assert.h>
#include <pthread.h>

#include "broker.h"
#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"
#include "utarray.h"

void subscriber__ctor(void* elt) {
    ASSERT_ARG(elt, __EMPTY__);
    bpsp__subscriber* sub = (bpsp__subscriber*)elt;

    memset(sub, 0, sizeof(*sub));
}

bpsp__subscriber* subscriber__new(char* topic, bpsp__client* client, topic__node* node) {
    ASSERT_ARG(topic, NULL);

    bpsp__subscriber* sub = (bpsp__subscriber*)mem__malloc(sizeof(bpsp__subscriber));

    ASSERT_ARG(sub, NULL);

    subscriber__ctor((void*)sub);

    sub->topic = mem__strdup(topic);
    sub->client = client;
    sub->node = node;

    return sub;
}

void subscriber__copy(void* _dst, const void* _src) {
    bpsp__subscriber* dst = (bpsp__subscriber*)_dst;
    const bpsp__subscriber* src = (bpsp__subscriber*)_src;

    if (src->topic) {
        dst->topic = mem__strdup(src->topic);
    }

    dst->client = src->client;
    dst->node = src->node;
}

void subscriber__dtor(void* _elt) {
    bpsp__subscriber* elt = (bpsp__subscriber*)_elt;

    if (!elt) {
        return;
    }

    mem__free(elt->topic);
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

    utarray_new(c->subs, &bpsp__subscriber_icd);
    pthread_mutex_init(&c->cli_mutex, NULL);
    pthread_cond_init(&c->ref_cond, NULL);
    c->in_frame = frame__new();
    assert(c->in_frame);
    c->out_frame = frame__new();
    assert(c->out_frame);
}

bpsp__client* client__new(bpsp__connection* conn) {
    ASSERT_ARG(conn, NULL);

    bpsp__client* c = (bpsp__client*)mem__malloc(sizeof(bpsp__client));

    if (!c) {
        log__error("Cannot malloc() client");

        return NULL;
    }

    client__init((void*)c);

    c->conn = conn;

    return c;
}

void client__copy(void* _dst, const void* _src) {
    bpsp__client* dst = (bpsp__client*)_dst;
    const bpsp__client* src = (bpsp__client*)_src;

    utarray_new(dst->subs, &bpsp__subscriber_icd);
    utarray_concat(dst->subs, src->subs);

    if (src->conn) {
        dst->conn = net__dup(src->conn);
    }
}

void client__dtor(void* _elt) {
    bpsp__client* elt = (bpsp__client*)_elt;

    if (!elt) {
        return;
    }

    // TODO: remove sub from topic_tree(prune if need), and free subs array
    net__free(elt->conn);

    if (elt->subs) {
        utarray_free(elt->subs);
    }

    pthread_mutex_destroy(&elt->cli_mutex);
    pthread_cond_destroy(&elt->ref_cond);
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

    pthread_mutex_lock(&client->cli_mutex);
    while (client->ref_count > 0) {
        pthread_cond_wait(&client->ref_cond, &client->cli_mutex);
    }

    s = net__close(client->conn);

    pthread_mutex_unlock(&client->cli_mutex);

    ASSERT_BPSP_OK(s);

    client__free(client);

    return s;
}

void client__inc_ref(bpsp__client* client) {
    pthread_mutex_lock(&client->cli_mutex);
    client->ref_count += 1;
    pthread_mutex_unlock(&client->cli_mutex);
}

void client__dec_ref(bpsp__client* client) {
    pthread_mutex_lock(&client->cli_mutex);
    client->ref_count -= 1;

    if (client->ref_count == 0) {
        pthread_cond_signal(&client->ref_cond);
    }
    pthread_mutex_unlock(&client->cli_mutex);
}

status__err client__read(bpsp__client* client) {
    ASSERT_ARG(client, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    client__inc_ref(client);

    s = frame__read(client->conn, client->in_frame);

    client__dec_ref(client);

    return s;
}

status__err client__write(bpsp__client* client) {
    ASSERT_ARG(client, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    client__inc_ref(client);

    s = frame__write(client->conn, client->out_frame);

    client__dec_ref(client);

    return s;
}
