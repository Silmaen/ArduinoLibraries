//
// Created by damien.lachouette on 23/11/2020.
//
#include "complex.h"

namespace math {

    Complex &Complex::fromModArg(const float &mod, const float &arg) noexcept {
        real      = mod * cosf(arg);
        imaginary = mod * sinf(arg);
        return *this;
    }

    Complex &Complex::operator/=(const Complex &o) {
        float den = o.mod();
        if (isNull(den))
            return *this;
        float tr  = real;
        real      = (real * o.real + imaginary * o.imaginary) / den;
        imaginary = (imaginary * o.real - tr * o.imaginary) / den;
        return *this;
    }

    float Complex::arg() const { return atan2f(imaginary, real); }

    Complex sqrt(const Complex &c) {
        Complex res;
        res.fromModArg(sqrtf(c.mod()), c.arg() / 2.F);
        return res;
    }
} // namespace math