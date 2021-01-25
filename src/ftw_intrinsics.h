#ifndef FTW_INTRINSICS_H
#define FTW_INTRINSICS_H

#include <time.h>
#include <math.h>
#include "ftw_math.h"

// TODO: Move this macros to another file
#define array_len(array) (sizeof(array) / sizeof((array)[0]))

inline s32 floor_r32_to_s32(r32 value) {
    s32 result = (s32)floorf(value);

    return result;
}

inline u32 random(u32 max_value) {
    u32 result = rand()%(max_value + 1);

    return result;
}

inline u32 random_range(u32 min_value, u32 max_value) {
    u32 result = min_value + rand()%((max_value + 1) - min_value);

    return result;
}

#endif
