//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once
#include "composants/math3d.h"
#include "composants/lsm303.h"

enum class l3gd20GyroSensitivity {
	L3GD20_SENSITIVITY_250DPS,  ///< Sensitivity at 250 dps
	L3GD20_SENSITIVITY_500DPS,  ///< Sensitivity at 500 dps
	L3GD20_SENSITIVITY_2000DPS, ///< Sensitivity at 2000 dps
};

struct bmp180Calibration {
	int16_t ac1;
	int16_t ac2;
	int16_t ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1;
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
};

class imu10dof {
public:
	// construct, init
	imu10dof();

	// calibration
	void calibration();

	// measure & store
	const vec3f& measureAccel(){m_accel();return __acc;}
	const vec3f& measureGyro(){m_gyro();return __gyro;}
	const vec3f& measureMagneto(){m_Magneto();return __mag;}
	const float& measureTemperature(){m_Temperature();return __temp;}
	const float& measurePressure(){m_Pressure();return __pres;}
	void measureAll(){m_accel();m_gyro();m_Magneto();m_Temperature();m_Pressure();}

	// getter
	[[nodiscard]] const vec3f& getAccel()const noexcept{return __acc;};
	[[nodiscard]] const vec3f& getAccelNoGravity()const noexcept{return __acc;};
	[[nodiscard]] const vec3f& getGyro()const noexcept{return __gyro;};
	[[nodiscard]] const vec3f& getMagneto()const noexcept{return __mag;};
	[[nodiscard]] const float& getTemperature()const noexcept{return __temp;};
	[[nodiscard]] const float& getPressure()const noexcept{return __pres;};

	// settings
	//   accelerometer
	void setAccelRate(lsm303::AccelRate rate);
	void setAccelMode(lsm303::AccelMode mode);
	void setAccelRange(lsm303::AccelRange range);
	void setAccelResolution(lsm303::AccelResolution hiRes);
	[[nodiscard]] const lsm303::AccelRate& getAccelRate() const {return __accelRate;}
	[[nodiscard]] const lsm303::AccelMode& getAccelMode() const {return __accelMode;}
	[[nodiscard]] const lsm303::AccelRange& getAccelRange() const {return __accelRange;}
	[[nodiscard]] const lsm303::AccelResolution& getAccelResolution() const {return __accelResolution;}

private:

	void m_accel();
	void m_gyro();
	void m_Magneto();
	void m_Temperature();
	void m_Pressure();

	// data
	vec3f __acc;
	vec3f __gyro;
	vec3f __mag;
	float __temp;
	float __pres;

	//  settings
	//    Accelerometer
	lsm303::AccelRate __accelRate = lsm303::AccelRate::HZ_100;
	lsm303::AccelMode __accelMode = lsm303::AccelMode::NORMAL;
	lsm303::AccelRange __accelRange = lsm303::AccelRange::R_2G;
	lsm303::AccelResolution __accelResolution = lsm303::AccelResolution::LOW_RES;
	float __factor = 0.00980665F;


};