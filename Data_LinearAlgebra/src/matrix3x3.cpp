//
// Created by damien.lachouette on 23/11/2020.
//
#include "matrix3x3.h"

namespace math {

    void Matrix3x3::makeRotationBetweenVectors(const vec3f &a, const vec3f &b) {
        vec3f u       = a.cross(b).normalized();            // rotation axe
        float ct      = a.normalized().dot(b.normalized()); // rotation cosius of angle
        float st      = sin(acos(ct));
        (*this)(1, 1) = u[0] * u[0] * (1.f - ct) + ct;
        (*this)(1, 2) = u[0] * u[1] * (1.f - ct) - u[3] * st;
        (*this)(1, 3) = u[0] * u[2] * (1.f - ct) + u[2] * st;
        (*this)(2, 1) = u[1] * u[0] * (1.f - ct) + u[3] * st;
        (*this)(2, 2) = u[1] * u[1] * (1.f - ct) + ct;
        (*this)(2, 3) = u[1] * u[2] * (1.f - ct) - u[0] * st;
        (*this)(3, 1) = u[2] * u[0] * (1.f - ct) - u[2] * st;
        (*this)(3, 2) = u[2] * u[1] * (1.f - ct) + u[0] * st;
        (*this)(3, 3) = u[2] * u[2] * (1.f - ct) + ct;
    }

    void Matrix3x3::makeRotationLookAndUp(const vec3f &lookAt, const vec3f &up) {
        vec3f x = lookAt.normalized();
        vec3f y = up.normalized().cross(x);
        vec3f z = x.cross(y);
        setByColumn(x, y, z);
    }

    bool Matrix3x3::isNull() const {
        for (uint8_t i = 0; i < 9; ++i) {
            if (isNotNull(data[i]))
                return false;
        }
        return true;
    }

    bool Matrix3x3::isIdentity() const {
        for (uint8_t j = 0; j < 3; ++j) {
            for (uint8_t i = 0; i < 3; ++i) {
                if (i == j) {
                    if (isNotEqual(data[3 * i + j], 1.F))
                        return false;
                } else {
                    if (isNotNull(data[3 * i + j]))
                        return false;
                };
            }
        }
        return true;
    }

    bool Matrix3x3::isDiagonal() const {
        for (uint8_t j = 0; j < 3; ++j) {
            for (uint8_t i = 0; i < 3; ++i) {
                if (i == j)
                    continue;
                if (isNotNull(data[3 * i + j]))
                    return false;
            }
        }
        return true;
    }

    bool Matrix3x3::isSymmetric() const {
        for (uint8_t j = 0; j < 3; ++j) {
            for (uint8_t i = j; i < 3; ++i) {
                if (i == j)
                    continue;
                if (isNotEqual(data[3 * i + j], data[3 * j + i]))
                    return false;
            }
        }
        return true;
    }
    bool Matrix3x3::isOthogonal() const { return ((*this) * transposed()).isIdentity(); }

    bool Matrix3x3::isRotation() const { return isEqual(det(), 1.F) && isOthogonal(); }

    Matrix3x3 &Matrix3x3::operator*=(const Matrix3x3 &o) {
        Matrix3x3 a(*this);
        for (uint8_t j = 0; j < 3; ++j) {
            for (uint8_t i = 0; i < 3; ++i) {
                for (uint8_t k = 0; k < 3; ++k) {
                    data[3 * i + j] += a[3 * i + k] * o[3 * k + j];
                }
            }
        }
        return *this;
    }

    Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &o) const {
        Matrix3x3 a;
        for (uint8_t j = 0; j < 3; ++j) {
            for (uint8_t i = 0; i < 3; ++i) {
                for (uint8_t k = 0; k < 3; ++k) {
                    a[3 * i + j] += data[3 * i + k] * o[3 * k + j];
                }
            }
        }
        return a;
    }

    vec3f Matrix3x3::operator*(const vec3f &o) const {
        vec3f a;
        for (uint8_t i = 0; i < 3; ++i) {
            for (uint8_t k = 0; k < 3; ++k) {
                a[i] += data[3 * i + k] * o[k];
            }
        }
        return a;
    }

    vec3f operator*(const vec3f &o, const Matrix3x3 &m) {
        vec3f a;
        for (uint8_t i = 0; i < 3; ++i) {
            for (uint8_t k = 0; k < 3; ++k) {
                a[i] += m[3 * k + i] * o[k];
            }
        }
        return a;
    }

    Matrix3x3 &Matrix3x3::operator*=(const float &o) {
        for (uint8_t i = 0; i < 9; ++i) {
            data[i] *= o;
        }
        return (*this);
    }

    Matrix3x3 Matrix3x3::operator*(const float &o) const {
        Matrix3x3 r(*this);
        r *= o;
        return r;
    }

    Matrix3x3 operator*(const float &o, const Matrix3x3 &m) { return m * o; }

    Matrix3x3 &Matrix3x3::operator/=(const float &o) {
        for (uint8_t i = 0; i < 9; ++i) {
            data[i] /= o;
        }
        return (*this);
    }

    Matrix3x3 Matrix3x3::operator/(const float &o) const {
        Matrix3x3 r(*this);
        r /= o;
        return r;
    }

    float Matrix3x3::doubleDot(const Matrix3x3 &o) const {
        float res = 0;
        for (uint8_t i = 0; i < 3; ++i) {
            for (uint8_t j = 0; j < 3; ++j) {
                res += data[3 * i + j] * o[3 * j + i];
            }
        }
        return res;
    }
    Matrix3x3 &Matrix3x3::transpose() {
        for (uint8_t j = 0; j < 3; ++j) {
            for (uint8_t i = j; i < 3; ++i) {
                if (i == j)
                    continue;
                swap(data[3 * i + j], data[3 * j + i]);
            }
        }
        return *this;
    }

    Matrix3x3 Matrix3x3::transposed() const {
        Matrix3x3 a(*this);
        return a.transpose();
    }

    float Matrix3x3::det() const {
        return data[0] * (data[4] * data[8] - data[7] * data[5]) - data[3] * (data[7] * data[2] - data[1] * data[8]) +
               data[6] * (data[1] * data[5] - data[4] * data[2]);
    }

    Matrix3x3 &Matrix3x3::invert() {
        if (isOthogonal())
            return transpose();
        float d = det();
        if (::math::isNull(d)) // the matrix is not invertible
            return *this;
        Matrix3x3 a(*this);
        (*this)(1, 1) = a(2, 2) * a(3, 3) - a(3, 2) * a(2, 3);
        (*this)(1, 2) = a(1, 3) * a(3, 2) - a(3, 3) * a(1, 2);
        (*this)(1, 3) = a(1, 2) * a(2, 3) - a(2, 2) * a(1, 3);
        (*this)(2, 1) = a(2, 3) * a(3, 1) - a(3, 3) * a(2, 1);
        (*this)(2, 2) = a(1, 1) * a(3, 3) - a(3, 1) * a(1, 3);
        (*this)(2, 3) = a(1, 3) * a(2, 1) - a(2, 3) * a(1, 1);
        (*this)(3, 1) = a(2, 1) * a(3, 2) - a(3, 1) * a(2, 2);
        (*this)(3, 2) = a(1, 2) * a(3, 1) - a(3, 2) * a(1, 1);
        (*this)(3, 3) = a(1, 1) * a(2, 2) - a(2, 1) * a(1, 2);
        (*this) /= d;
        return *this;
    }

    Matrix3x3 Matrix3x3::inverted() const {
        if (isOthogonal())
            return transposed();
        float d = det();
        if (::math::isNull(d)) // the matrix is not invertible
            return *this;
        Matrix3x3 a;
        a(1, 1) = (*this)(2, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(2, 3);
        a(1, 2) = (*this)(1, 3) * (*this)(3, 2) - (*this)(3, 3) * (*this)(1, 2);
        a(1, 3) = (*this)(1, 2) * (*this)(2, 3) - (*this)(2, 2) * (*this)(1, 3);
        a(2, 1) = (*this)(2, 3) * (*this)(3, 1) - (*this)(3, 3) * (*this)(2, 1);
        a(2, 2) = (*this)(1, 1) * (*this)(3, 3) - (*this)(3, 1) * (*this)(1, 3);
        a(2, 3) = (*this)(1, 3) * (*this)(2, 1) - (*this)(2, 3) * (*this)(1, 1);
        a(3, 1) = (*this)(2, 1) * (*this)(3, 2) - (*this)(3, 1) * (*this)(2, 2);
        a(3, 2) = (*this)(1, 2) * (*this)(3, 1) - (*this)(3, 2) * (*this)(1, 1);
        a(3, 3) = (*this)(1, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(1, 2);
        a /= d;
        return a;
    }

} // namespace math
