#include <assert.h>

#include "client.h"
#include "topic.h"

int main() {
    bpsp__topic_tree tree = {};
    topic__init_tree(&tree);
    bpsp__topic_tree* ptree = topic__new_tree();
    bpsp__subscriber* sub = subscriber__new("locationA/sensorA/*", NULL, NULL);

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

    sub = subscriber__new("locationA/sensorA/uptime", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationA/sensorA/downtime", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationA/sensorA/uptime", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationA/sensorA/+", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/sensorB", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/*/", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/*/alo", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationB/+/uptime", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("locationA/+/uptime", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    sub = subscriber__new("*+/locationA/+/uptime", NULL, NULL);
    s = topic__add_subscriber(&tree, sub);
    subscriber__free(sub);

    topic__print_tree(ptree);

    topic__print_tree(&tree);

    ASSERT_ARG(s == BPSP_OK, 1);

    topic__dtor_tree(&tree);
    topic__free_tree(ptree);

    return 0;
}
