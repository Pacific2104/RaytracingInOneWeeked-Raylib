#pragma once
#include "Interval.h"
#include "raylib.h"

class AABB
{
public:
    Interval x, y, z;

    AABB() {}

    AABB(const Interval& x, const Interval& y, const Interval& z) :x(x), y(y), z(z) {}

    AABB(const Vector3& a, const Vector3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
       // particular minimum/maximum coordinate order.
        x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
        y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
        z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
    }
    AABB(const AABB& box0, const AABB& box1) {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);
    }

    const Interval& AxisInterval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool Hit(const Ray& r, Interval rayT) const {
        const Vector3& rayOrigin = r.position;
        const Vector3& rayDir = r.direction;
 
        const float origin[3] = { rayOrigin.x, rayOrigin.y, rayOrigin.z };
        const float dir[3] = { rayDir.x, rayDir.y, rayDir.z };

        for (int axis = 0; axis < 3; axis++) {
            const Interval& ax = AxisInterval(axis);
            const double adinv = 1.0f / dir[axis];

            double t0 = (ax.min - origin[axis]) * adinv;
            double t1 = (ax.max - origin[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > rayT.min) rayT.min = t0;
                if (t1 < rayT.max) rayT.max= t1;
            }
            else {
                if (t1 > rayT.min) rayT.min = t1;
                if (t0 < rayT.max) rayT.max = t0;
            }

            if (rayT.max <= rayT.max) 
                return false;
        }
        return true;
    }
};