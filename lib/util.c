#include "util.h"

#include "time.h"

void rand_str(char* str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    if (str && size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int)(sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}

void date_now_utc(char* str, size_t len) {
    /* char date[20]; */
    time_t rawtime;

    time(&rawtime);

    // FIXME: add exact time in miliseconds instead of .000Z
    strftime(str, len, "%Y-%m-%dT%H:%M:%S.000Z", gmtime(&rawtime));
    /* printf("%s.%03ldZ", date, tv.tv_usec / 1000); */
}
