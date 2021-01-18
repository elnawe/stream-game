#ifndef FTW_MATH_H
#define FTW_MATH_H

#include "ftw_types.h"

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
