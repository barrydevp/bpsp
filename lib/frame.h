#ifndef _FRAME_H_
#define _FRAME_H_

#include "datatype.h"

typedef struct bpsp__frame {
    bpsp__byte* payload;
    bpsp__frame* next;

} bpsp__frame;

#endif  // _FRAME_H_
