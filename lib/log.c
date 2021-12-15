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

int log__timestamps = 0;
int log__stack_trace = 0;
int log__enable = 1;
int log__frame = 0;

static inline void print_label(const char* label) {
    if (log__timestamps) {
        char date[20];
        struct timeval tv;

        // print timestamp eg: " 2021-10-13T15:05:04.841Z    INFO] msg "
        gettimeofday(&tv, NULL);
        strftime(date, sizeof(date) / sizeof(*date), "%Y-%m-%dT%H:%M:%S", gmtime(&tv.tv_sec));
        printf("%s.%03ldZ", date, tv.tv_usec / 1000);
    }

    // print label like normal
    printf("%6s] ", label);
}

void _log__print(const char* label, const char* fmt, ...) {
    if (log__enable) {
        if (label) {
            print_label(label);
        } else {
            print_label("LOG");
        }

        _VPRINT
    }
}

void _log__debug(const char* fmt, ...) {
    print_label("DEBUG");

    _VPRINT
}

void _log__info(const char* fmt, ...) {
    print_label("INFO");

    _VPRINT
}

void _log__warn(const char* fmt, ...) {
    print_label("WARN");

    _VPRINT
}

void _log__error(const char* fmt, ...) {
    print_label("ERROR");

    _VPRINT
}
