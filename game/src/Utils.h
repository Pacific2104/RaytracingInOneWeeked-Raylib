#pragma once
#include "raymath.h"
#include <iostream>

static Vector4 Vector4Clamp(const Vector4& vector, Vector4 min, Vector4 max) {
    Vector4 result = { 0.0f };

    result.x = fminf(max.x, fmaxf(min.x, vector.x));
    result.y = fminf(max.y, fmaxf(min.y, vector.y));
    result.z = fminf(max.z, fmaxf(min.z, vector.z));
    result.w = fminf(max.w, fmaxf(min.w, vector.w));

    return result;
}

static float RandomFloat() {
    // Returns a random real in (0,1).
    return std::rand() / (RAND_MAX + 1.0);
}
static float RandomFloat(float min, float max) {
    // Returns a random real in (min,max).
    return min + (max - min) * RandomFloat();
}
static Vector3 RandomVector3() {
    return Vector3{ RandomFloat(), RandomFloat(), RandomFloat() };
}
static Vector3 RandomVector3(float min, float max) {
    return Vector3{ RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max) };
}
static Vector3 InUnitSphere() {
    return Vector3Normalize(RandomVector3(-1, 1));
}
static Vector3 RandomInUnitDisk() {
    while (true) {
        Vector3 p = Vector3{ RandomFloat(-1, 1), RandomFloat(-1, 1), 0 };
        if (Vector3LengthSqr(p) < 1)
            return p;
    }
}
static bool NearZero(Vector3 e) {
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    return (std::fabs(e.x)< s) && (std::fabs(e.y) < s) && (std::fabs(e.z) < s);
}
static float LinearToGamma(float linearComponent)
{
    if (linearComponent > 0)
        return std::sqrt(linearComponent);
    return 0;
}