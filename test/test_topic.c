#include <assert.h>

#include "client.h"
#include "topic.h"

int main() {
    bpsp__client* client = client__new(NULL);
    bpsp__topic_tree tree = {};
    topic__init_tree(&tree);
    bpsp__topic_tree* ptree = topic__new_tree();
    bpsp__subscriber* sub = subscriber__new("locationA/sensorA/*", client, NULL);

    status__err s;
    /* uint16_t n_tok = 0; */
    /* char* first_tok = NULL; */
    /* s = topic__extract_token("./bar", &n_tok, &first_tok); */
    /* if (first_tok) { */
    /*     free(first_tok); */
    /* } */
    /* s = topic__extract_token("foo/.*", &n_tok, &first_tok); */
    /* free(first_tok); */
    /* s = topic__extract_token("foo/+/bar", &n_tok, &first_tok); */
    /* free(first_tok); */

    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    bpsp__subscriber* sub1 = subscriber__new("locationA/sensorA/uptime", client, NULL);
    s = topic__add_subscriber(&tree, sub1);
    bpsp__subscriber* sub2 = subscriber__new("locationA/sensorA/uptime", client, NULL);
    s = topic__add_subscriber(&tree, sub2);
    /* subscriber__free(sub1); */

    sub = subscriber__new("locationA/sensorA/downtime", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationA/sensorA/uptime", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    /* s = topic__del_subscriber(sub); */
    /* s = topic__del_subscriber(sub1); */
    subscriber__free(sub);
    subscriber__free(sub1);
    subscriber__free(sub2);

    sub = subscriber__new("locationA/sensorA/+", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/sensorB", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/*/", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/*/alo", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/+/uptime", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationA/+/uptime", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("*+/locationA/+/uptime", client, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    topic__print_tree(ptree);

    topic__print_tree(&tree);

    ASSERT_ARG(s == BPSP_OK, 1);

    topic__dtor_tree(&tree);
    topic__free_tree(ptree);

    client__free(client);

    return 0;
}
