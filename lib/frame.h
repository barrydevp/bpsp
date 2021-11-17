#ifndef _FRAME_H_
#define _FRAME_H_

#include "datatype.h"
#include "net.h"
#include "status.h"
#include "uthash.h"

#define FIXED_HEADER_SIZE 8  // bytes
// offset from byte 0
#define OFFSET_VARS_SIZE 0
#define OFFSET_OPCODE 2
#define OFFSET_FLAG 3
#define OFFSET_DATA_SIZE 4
#define OFFSET_VAR_HEADERS 8

typedef struct {
    char* key;
    bpsp__uint16 n_key;
    char* value;
    bpsp__uint16 n_value;

    /** uthash.h **/
    UT_hash_handle hh;
} bpsp__var_header;

typedef struct bpsp__frame {
    /** Fixed Header **/
    bpsp__uint16 vars_size;
    bpsp__uint8 opcode;
    bpsp__uint8 flag;
    bpsp__uint32 data_size;

    /* bpsp__uint32 n_processed;  // number of byte has been processsed, usually in write mode */

    /** Variable Header **/
    bpsp__var_header* var_headers;

    /** Frame body **/
    bpsp__byte* payload;  //
    bpsp__uint32 pos;     // current processsed position of frame

    /** Misc **/
    /* struct bpsp__frame* next; */
    bpsp__uint8 _is_completed;

} bpsp__frame;

/** core **/

status__err var_header__to_bytes(bpsp__var_header* vars_headers, bpsp__byte* data);
bpsp__frame* frame__alloc();
bpsp__frame* frame__realloc(bpsp__frame* frame);
void frame__free(bpsp__frame* frame);
bpsp__frame* frame__new();
status__err frame__empty(bpsp__frame* frame);
status__err frame__set_var_header(bpsp__frame* frame, const char* key, const char* value);
status__err frame__parse_var_header(bpsp__frame* frame, bpsp__byte* buf, bpsp__uint16 size);
status__err frame__set_opcode(bpsp__frame* frame, bpsp__uint8 opcode);
status__err frame__set_flag(bpsp__frame* frame, bpsp__uint8 flag);
status__err frame__malloc_payload(bpsp__frame* frame, bpsp__uint32 data_size);
status__err frame__is_completed(bpsp__frame* frame);
status__err frame__build(bpsp__frame* frame);

/** read **/
status__err frame__read(bpsp__connection* conn, bpsp__frame* frame);

/** write **/
status__err frame__write(bpsp__connection* conn, bpsp__frame* frame);

/** test **/
status__err frame__parse_var_header(bpsp__frame* frame, bpsp__byte* buf, bpsp__uint16 size);

#endif  // _FRAME_H_
