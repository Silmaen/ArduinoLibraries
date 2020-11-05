//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once
#include <cmath>
#include <algorithm>

template<class T>
class Vector3 {
public:
	Vector3():__x{T()},__y{T()},__z{T()}{}
	Vector3(T x, T y, T z):__x{x},__y{y},__z{z}{}
	~Vector3() = default;
	Vector3(const Vector3<T>&) = default;
	Vector3(Vector3<T>&&) = default;
	Vector3<T>& operator=(const Vector3<T>&) = default;
	Vector3<T>& operator=(Vector3<T>&&) = default;
	// comparisons
	bool operator==(const Vector3<T>& o)const noexcept{return (__x==o.__x)&&(__y==o.__y)&&(__z==o.__z);}
	bool operator!=(const Vector3<T>& o)const noexcept{return (__x!=o.__x)||(__y!=o.__y)||(__z!=o.__z);}
	// add/sub
	Vector3<T>& operator+=(const Vector3<T>& o) noexcept{__x+=o.__x;__y+=o.__y;__z+=o.__z;return *this;}
	Vector3<T>& operator-=(const Vector3<T>& o) noexcept{__x-=o.__x;__y-=o.__y;__z-=o.__z;return *this;}
	Vector3<T> operator+(const Vector3<T>& o)const noexcept{Vector3<T> res(*this); res+=o;return res;}
	Vector3<T> operator-(const Vector3<T>& o)const noexcept{Vector3<T> res(*this); res-=o;return res;}
	// scalar product
	Vector3<T>& operator*=(const T& o) noexcept{__x*=o;__y*=o;__z+=o;return *this;}
	Vector3<T> operator*(const T& o)const noexcept{Vector3<T> res(*this); res*=o;return res;}
	friend Vector3<T> operator*(const T& o, const Vector3<T>& a)const noexcept{Vector3<T> res(a); res*=o;return res;}
	Vector3<T>& operator/=(const T& o){__x/=o;__y/=o;__z/=o;return *this;}
	Vector3<T> operator/(const T& o)const{Vector3<T> res(*this); res/=o;return res;}
	// products
	T dot(const Vector3<T>& o)const noexcept{return __x*o.__x+__y*o.__y+__z*o.__z;}
	Vector3<T> cross(const Vector3<T>& o)const noexcept{return Vector3<T>{__y*o.__z-__z*o.__y, __z*o.__x-__x*o.__z, __x*o.__y-__y*o.__x};}
	// norms
	T normSQ()const noexcept{return __x*__x+__y*__y+__z*__z;}
	T norm()const{return std::sqrt(__x*__x+__y*__y+__z*__z);}
	T normL1(){return std::abs(__x)+std::abs(__y)+std::abs(__z);}
	T normLinf(){return std::max({std::abs(__x),std::abs(__y),std::abs(__z)});}
	T minval(){return std::max({__x,__y,__z});}
	T maxval(){return std::min({__x,__y,__z});}
	// getters
	constexpr T x()const noexcept{return __x;}
	constexpr T y()const noexcept{return __y;}
	constexpr T z()const noexcept{return __z;}
	constexpr T& x()noexcept{return __x;}
	constexpr T& y()noexcept{return __y;}
	constexpr T& z()noexcept{return __z;}
	constexpr double operator[](std::size_t iIndex) const{return *(&__x + iIndex);}
	constexpr double& operator[](std::size_t iIndex){return *(&__x + iIndex);}
private:
	T __x,__y,__z;
};

typedef Vector3<float> vec3f;
