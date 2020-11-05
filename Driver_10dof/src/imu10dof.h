//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once
#include "math3d.h"

class imu10dof {
public:
	// construct, init
	imu10dof();

	// calibration
	void calibration();

	// measure & store
	const vec3f& measureAccel();
	const vec3f& measureAccelNoGravity();
	const vec3f& measureGyro();
	const vec3f& measureMagneto();
	const float& measureTemperature();
	const float& measurePressure();
	void measureAll();

	// getter
	const vec3f& getAccel()const noexcept{return __acc;};
	const vec3f& getAccelNoGravity()const noexcept{return __acc;};
	const vec3f& getGyro()const noexcept{return __gyro;};
	const vec3f& getMagneto()const noexcept{return __mag;};
	const float& getTemperature()const noexcept{return __temp;};
	const float& getPressure()const noexcept{return __pres;};

private:

	void m_accel();
	void m_gyro();

	// data
	vec3f __acc;
	vec3f __gyro;
	vec3f __mag;
	float __temp;
	float __pres;
	// calibration
	bool __calibrated;
	vec3f __gravity;
};