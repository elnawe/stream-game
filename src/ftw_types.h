#ifndef FTW_TYPES_H
#define FTW_TYPES_H

#include <stdint.h>
#include <float.h>

/*
 * FTW_DEVELOP:
 *   1 - Slower but double checking for security
 *   0 - Faster, Performance Mode.
 *
 * FTW_INTERNAL:
 *   1 - Enable internal developer-only mode
 *   0 - Release mode
 */

#if FTW_DEVELOP
// TODO: assert requires more logic. This is a pretty light macro.
    #define assert(expression) if(!(expression)) {*(int *)0 = 0;}
#else
    #define assert(expression)
#endif

/*
 * Static has (at least) 3 different meanings. With the following macro I'm renaming
 * `static` to:
 *   - `global_var` when used on a new variable outside of the scope of a function
 *      making that variable global (to the file).
 *   - `local_var` when used inside the scope of a function. Makes the variable global
 *     to that scope.
 *   - `internal` when used on a function, it makes the function private to that file.
 */
#define global_var static
#define local_var  static
#define internal   static

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

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

typedef intptr_t   iptr;
typedef uintptr_t  uptr;

union Vector2 {
    struct {
        f32 x;
        f32 y;
    };

    struct {
        f32 w;
        f32 h;
    };

    f32 e[2];
};

#endif
