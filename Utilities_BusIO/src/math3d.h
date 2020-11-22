//
// Created by Silmaen on 05/11/2020.
//

#pragma once
#include <Arduino.h>

template<class T>
class Vector3 {
public:
    Vector3() = default;
    Vector3(T x, T y, T z) : _x{x}, _y{y}, _z{z} {}
    ~Vector3()                      = default;
    Vector3(const Vector3<T> &)     = default;
    Vector3(Vector3<T> &&) noexcept = default;
    virtual Vector3<T> &operator    =(const Vector3<T> &o) {
        _x = o._x;
        _y = o._y;
        _z = o._z;
        return *this;
    }
    virtual Vector3<T> &operator=(Vector3<T> &&o) noexcept {
        _x = o._x;
        _y = o._y;
        _z = o._z;
        return *this;
    }
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

    // getters
    T  x() const noexcept { return _x; }
    T  y() const noexcept { return _y; }
    T  z() const noexcept { return _z; }
    T &x() noexcept { return _x; }
    T &y() noexcept { return _y; }
    T &z() noexcept { return _z; }

    T  operator[](uint8_t iIndex) const { return *(&_x + iIndex); }
    T &operator[](uint8_t iIndex) { return *(&_x + iIndex); }

private:
    T _x, _y, _z;
};

class vec3f : public Vector3<float> {
public:
    explicit vec3f(float x = 0, float y = 0, float z = 0) : Vector3<float>{x, y, z} {}
    vec3f &operator=(const Vector3<float> &o) override {
        x() = o.x();
        y() = o.y();
        z() = o.z();
        return *this;
    }
    vec3f &operator=(Vector3<float> &&o) noexcept override {
        x() = o.x();
        y() = o.y();
        z() = o.z();
        return *this;
    }
    // products
    float dot(const vec3f &o) const noexcept { return x() * o.x() + y() * o.y() + z() * o.z(); }
    vec3f cross(const vec3f &o) const noexcept {
        return vec3f{y() * o.z() - z() * o.y(), z() * o.x() - x() * o.z(), x() * o.y() - y() * o.x()};
    }
    // norms
    float normSQ() const noexcept { return x() * x() + y() * y() + z() * z(); }
    float norm() const { return sqrt(x() * x() + y() * y() + z() * z()); }
    float normL_1() { return abs(x()) + abs(y()) + abs(z()); }
    float normL_inf() { return max(max(abs(x()), abs(y())), abs(z())); }
    float min_val() { return max(max(x(), y()), z()); }
    float max_val() { return min(min(x(), y()), z()); }
};

// using vec3s16 = Vector3<int16_t>;
class vec3s16 : public Vector3<int16_t> {
public:
    explicit vec3s16(int16_t x = 0, int16_t y = 0, int16_t z = 0) : Vector3<int16_t>{x, y, z} {}
    vec3s16 &operator=(const Vector3<int16_t> &o) override {
        x() = o.x();
        y() = o.y();
        z() = o.z();
        return *this;
    }
    vec3s16 &operator=(Vector3<int16_t> &&o) noexcept override {
        x() = o.x();
        y() = o.y();
        z() = o.z();
        return *this;
    }
    // products
    int16_t dot(const vec3s16 &o) const noexcept { return x() * o.x() + y() * o.y() + z() * o.z(); }
    vec3s16 cross(const vec3s16 &o) const noexcept {
        return vec3s16{static_cast<int16_t>(y() * o.z() - z() * o.y()), static_cast<int16_t>(z() * o.x() - x() * o.z()),
                       static_cast<int16_t>(x() * o.y() - y() * o.x())};
    }
    // norms
    int16_t normSQ() const noexcept { return x() * x() + y() * y() + z() * z(); }
    int16_t norm() const { return sqrt(x() * x() + y() * y() + z() * z()); }
    int16_t normL_1() { return abs(x()) + abs(y()) + abs(z()); }
    int16_t normL_inf() { return max(max(abs(x()), abs(y())), abs(z())); }
    int16_t min_val() { return max(max(x(), y()), z()); }
    int16_t max_val() { return min(min(x(), y()), z()); }
};