#include "broker.h"

#include <assert.h>

#include "client.h"
#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"
#include "utarray.h"
#include "util.h"

broker__info* broker__info_new(char* name, char* version) {
    broker__info* info = (broker__info*)mem__malloc(sizeof(broker__info));

    ASSERT_ARG(info, NULL);

    info->name = strdup(name);
    info->version = strdup(version);

    return info;
}

void broker__info_free(broker__info* info) {
    ASSERT_ARG(info, __EMPTY__);

    if (info->name) {
        mem__free(info->name);
    }

    if (info->version) {
        mem__free(info->version);
    }

    mem__free(info);
}

status__err broker__close(bpsp__broker* broker, uint8_t lock) {
    ASSERT_ARG(broker, BPSP_OK);

    if (lock) {
        pthread_mutex_lock(&broker->mutex);
    }

    status__err s = net__close(broker->listener);

    broker->is_close = 1;

    if (lock) {
        pthread_mutex_unlock(&broker->mutex);
    }

    IFN_OK(s) {
        log__error("Unexpected close broker: %s", ERR_TEXT(s));

        return s;
    }

    return s;
}

void broker__free(bpsp__broker* broker) {
    ASSERT_ARG(broker, __EMPTY__);

    net__free(broker->listener);

    topic__free_tree(broker->topic_tree);

    pthread_mutex_destroy(&broker->mutex);

    pthread_rwlock_destroy(&broker->cli_rw_lock);

    mem__free(broker);
}

bpsp__broker* broker__new(const char* host, uint16_t port) {
    broker__info* info = broker__info_new(BROKER_DEFAULT_NAME, BROKER_DEFAULT_VERION);

    if (!info) {
        return NULL;
    }

    bpsp__broker* broker = broker__new_with_info(host, port, info);

    if (!broker) {
        broker__info_free(info);
        return NULL;
    }

    return broker;
}

bpsp__broker* broker__new_with_info(const char* host, uint16_t port, broker__info* info) {
    bpsp__broker* broker = (bpsp__broker*)mem__malloc(sizeof(bpsp__broker));

    if (!broker) {
        return NULL;
    }

    memset(broker, 0, sizeof(*broker));

    broker->info = info;

    if (pthread_mutex_init(&broker->mutex, NULL)) {
        log__error("Broker cannot init `mutex`");

        goto RET_ERROR;
    }

    if (pthread_rwlock_init(&broker->cli_rw_lock, NULL)) {
        log__error("Broker cannot init `cli_rw_lock`");

        goto RET_ERROR;
    }

    /* utarray_new(broker->clients, &bpsp__client_icd); */

    bpsp__topic_tree* tree = topic__new_tree();

    if (!tree) {
        goto RET_ERROR;
    }

    broker->topic_tree = tree;

    bpsp__connection* listener = net__listen(host, port);

    if (!listener) {
        log__error("Broker cannot listen on %s:%u", host, port);

        goto RET_ERROR;
    }

    broker->listener = listener;

    return broker;
RET_ERROR:
    broker__free(broker);

    return NULL;
}

void broker__destroy(bpsp__broker* broker) {
    ASSERT_ARG(broker, __EMPTY__);

    // is it necessary to holding lock?
    pthread_mutex_lock(&broker->mutex);

    broker__close(broker, 0);

    /* if (broker->clients) { */
    /*     utarray_free(broker->clients); */
    /* } */

    if (broker->info) {
        broker__info_free(broker->info);
    }

    if (broker->clients) {
        bpsp__client *cli, *tmp;
        HASH_ITER(hh, broker->clients, cli, tmp) {
            HASH_DEL(broker->clients, cli);
            client__destroy(cli);
        }
    }

    pthread_mutex_unlock(&broker->mutex);

    broker__free(broker);
}

bpsp__client* broker__accept(bpsp__broker* broker) {
    assert(broker);

    bpsp__connection* c_conn = net__accept(broker->listener);

    if (!c_conn) {
        log__error("NULL client__accept()");
        return NULL;
    }

    bpsp__client* client = client__new(c_conn, broker);

    if (!client) {
        log__error("NULL client__new()");
        goto RET_ERROR;
    }

    /* pthread_rwlock_wrlock(&broker->cli_rw_lock); */

    bpsp__client* dup_client_id = NULL;

    // unique client->_id
    HASH_FIND_STR(broker->clients, client->_id, dup_client_id);
    while (dup_client_id) {
        rand_str(client->_id, BPSP_CLIENT_ID_LEN);
        HASH_FIND_STR(broker->clients, client->_id, dup_client_id);
    }

    HASH_ADD_STR(broker->clients, _id, client);

    /* pthread_rwlock_unlock(&broker->cli_rw_lock); */

    return client;
RET_ERROR:
    net__free(c_conn);

    return NULL;
}

status__err broker__deliver_msg(bpsp__client* source_client, bpsp__subscriber* sub, bpsp__frame* frame) {
    ASSERT_ARG(source_client && source_client->broker, BPSP_INVALID_ARG);

    bpsp__broker* broker = source_client->broker;
    status__err s = BPSP_OK;

    s = frame__set_var_header(frame, "x-topic", subscriber__get_topic(sub));
    ASSERT_BPSP_OK(s);

    pthread_rwlock_rdlock(&broker->cli_rw_lock);

    bpsp__client* target_client = NULL;

    char* client_id = subscriber__get_client_id(sub);

    HASH_FIND_STR(broker->clients, client_id, target_client);

    if (target_client) {
        if (target_client == sub->client) {
            char* source_ip = inet_ntoa(source_client->conn->addr->sin_addr);
            char* source_addr = mem__malloc(sizeof(char) * (strlen(source_ip) + 10));
            if (!source_addr) {
                pthread_rwlock_unlock(&broker->cli_rw_lock);

                return BPSP_NO_MEMORY;
            }
            sprintf(source_addr, "%s:%d", source_ip, ntohs(source_client->conn->addr->sin_port));
            s = frame__set_var_header(frame, "x-from", source_addr);
            mem__free(source_addr);

            IFN_OK(s) {
                pthread_rwlock_unlock(&broker->cli_rw_lock);
                log__error("Cannot set source header to deliver_frame");

                return s;
            }

            client__send(target_client, frame, 1);
        } else {
            log__error("Client pointer in subscriber does not match with result found from broker's client table.");
        }
    } else {
        log__warn("Deliver msg to Client %s has been destroy.", client_id);
    }

    pthread_rwlock_unlock(&broker->cli_rw_lock);

    return s;
}

status__err broker__destroy_client(bpsp__broker* broker, bpsp__client* client, uint8_t lock) {
    status__err s = BPSP_OK;

    if (lock) {
        pthread_rwlock_wrlock(&broker->cli_rw_lock);
    }

    bpsp__client* del_client = NULL;

    HASH_FIND_STR(broker->clients, client->_id, del_client);

    if (del_client) {
        HASH_DEL(broker->clients, del_client);
    }

    if (lock) {
        pthread_rwlock_unlock(&broker->cli_rw_lock);
    }

    client__destroy(client);

    return s;
}

status__err broker__add_sub(bpsp__broker* broker, bpsp__subscriber* sub, uint8_t lock) {
    status__err s = BPSP_OK;

    s = topic__add_subscriber(broker->topic_tree, sub, lock);

    return s;
}

status__err broker__del_sub(bpsp__broker* broker, bpsp__subscriber* sub, uint8_t lock) {
    status__err s = BPSP_OK;

    s = topic__del_subscriber(broker->topic_tree, sub, lock);

    return s;
}

UT_array* broker__find_subs(bpsp__broker* broker, char* topic, uint8_t lock) {
    UT_array* subs = topic__tree_find_subscribers(broker->topic_tree, topic, lock);

    return subs;
}
