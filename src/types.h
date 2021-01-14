#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <float.h>

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int8  i8;
typedef int16 i16;
typedef int32 i32;
typedef int64 i64;

typedef float  f32;
typedef double f64;

// Real (only positive, floating point values)
typedef float  r32;
typedef double r64;

typedef intptr_t  intptr;
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
