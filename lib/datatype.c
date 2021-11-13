#include "datatype.h"

#include <string.h>

static inline int is_bigendian(void) {
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};
    return bint.c[0] == 1;
}

static inline void datatype__e8(bpsp__uint8 val, bpsp__byte* data) { memcpy(data, &val, sizeof(val)); }

static inline bpsp__uint8 datatype__d8(bpsp__byte* data) {
    bpsp__uint8 val;
    memcpy(&val, data, sizeof(val));
    return val;
}

static inline void datatype__e16(bpsp__uint16 val, bpsp__byte* data) {
    if (!is_bigendian()) {
        val = ((val & 0xFF00u) >> 8u) | ((val & 0x00FFu) << 8u);
    }
    memcpy(data, &val, sizeof(val));
}

static inline bpsp__uint16 datatype__d16(bpsp__byte* data) {
    bpsp__uint16 val;
    memcpy(&val, data, sizeof(val));
    if (!is_bigendian()) {
        val = ((val & 0xFF00u) >> 8u) | ((val & 0x00FFu) << 8u);
    }
    return val;
}

static inline void datatype__e32(bpsp__uint32 val, bpsp__byte* data) {
    if (!is_bigendian()) {
        val = ((val & 0xFF000000u) >> 24u) | ((val & 0x00FF0000u) >> 8u) | ((val & 0x0000FF00u) << 8u) |
              ((val & 0x000000FFu) << 24u);
    }
    memcpy(data, &val, sizeof(val));
}

static inline bpsp__uint32 datatype__d32(bpsp__byte* data) {
    bpsp__uint32 val;
    memcpy(&val, data, sizeof(val));
    if (!is_bigendian()) {
        val = ((val & 0xFF000000u) >> 24u) | ((val & 0x00FF0000u) >> 8u) | ((val & 0x0000FF00u) << 8u) |
              ((val & 0x000000FFu) << 24u);
    }
    return val;
}

