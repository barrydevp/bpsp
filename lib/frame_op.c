#include "bpsp.h"
#include "broker.h"
#include "datatype.h"
#include "frame.h"
#include "log.h"
#include "mem.h"
#include "status.h"

static const char* op_text[] = {
    "NO OP",    // NOOP
    "INFO",     // INFO
    "CONNECT",  // CONNECT
    "PUB",      // PUB
    "SUB",      // SUB
    "UNSUB",    // UNSUB
    "MSG",      // MSG
    "OK",       // OK
    "ERR"       // ERR
};

const char* frame__get_op_text(bpsp__opcode op) { return op_text[(int)op]; }

status__err frame__INFO(bpsp__frame* frame, bpsp__broker* broker) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_INFO, 0);
    ASSERT_BPSP_OK(s);

    broker__info* info = broker->info;

    uint32_t version_len = strlen(info->version);
    uint32_t name_len = strlen(info->name);
    uint32_t info_len = version_len + name_len + 1;
    bpsp__byte* info_bytes = mem__malloc(sizeof(bpsp__byte) * info_len);

    ASSERT_ARG(info_bytes, BPSP_NO_MEMORY);

    mem__memmove(info_bytes, info->version, version_len);
    mem__memmove(info_bytes + version_len, info->name, name_len);
    *(info_bytes + info_len - 1) = '\0';

    s = frame__put_payload(frame, info_bytes, info_len, 0);
    IFN_OK(s) {
        //
        goto RET_ERROR;
    }

    s = frame__build(frame);
    mem__free(info_bytes);

    return s;
RET_ERROR:
    mem__free(info_bytes);

    return s;
}

status__err frame__CONNECT(bpsp__frame* frame, bpsp__byte* info, uint32_t size) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_CONNECT, 0);
    ASSERT_BPSP_OK(s);

    s = frame__put_payload(frame, info, size, 0);
    ASSERT_BPSP_OK(s);

    s = frame__build(frame);

    return s;
}

status__err frame__PUB(bpsp__frame* frame, char* topic, bpsp__uint8 flag, bpsp__var_header_pair* headers,
                       uint16_t n_headers, bpsp__byte* msg, uint32_t size) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_PUB, 0);
    ASSERT_BPSP_OK(s);

    s = frame__set_var_headers(frame, headers, n_headers);
    ASSERT_BPSP_OK(s);

    s = frame__set_var_header(frame, "x-topic", topic);
    ASSERT_BPSP_OK(s);

    s = frame__put_payload(frame, msg, size, 0);
    ASSERT_BPSP_OK(s);

    s = frame__build(frame);

    return s;
}

status__err frame__SUB(bpsp__frame* frame, char* topic, bpsp__uint8 flag, bpsp__var_header_pair* headers_rule,
                       uint16_t n_headers) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_SUB, 0);
    ASSERT_BPSP_OK(s);

    s = frame__set_var_headers(frame, headers_rule, n_headers);
    ASSERT_BPSP_OK(s);

    s = frame__set_var_header(frame, "x-topic", topic);
    ASSERT_BPSP_OK(s);

    s = frame__build(frame);

    return s;
}

status__err frame__UNSUB(bpsp__frame* frame, char* topic, bpsp__uint8 flag) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_UNSUB, 0);
    ASSERT_BPSP_OK(s);

    s = frame__set_var_header(frame, "x-topic", topic);
    ASSERT_BPSP_OK(s);

    s = frame__build(frame);

    return s;
}

status__err frame__MSG(bpsp__frame* frame, char* topic, bpsp__uint8 flag, bpsp__var_header_pair* headers,
                       uint16_t n_headers, bpsp__byte* msg, uint32_t size) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_MSG, 0);
    ASSERT_BPSP_OK(s);

    s = frame__set_var_header(frame, "x-topic", topic);
    ASSERT_BPSP_OK(s);

    s = frame__put_payload(frame, msg, size, 0);
    ASSERT_BPSP_OK(s);

    s = frame__build(frame);

    return s;
}

status__err frame__OK(bpsp__frame* frame, bpsp__uint8 flag, bpsp__byte* msg, uint32_t size) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_OK, 0);
    ASSERT_BPSP_OK(s);

    s = frame__put_payload(frame, msg, size, 0);
    ASSERT_BPSP_OK(s);

    s = frame__build(frame);

    return s;
}

status__err frame__ERR(bpsp__frame* frame, bpsp__uint8 flag, status__err s_err, char* msg) {
    status__err s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    s = frame__set_frame_control(frame, OP_ERR, 0);
    ASSERT_BPSP_OK(s);

    s = frame__put_payload(frame, (bpsp__byte*)ERR_TEXT(s_err), strlen(ERR_TEXT(s_err)), 0);
    ASSERT_BPSP_OK(s);

    if (msg && strlen(msg)) {
        s = frame__put_payload(frame, (bpsp__byte*)msg, strlen(msg), 1);
        ASSERT_BPSP_OK(s);
    }

    s = frame__build(frame);

    return s;
}
