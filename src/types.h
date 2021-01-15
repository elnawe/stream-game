#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <float.h>

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t    i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float  f32;
typedef double f64;

// Real (only positive, floating point values)
typedef float  r32;
typedef double r64;

typedef intptr_t   intptr;
typedef uintptr_t uintptr;

union Vector2 {
    struct {
        f32 x;
        f32 y;
    };

    struct {
        f32 w;
        f32 h;
    };

    f32 E[2];
};



#endif
