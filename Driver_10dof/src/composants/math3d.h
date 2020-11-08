//
// Created by Silmaen on 05/11/2020.
//

#pragma once
#include <Arduino.h>

template<class T>
class Vector3 {
public:
    Vector3() : _x{T()}, _y{T()}, _z{T()} {}
    Vector3(T x, T y, T z) : _x{x}, _y{y}, _z{z} {}
    ~Vector3()                      = default;
    Vector3(const Vector3<T> &)     = default;
    Vector3(Vector3<T> &&) noexcept = default;
    Vector3<T> &operator=(const Vector3<T> &) = default;
    Vector3<T> &operator=(Vector3<T> &&) noexcept = default;
    // comparisons
    bool operator==(const Vector3<T> &o) const noexcept { return (_x == o._x) && (_y == o._y) && (_z == o._z); }
    bool operator!=(const Vector3<T> &o) const noexcept { return (_x != o._x) || (_y != o._y) || (_z != o._z); }
    // add/sub

    Vector3<T> &operator+=(const Vector3<T> &o) noexcept {
        _x += o._x;
        _y += o._y;
        _z += o._z;
        return *this;
    }

    Vector3<T> &operator-=(const Vector3<T> &o) noexcept {
        _x -= o._x;
        _y -= o._y;
        _z -= o._z;
        return *this;
    }

    Vector3<T> operator+(const Vector3<T> &o) const noexcept {
        Vector3<T> res(*this);
        res += o;
        return res;
    }

    Vector3<T> operator-(const Vector3<T> &o) const noexcept {
        Vector3<T> res(*this);
        res -= o;
        return res;
    }

    // scalar product
    Vector3<T> &operator*=(const T &o) noexcept {
        _x *= o;
        _y *= o;
        _z += o;
        return *this;
    }

    Vector3<T> operator*(const T &o) const noexcept {
        Vector3<T> res(*this);
        res *= o;
        return res;
    }

    friend Vector3<T> operator*(const T &o, const Vector3<T> &a) noexcept {
        Vector3<T> res(a);
        res *= o;
        return res;
    }

    Vector3<T> &operator/=(const T &o) {
        _x /= o;
        _y /= o;
        _z /= o;
        return *this;
    }

    Vector3<T> operator/(const T &o) const {
        Vector3<T> res(*this);
        res /= o;
        return res;
    }

    // products
    T dot(const Vector3<T> &o) const noexcept { return _x * o._x + _y * o._y + _z * o._z; }

    Vector3<T> cross(const Vector3<T> &o) const noexcept {
        return Vector3<T>{_y * o._z - _z * o._y, _z * o._x - _x * o._z, _x * o._y - _y * o._x};
    }

    // norms
    T normSQ() const noexcept { return _x * _x + _y * _y + _z * _z; }

    T norm() const { return sqrt(_x * _x + _y * _y + _z * _z); }

    T normL_1() { return abs(_x) + abs(_y) + abs(_z); }

    T normL_inf() { return max(max(abs(_x), abs(_y)), abs(_z)); }

    T min_val() { return max(max(_x, _y), _z); }

    T max_val() { return min(min(_x, _y), _z); }

    // getters
    T x() const noexcept { return _x; }

    T y() const noexcept { return _y; }

    T z() const noexcept { return _z; }

    T &x() noexcept { return _x; }

    T &y() noexcept { return _y; }

    T &z() noexcept { return _z; }

    T operator[](uint8_t iIndex) const { return *(&_x + iIndex); }

    T &operator[](uint8_t iIndex) { return *(&_x + iIndex); }

private:
    T _x, _y, _z;
};

using vec3f   = Vector3<float>;
using vec3s16 = Vector3<int16_t>;
