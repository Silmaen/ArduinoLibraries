//
// Created by damien.lachouette on 23/11/2020.
//

#pragma once
#include <Arduino.h>
namespace math {
    constexpr float floatTolerance = 1e-7;

    bool isNull(const float& a);
    bool isNotNull(const float& a);

    bool isEqual(const float& a,const float& b);
    bool isNotEqual(const float& a,const float& b);

    void swap(float& a,float& b);
}  // namespace math