#pragma once
#include "raymath.h"
#include <iostream>

inline Vector4 Vector4Clamp(const Vector4& vector, Vector4 min, Vector4 max) {
    Vector4 result = { 0.0f };

    result.x = fminf(max.x, fmaxf(min.x, vector.x));
    result.y = fminf(max.y, fmaxf(min.y, vector.y));
    result.z = fminf(max.z, fmaxf(min.z, vector.z));
    result.w = fminf(max.w, fmaxf(min.w, vector.w));

    return result;
}
inline uint32_t PCG_Hash(uint32_t input) {
    uint32_t state = input * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 27703737u;
    return (word >> 22u) ^ word;
}
inline float RandomFloat() {
    // Returns a random real in (0,1).
    return std::rand() / (RAND_MAX + 1.0);
}
inline float RandomFloat(float min, float max) {
    // Returns a random real in (min,max).
    return min + (max - min) * RandomFloat();
}
inline Vector3 RandomVector3() {
    return Vector3{ RandomFloat(), RandomFloat(), RandomFloat() };
}
inline Vector3 RandomVector3(float min, float max) {
    return Vector3{ RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max) };
}
inline Vector3 InUnitSphere() {
    return Vector3Normalize(RandomVector3(-1, 1));
}
inline bool NearZero(Vector3 e) {
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    return (std::fabs(e.x)< s) && (std::fabs(e.y) < s) && (std::fabs(e.z) < s);
}
inline float LinearToGamma(float linearComponent)
{
    if (linearComponent > 0)
        return std::sqrt(linearComponent);
    return 0;
}