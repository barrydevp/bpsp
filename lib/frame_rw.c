#include "datatype.h"
#include "frame.h"
#include "log.h"
#include "mem.h"
#include "net.h"
#include "status.h"

status__err frame__parse_var_header(bpsp__frame* frame, bpsp__byte* buf, bpsp__uint16 size) {
    status__err s = BPSP_OK;

    log__debug("size: %u - buf: %s", size, buf);

    if (size == 0) {
        return s;
    }

    bpsp__uint16 n = 0;
    bpsp__uint16 start_key = 0;
    bpsp__uint16 end_key = 0;
    bpsp__uint16 start_value = 0;

    for (; n < size; n++) {
        if (n != 0 && ((char)*(buf + n) != '"' || (char)*(buf + n - 1) == '\\')) {
            continue;
        }
        // if start of key is not set, so this open double quote is the start of key
        if (!start_key) {
            start_key = n + 1;
        } else if (!end_key) {
            end_key = n;
        } else if (!start_value) {
            start_value = n + 1;
        } else {
            // we reach close double quote of end of value

            // set NULL char for end of key
            *(buf + end_key) = (uint8_t)'\0';
            // set NULL char for end of value
            *(buf + n) = (uint8_t)'\0';

            frame__set_var_header(frame, (char*)buf + start_key, (char*)buf + start_value);

            // skip ';', should we remove ';' in spec?
            n += 1;

            // reset
            start_key = 0;
            end_key = 0;
            start_value = 0;
        }
    }

    // partial parsed (not completed parse header)
    if (start_key) {
        return BPSP_INVALID_VAR_HEADERS;
    }

    return s;
}

status__err frame__read(bpsp__connection* conn, bpsp__frame* frame) {
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
    frame__set_opcode(frame, opcode);
    bpsp__uint8 flag = datatype__d8(fixed_header + OFFSET_FLAG);
    frame__set_flag(frame, flag);
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

status__err frame__write(bpsp__connection* conn, bpsp__frame* frame) {
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

    /** read variables header **/
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
