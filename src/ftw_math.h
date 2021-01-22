#ifndef FTW_MATH_H
#define FTW_MATH_H

#include "ftw_types.h"

// TODO: Remove these dependencies once we write our own implementation
#include "SDL.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

u8 random(u8 max_value) {
    u8 result = rand()%(max_value + 1);

    return result;
}

u8 random_range(u8 min_value, u8 max_value) {
    u8 result = min_value + rand()%((max_value + 1) - min_value);

    return result;
}

Vector2 operator+(Vector2 a, Vector2 b) {
    Vector2 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

Vector2 &operator+=(Vector2 &a, Vector2 b) {
    a = a + b;

    return a;
}

Vector2 operator-(Vector2 a, Vector2 b) {
    Vector2 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

Vector2 &operator-=(Vector2 &a, Vector2 b) {
    a = a - b;

    return a;
}

Vector2 operator*(Vector2 a, f32 b) {
    Vector2 result;

    result.x = a.x * b;
    result.y = a.y * b;

    return result;
}

Vector2 operator*(f32 a, Vector2 b) {
    Vector2 result;

    result.x = b.x * a;
    result.y = b.y * a;

    return result;
}

Vector2 &operator*=(Vector2 &a, f32 b) {
    a = a * b;

    return a;
}

Vector2 operator/(Vector2 a, f32 b) {
    Vector2 result;

    result.x = a.x / b;
    result.y = a.y / b;

    return result;
}

Vector2 operator/(f32 a, Vector2 b) {
    Vector2 result;

    result.x = a / b.x;
    result.y = a / b.y;

    return result;
}

Vector2 &operator/=(Vector2 &a, f32 b) {
    a = a / b;

    return a;
}

#endif
