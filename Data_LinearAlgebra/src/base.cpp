//
// Created by damien.lachouette on 23/11/2020.
//
#include "bases.h"

namespace math {

    bool isNull(const float a) { return (abs(a) <= floatTolerance); }

    bool isNotNull(const float a) { return (abs(a) > floatTolerance); }

    bool isEqual(const float a, const float b) { return (abs(a - b) <= floatTolerance); }

    bool isNotEqual(const float a, const float b) { return (abs(a - b) > floatTolerance); }

    void swap(float &a, float &b) {
        float t{a};
        a = b;
        b = t;
    }

} // namespace math