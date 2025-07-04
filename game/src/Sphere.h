#pragma once
#include "Hittable.h"

class Sphere : public Hittable {
public:
    Sphere(const Vector3& center, float radius, shared_ptr<Mat> mat) :
        center(center), radius(fmaxf(0, radius)), mat(mat)
    {
        Vector3 rvec = Vector3{ radius, radius, radius };
        bbox = AABB(center - rvec, center + rvec);
    }

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
        rec.SetFaceNormal(r, outward_normal);
        rec.mat = mat;
        return true;
    }
    AABB BoundingBox() const override { return bbox; }
private:
    Vector3 center;
    float radius;
    std::shared_ptr<Mat> mat;
    AABB bbox;
};