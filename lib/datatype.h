#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

#include "stdint.h"

typedef uint8_t bpsp__byte;
typedef uint8_t bpsp__uint8;
typedef uint16_t bpsp__uint16;
typedef uint32_t bpsp__uint32;
typedef uint64_t bpsp__uintvar;
typedef struct {
    char* key;
    char* value;
} bpsp__var_header;

static inline int is_bigendian(void);

static inline void datatype__e8(bpsp__uint8 val, bpsp__byte* data);
static inline bpsp__uint8 datatype__d8(bpsp__byte* data);

static inline void datatype__e16(bpsp__uint16 val, bpsp__byte* data);
static inline bpsp__uint16 datatype__d16(bpsp__byte* data);

static inline void datatype__e32(bpsp__uint32 val, bpsp__byte* data);
static inline bpsp__uint32 datatype__d32(bpsp__byte* data);

static inline void datatype__e32(bpsp__uint32 val, bpsp__byte* data);
static inline bpsp__uint32 datatype__d32(bpsp__byte* data);

// encode and decode for variable integer must be implement in frame
/* static inline void datatype__evar(bpsp__uintvar val, bpsp__byte* data); */
/* static inline bpsp__uintvar datatype__dvar(bpsp__byte* data); */

#endif  // _DATA_TYPE_H_
