#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define _VPRINT          \
    va_list args;        \
    va_start(args, fmt); \
    vprintf(fmt, args);  \
    va_end(args);        \
    printf("\n");


static inline void print_label(const char* label) {
    char date[20];
    struct timeval tv;

    // print lable and timestamp eg: " 2021-10-13T15:05:04.841Z    INFO] msg "
    gettimeofday(&tv, NULL);
    strftime(date, sizeof(date) / sizeof(*date), "%Y-%m-%dT%H:%M:%S", gmtime(&tv.tv_sec));
    printf("%s.%03ldZ%6s] ", date, tv.tv_usec / 1000, label);
}

void log__debug(const char* fmt, ...) {
    print_label("DEBUG");

    _VPRINT
}

void log__info(const char* fmt, ...) {
    print_label("INFO");

    _VPRINT
}

void log__warn(const char* fmt, ...) {
    print_label("WARN");

    _VPRINT
}

void log__error(const char* fmt, ...) {
    print_label("ERROR");

    _VPRINT
}
