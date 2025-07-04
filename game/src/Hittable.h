#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Interval.h"
#include "AABB.h"
#include <iostream>

class Mat;

class HitRecord {
public:
    Vector3 p;
    Vector3 normal;
    float t;
    bool front_face;
    std::shared_ptr<Mat> mat;

    void SetFaceNormal(const Ray& r, const Vector3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = Vector3DotProduct(r.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : Vector3Negate(outward_normal);
    }
};
class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& r, Interval rayT, HitRecord& rec) const = 0;

    virtual AABB BoundingBox() const = 0;
};