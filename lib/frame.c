#include <string.h>

#include "client.h"
#include "datatype.h"
#include "log.h"
#include "mem.h"
#include "status.h"

bpsp__var_header* var_header__new(const char* key, const char* value) {
    ASSERT_ARG(key, NULL);
    ASSERT_ARG(strlen(key), NULL);
    /* ASSERT_ARG(value, NULL); */

    bpsp__var_header* var_header = (bpsp__var_header*)mem__malloc(sizeof(bpsp__var_header));

    ASSERT_ARG(var_header, NULL);

    var_header->key = mem__strdup(key);

    if (value && strlen(value)) {
        var_header->value = mem__strdup(value);
    }

    return var_header;
}

status__err var_header__set(bpsp__var_header* var_header, const char* value) {
    ASSERT_ARG(var_header, BPSP_INVALID_ARG);
    ASSERT_ARG(var_header->key, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    if (var_header->value) {
        mem__free(var_header->value);
    }

    var_header->value = mem__strdup(value);

    return s;
}

void var_header__free(bpsp__var_header* var_header) {
    ASSERT_ARG(var_header, __EMPTY__);

    if (var_header->key) {
        mem__free(var_header->key);
    }

    if (var_header->value) {
        mem__free(var_header->value);
    }

    mem__free(var_header);
}

status__err var_header__to_bytes(bpsp__var_header* var_headers, bpsp__byte* data) {
    if (!HASH_COUNT(var_headers)) {
        return BPSP_OK;
    }

    status__err s = BPSP_OK;

    if (var_headers) {
        bpsp__uint16 n = 0;
        bpsp__uint16 key_len = 0;
        bpsp__uint16 value_len = 0;

        bpsp__var_header *var_header, *tmp;
        HASH_ITER(hh, var_headers, var_header, tmp) {
            // open double quote key
            *(data + n) = (bpsp__uint8)'"';
            n += 1;
            key_len = strlen(var_header->key);
            // copy key
            mem__memcpy(data + n, var_header->key, key_len);
            n += key_len;
            // close double quote for key
            *(data + n) = (bpsp__uint8)'"';
            n += 1;

            // open double quote key
            *(data + n) = (bpsp__uint8)'"';
            n += 1;
            value_len = strlen(var_header->value);
            // copy key
            mem__memcpy(data + n, var_header->value, value_len);
            n += value_len;
            // close double quote for key
            *(data + n) = (bpsp__uint8)'"';
            n += 1;

            // end with `;`
            *(data + n) = (bpsp__uint8)';';
            n += 1;
        }
    }

    return s;
}

bpsp__frame* frame__alloc() {
    bpsp__frame* frame = (bpsp__frame*)mem__malloc(sizeof(bpsp__frame));

    if (!frame) {
        log__error("Cannot frame__alloc()");

        return NULL;
    }

    memset(frame, 0, sizeof(*frame));

    return frame;
}

void frame__free_payload(bpsp__frame* frame) {
    ASSERT_ARG(frame, __EMPTY__);

    if (frame->payload) {
        mem__free(frame->payload);
    }

    frame->data_size = 0;
    frame->pos = 0;
}

void frame__free_var_headers(bpsp__frame* frame) {
    ASSERT_ARG(frame, __EMPTY__);

    if (frame->var_headers) {
        bpsp__var_header *var_header, *tmp;
        HASH_ITER(hh, frame->var_headers, var_header, tmp) {
            HASH_DEL(frame->var_headers, var_header); /* delete it (users advances to next) */
            var_header__free(var_header);             /* free it */
        }

        HASH_CLEAR(hh, frame->var_headers);
    }
}

void frame__free(bpsp__frame* frame) {
    ASSERT_ARG(frame, __EMPTY__);

    /* if(frame->next) */
    frame__free_payload(frame);
    frame->payload = NULL;

    frame__free_var_headers(frame);

    mem__free(frame);
}

bpsp__frame* frame__new() {
    bpsp__frame* frame = frame__alloc();

    ASSERT_ARG(frame, NULL);

    return frame;
}

status__err frame__empty(bpsp__frame* frame) {
    status__err s = BPSP_OK;

    ASSERT_ARG(frame, BPSP_INVALID_ARG);

    frame__free_payload(frame);

    frame__free_var_headers(frame);

    memset(frame, 0, sizeof(*frame));

    return s;
}

status__err frame__set_var_header(bpsp__frame* frame, const char* key, const char* value) {
    status__err s = BPSP_OK;

    ASSERT_ARG(frame, BPSP_INVALID_ARG);

    bpsp__uint16 key_len = strlen(key);
    bpsp__uint16 value_len = strlen(value);

    bpsp__var_header* var_header = NULL;
    HASH_FIND_STR(frame->var_headers, key, var_header);

    if (var_header) {
        frame->vars_size -= strlen(var_header->value);

        s = var_header__set(var_header, value);

        ASSERT_BPSP_OK(s);

        // 5 for extra two pair of `""` and `;` character
        ASSERT_ARG((frame->vars_size + value_len) < 65536, BPSP_MAX_VAR_HEADERS);

        frame->vars_size += value_len;

    } else {
        var_header = var_header__new(key, value);

        ASSERT_ARG(var_header, BPSP_NO_MEMORY);

        // 5 for extra two pair of `""` and `;` character
        ASSERT_ARG((frame->vars_size + key_len + value_len + 5) < 65536, BPSP_MAX_VAR_HEADERS);

        // 5 for extra two pair of `""` and `;` character
        frame->vars_size += key_len + value_len + 5;

        HASH_ADD_STR(frame->var_headers, key, var_header);
    }

    return s;
}

status__err frame__parse_var_header(bpsp__frame* frame, bpsp__byte* buf, bpsp__uint16 size) {
    status__err s = BPSP_OK;

    /* log__debug("size: %u - buf: %s", size, buf); */

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

status__err frame__validate_opcode(bpsp__uint8 opcode) {
    ASSERT_ARG(opcode > 0, BPSP_INVALID_OPCODE);

    return BPSP_OK;
}

status__err frame__set_opcode(bpsp__frame* frame, bpsp__uint8 opcode) {
    ASSERT_ARG(frame, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    s = frame__validate_opcode(opcode);

    ASSERT_BPSP_OK(s);

    frame->opcode = opcode;

    return s;
}

status__err frame__set_flag(bpsp__frame* frame, bpsp__uint8 flag) {
    ASSERT_ARG(frame, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    frame->flag = flag;

    return s;
}

status__err frame__malloc_payload(bpsp__frame* frame, bpsp__uint32 data_size) {
    ASSERT_ARG(frame, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    frame__free_payload(frame);

    if (data_size == 0) {
        return s;
    }

    frame->payload = mem__malloc(data_size);

    ASSERT_ARG(frame->payload, BPSP_NO_MEMORY);

    frame->data_size = data_size;

    return s;
}

status__err frame__validate_var_headers(bpsp__frame* frame) {
    ASSERT_ARG(frame, BPSP_INVALID_ARG);

    // should we?
    ASSERT_ARG((uint32_t)frame->vars_size < 65536, BPSP_INVALID_OPCODE);

    return BPSP_OK;
}

status__err frame__is_completed(bpsp__frame* frame) {
    ASSERT_ARG(frame, BPSP_INVALID_ARG);

    status__err s = BPSP_OK;

    /* s = frame__validate_opcode(frame); */
    /* ASSERT_BPSP_OK(s); */

    /* s = frame__validate_var_headers(frame); */
    /* ASSERT_BPSP_OK(s); */

    ASSERT_ARG(frame->_is_completed, BPSP_NOT_COMPLETED_FRAME);

    return BPSP_OK;
}

status__err frame__build(bpsp__frame* frame) {
    status__err s = BPSP_OK;

    s = frame__validate_opcode(frame->opcode);
    /* ASSERT_BPSP_OK(s); */
    IFN_OK(s) { goto RET_ERROR; }

    /* s = frame__validate_var_headers(frame); */
    /* ASSERT_BPSP_OK(s); */
    /* IFN_OK(s) { goto RET_ERROR; } */

    /* frame->n_processed = 0; */
    frame->_is_completed = 1;

    return s;

RET_ERROR:
    log__error("frame__build() error: %s", ERR_TEXT(s));
    return s;
}

void frame__print(bpsp__frame* frame) {
    printf("[\tFRAME\t\t]\n");

    if (frame) {
        printf("--Fixed Header--\n");
        printf("|var size| |opcode| |flag|\n");
        printf("|%u| |%u| |%u|\n", frame->vars_size, frame->opcode, frame->flag);
        printf("|data size|\n");
        printf("|%u|\n", frame->data_size);

        printf("--Var Headers--\n");
        bpsp__var_header *var_header, *tmp;
        HASH_ITER(hh, frame->var_headers, var_header, tmp) {
            printf("\"%s\"\"%s\";\n", var_header->key, var_header->value);
        }

        printf("--Data Payload--\n");
        if (frame->data_size > 0) {
            char* payload = mem__malloc(sizeof(bpsp__byte) * (frame->data_size + 1));
            mem__memcpy(payload, frame->payload, frame->data_size);
            printf("%s\n", payload);
            mem__free(payload);
        } else {
            printf("NULL\n");
        }

    } else {
        printf("NULL\n");
    }

    printf("[-------------------]\n");
}
