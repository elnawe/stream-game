#ifndef FTW_INTRINSICS_H
#define FTW_INTRINSICS_H

#include <math.h>

inline i32 floor_r32_to_i32(r32 value) {
    i32 result = (i32)floorf(value);

    return result;
}

#endif
