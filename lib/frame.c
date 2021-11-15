#include <string.h>

#include "client.h"
#include "datatype.h"
#include "log.h"
#include "mem.h"
#include "status.h"

bpsp__frame* frame__alloc() {
    bpsp__frame* frame = (bpsp__frame*)mem__malloc(sizeof(bpsp__frame));

    if (!frame) {
        log__error("Cannot frame__alloc()");

        return NULL;
    }

    memset(frame, 0, sizeof(*frame));

    return frame;
}

void frame__free(bpsp__frame* frame) {
    if(!frame) {
        return;
    }

    /* if(frame->next) */
    if(frame->payload) {
        mem__free(frame->payload);
    }

    if(frame->var_headers) {

    }

    mem__free(frame);

}

status__err frame__init() {
    status__err s = BPSP_OK;

    bpsp__frame* frame = frame__alloc();

    if (!frame) {

        return BPSP_NO_MEMORY;
    }

    return s;
}

status__err frame__build(bpsp__frame* frame);
status__err frame_empty(bpsp__frame* frame);
status__err frame_ctr(bpsp__frame* frame);
