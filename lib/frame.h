#ifndef _FRAME_H_
#define _FRAME_H_

#include "bpsp.h"
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

typedef enum {
    OP_NOOP = 0,
    OP_INFO,
    OP_CONNECT,
    OP_PUB,
    OP_SUB,
    OP_UNSUB,
    OP_MSG,
    OP_OK,
    OP_ERR,
} bpsp__opcode;

#define OP_TEXT(o) frame__get_op_text(o)

typedef char** bpsp__var_header_pair;  // must be char[2]

struct bpsp__var_header {
    char* key;
    bpsp__uint16 n_key;
    char* value;
    bpsp__uint16 n_value;

    /** uthash.h **/
    UT_hash_handle hh;
};

struct bpsp__frame {
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
    bpsp__uint32 payload_size;
    bpsp__uint32 pos;  // current processsed position of frame

    /** Misc **/
    /* struct bpsp__frame* next; */
    bpsp__uint8 _is_completed;
};

/** core **/

status__err var_header__to_bytes(bpsp__var_header* vars_headers, bpsp__byte* data);
bpsp__frame* frame__alloc();
bpsp__frame* frame__realloc(bpsp__frame* frame);
void frame__free(bpsp__frame* frame);
bpsp__frame* frame__new();
status__err frame__empty(bpsp__frame* frame);
status__err frame__set_var_header(bpsp__frame* frame, char* key, char* value);
status__err frame__set_var_header2(bpsp__frame* frame, bpsp__var_header_pair pair);
status__err frame__set_var_headers(bpsp__frame* frame, bpsp__var_header_pair* headers, uint16_t n_headers);
status__err frame__parse_var_header(bpsp__frame* frame, bpsp__byte* buf, bpsp__uint16 size);
status__err frame__set_opcode(bpsp__frame* frame, bpsp__uint8 opcode);
status__err frame__set_flag(bpsp__frame* frame, bpsp__uint8 flag);
status__err frame__set_frame_control(bpsp__frame* frame, bpsp__uint8 opcode, uint8_t flag);
status__err frame__malloc_payload(bpsp__frame* frame, bpsp__uint32 data_size);
status__err frame__put_payload(bpsp__frame* frame, bpsp__byte* payload, bpsp__uint32 data_size, bpsp__uint8 append);
status__err frame__replace_payload(bpsp__frame* frame, bpsp__byte* payload, bpsp__uint32 data_size);
status__err frame__is_completed(bpsp__frame* frame);
status__err frame__build(bpsp__frame* frame);
status__err frame__copy(bpsp__frame* dst, bpsp__frame* src, uint8_t build);
bpsp__frame* frame__dup(bpsp__frame* src, uint8_t build);
void frame__print(bpsp__frame* frame);
status__err frame__is_completed(bpsp__frame* frame);

/** read **/
status__err frame__recv(bpsp__connection* conn, bpsp__frame* frame);

/** write **/
status__err frame__send(bpsp__connection* conn, bpsp__frame* frame);

/** frame op **/
const char* frame__get_op_text(bpsp__opcode op);
status__err frame__INFO(bpsp__frame* frame, bpsp__broker* broker);
status__err frame__CONNECT(bpsp__frame* frame, bpsp__byte* info, uint32_t size);
status__err frame__PUB(bpsp__frame* frame, char* topic, bpsp__uint8 flag, bpsp__var_header_pair* headers,
                       uint16_t n_headers, bpsp__byte* msg, uint32_t size);
status__err frame__SUB(bpsp__frame* frame, char* topic, bpsp__uint8 flag, bpsp__var_header_pair* headers_rule,
                       uint16_t n_headers);
status__err frame__UNSUB(bpsp__frame* frame, char* topic, bpsp__uint8 flag);
status__err frame__MSG(bpsp__frame* frame, char* topic, bpsp__uint8 flag, bpsp__var_header_pair* headers,
                       uint16_t n_headers, bpsp__byte* msg, uint32_t size);
status__err frame__OK(bpsp__frame* frame, bpsp__uint8 flag, char* msg);
status__err frame__ERR(bpsp__frame* frame, bpsp__uint8 flag, status__err s_err, char* msg);

/** test **/
status__err frame__parse_var_header(bpsp__frame* frame, bpsp__byte* buf, bpsp__uint16 size);

#endif  // _FRAME_H_
