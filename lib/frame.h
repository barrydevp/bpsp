#ifndef _FRAME_H_
#define _FRAME_H_

#include "datatype.h"
#include "net.h"
#include "status.h"
#include "uthash.h"

typedef struct {
    char* key;
    char* value;

    /** uthash.h **/
    UT_hash_handle hh;
} bpsp__var_header;

typedef struct bpsp__frame {
    /** Fixed Header **/
    bpsp__uint16 vars_size;
    bpsp__uint8 opcode;
    bpsp__uint8 flag;
    bpsp__uint32 data_size;

    bpsp__uint32 n_processed;  // number of byte has been processsed often in write mode

    /** Variable Header **/
    bpsp__var_header* var_headers;

    /** Data **/
    bpsp__byte* payload;
    bpsp__uint32 payload_length;  // current payload length
    bpsp__uint32 pos;

    /** Misc **/
    /* struct bpsp__frame* next; */

} bpsp__frame;

bpsp__var_header* var_header__new(char* key, char* value);

bpsp__frame* frame__alloc();
bpsp__frame* frame__realloc(bpsp__frame* frame);
void frame__free(bpsp__frame* frame);
status__err frame__init();
status__err frame__build(bpsp__frame* frame);
status__err frame_empty(bpsp__frame* frame);
status__err frame_ctr(bpsp__frame* frame);

#endif  // _FRAME_H_
