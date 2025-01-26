#pragma once

#include "math.h"
class Interval {
public:
    double min, max;

    Interval() : min(INFINITY), max(-INFINITY) {} // Default interval is empty

    Interval(double min, double max) : min(min), max(max) {}

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    static const Interval empty, universe;
};