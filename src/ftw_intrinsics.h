#ifndef FTW_INTRINSICS_H
#define FTW_INTRINSICS_H

#include <math.h>

// TODO: Move this macros to another file
#define array_len(array) (sizeof(array) / sizeof((array)[0]))

inline s32 floor_r32_to_s32(r32 value) {
    s32 result = (s32)floorf(value);

    return result;
}

#endif
