#include "client.h"

#include <pthread.h>
#include <stdio.h>

#include "broker.h"

bpsp__broker* broker;

void* test_sub(void* arg) {
    int i = *((int*)arg);

    bpsp__client* client = client__new(NULL, broker);

    pthread_t tid = pthread_self();

    char* str = strdup("0/locationA/sensorA/temperature");
    *(str) = abs((i % 42)) + 48;
    /* printf("%s\n", str); */

    client__sub(client, str, 1);

    return 0;
}

int main() {
    broker = broker__new("127.0.0.1", 29019);

    const int THREADS = 10;
    pthread_t tid[THREADS];

    for (int i = 0; i < THREADS; i++) {
        pthread_create(&tid[i], NULL, test_sub, &tid[i]);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    topic__print_tree(broker->topic_tree);

    return 0;
}
