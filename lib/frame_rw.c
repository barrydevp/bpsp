#include "datatype.h"
#include "frame.h"
#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"

status__err frame__recv(bpsp__connection* conn, bpsp__frame* frame) {
    // should we?
    /* ASSERT_ARG(frame, BPSP_INVALID_ARG); */

    status__err s = BPSP_OK;

    s = frame__empty(frame);
    ASSERT_BPSP_OK(s);

    /** read fixed header **/
    bpsp__byte fixed_header[FIXED_HEADER_SIZE];
    ssize_t n_read = 0;

    s = net__read(conn, fixed_header, FIXED_HEADER_SIZE, &n_read, 1);
    n_read = 0;
    ASSERT_BPSP_OK(s);

    bpsp__uint16 vars_size = datatype__d16(fixed_header);

    bpsp__uint8 opcode = datatype__d8(fixed_header + OFFSET_OPCODE);
    bpsp__uint8 flag = datatype__d8(fixed_header + OFFSET_FLAG);
    s = frame__set_frame_control(frame, opcode, flag);
    ASSERT_BPSP_OK(s);

    bpsp__uint32 data_size = datatype__d32(fixed_header + OFFSET_DATA_SIZE);

    /** read variables header **/
    bpsp__byte* var_hdr_buf = (bpsp__byte*)mem__malloc(sizeof(bpsp__byte) * vars_size);
    ASSERT_ARG(var_hdr_buf, BPSP_NO_MEMORY);

    s = net__read(conn, var_hdr_buf, vars_size, &n_read, 1);
    n_read = 0;
    IFN_OK(s) {
        //
        mem__free(var_hdr_buf);

        return s;
    }

    s = frame__parse_var_header(frame, var_hdr_buf, vars_size);
    mem__free(var_hdr_buf);
    IFN_OK(s) {
        //
        return s;
    }

    /** duplicated variable header? look weird, we do not allow this duplicate **/
    if (frame->vars_size != vars_size) {
        log__warn("Duplicate variable header when read() ?");
    }

    /** read payload/data **/
    s = frame__malloc_payload(frame, data_size);
    IFN_OK(s) {
        //
        return s;
    }

    if (data_size > 0) {
        s = net__read(conn, frame->payload, data_size, &n_read, 1);
        frame->pos = (bpsp__uint32)n_read;
        n_read = 0;
        IFN_OK(s) {
            //
            return s;
        }
    }

    /** finally build and validate **/
    s = frame__build(frame);

    IFN_OK(s) {
        //
        return s;
    }

    return s;
}

status__err frame__send(bpsp__connection* conn, bpsp__frame* frame) {
    // should we?
    /* ASSERT_ARG(frame, BPSP_INVALID_ARG); */

    status__err s = frame__is_completed(frame);

    ASSERT_BPSP_OK(s);

    /** write fixed header **/
    bpsp__byte fixed_header[FIXED_HEADER_SIZE];
    ssize_t n_write = 0;

    datatype__e16(frame->vars_size, fixed_header);
    datatype__e8(frame->opcode, fixed_header + OFFSET_OPCODE);
    datatype__e8(frame->flag, fixed_header + OFFSET_FLAG);
    datatype__e32(frame->data_size, fixed_header + OFFSET_DATA_SIZE);

    s = net__write(conn, fixed_header, FIXED_HEADER_SIZE, &n_write, 1);
    n_write = 0;
    ASSERT_BPSP_OK(s);

    /** write variables header **/
    if (frame->vars_size > 0) {
        bpsp__byte* var_hdr_buf = (bpsp__byte*)mem__malloc(sizeof(bpsp__byte) * frame->vars_size);
        ASSERT_ARG(var_hdr_buf, BPSP_NO_MEMORY);

        s = var_header__to_bytes(frame->var_headers, var_hdr_buf);
        IFN_OK(s) {
            mem__free(var_hdr_buf);

            return s;
        }

        s = net__write(conn, var_hdr_buf, frame->vars_size, &n_write, 1);
        n_write = 0;
        mem__free(var_hdr_buf);
        IFN_OK(s) {
            //

            return s;
        }
    }

    /** write payload/data **/
    if (frame->data_size > 0) {
        s = net__write(conn, frame->payload, frame->data_size, &n_write, 1);
        frame->pos = (bpsp__uint32)n_write;
        n_write = 0;
        IFN_OK(s) {
            //
            return s;
        }
    }

    IFN_OK(s) {
        //
        return s;
    }

    return s;
}
