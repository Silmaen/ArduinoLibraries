//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once
#include <Arduino.h>

template<class T>
class Vector3 {
public:
	Vector3():__x{T()},__y{T()},__z{T()}{}
	Vector3(T x, T y, T z):__x{x},__y{y},__z{z}{}
	~Vector3() = default;
	Vector3(const Vector3<T>&) = default;
	Vector3(Vector3<T>&&)noexcept = default ;
	Vector3<T>& operator=(const Vector3<T>&) = default;
	Vector3<T>& operator=(Vector3<T>&&)noexcept = default;
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
	friend Vector3<T> operator*(const T& o, const Vector3<T>& a)noexcept{Vector3<T> res(a); res*=o;return res;}
	Vector3<T>& operator/=(const T& o){__x/=o;__y/=o;__z/=o;return *this;}
	Vector3<T> operator/(const T& o)const{Vector3<T> res(*this); res/=o;return res;}
	// products
	T dot(const Vector3<T>& o)const noexcept{return __x*o.__x+__y*o.__y+__z*o.__z;}
	Vector3<T> cross(const Vector3<T>& o)const noexcept{return Vector3<T>{__y*o.__z-__z*o.__y, __z*o.__x-__x*o.__z, __x*o.__y-__y*o.__x};}
	// norms
	T normSQ()const noexcept{return __x*__x+__y*__y+__z*__z;}
	T norm()const{return sqrt(__x*__x+__y*__y+__z*__z);}
	T normL1(){return abs(__x)+abs(__y)+abs(__z);}
	T normLinf(){return max(max(abs(__x),abs(__y)),abs(__z));}
	T minval(){return max(max(__x,__y),__z);}
	T maxval(){return min(min(__x,__y),__z);}
	// getters
	T x()const noexcept{return __x;}
	T y()const noexcept{return __y;}
	T z()const noexcept{return __z;}
	T& x()noexcept{return __x;}
	T& y()noexcept{return __y;}
	T& z()noexcept{return __z;}
	T operator[](uint8_t iIndex)const{return *(&__x + iIndex);}
	T& operator[](uint8_t iIndex){return *(&__x + iIndex);}
private:
	T __x,__y,__z;
};

typedef Vector3<float> vec3f;
typedef Vector3<int16_t> vec3s16;
