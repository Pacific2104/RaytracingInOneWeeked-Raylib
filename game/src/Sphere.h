#pragma once
#include "Hittable.h"

class Sphere : public Hittable {
public:
    Sphere(const Vector3& center, float radius) : center(center), radius(fmaxf(0, radius)){}

    bool Hit(const Ray& r, Interval rayT, HitRecord& rec) const override {
        Vector3 oc = center - r.position;
        auto a = Vector3LengthSqr(r.direction);
        auto h = Vector3DotProduct(r.direction, oc);
        auto c = Vector3LengthSqr(oc) - (radius * radius);

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!rayT.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!rayT.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.position + r.direction * rec.t;
        Vector3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    Vector3 center;
    float radius;
};