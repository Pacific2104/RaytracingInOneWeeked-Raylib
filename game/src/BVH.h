#pragma once

#include "HittableList.h"

class BvhNode :public Hittable
{
public:
    BvhNode(HittableList list) :BvhNode(list.objects, 0, list.objects.size()) {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }
    BvhNode(std::vector<shared_ptr<Hittable>>& object, size_t start, size_t end) {
        // To be implemented later.
    }
    bool Hit(const Ray& r, Interval rayT, HitRecord& rec) const override {
        if (!bbox.Hit(r, rayT))
            return false;

        bool hit_left = left->Hit(r, rayT, rec);
        bool hit_right = right->Hit(r, Interval(rayT.min, hit_left ? rec.t : rayT.max), rec);

        return hit_left || hit_right;
    }

    AABB BoundingBox() const override { return bbox; }
private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB bbox;
};