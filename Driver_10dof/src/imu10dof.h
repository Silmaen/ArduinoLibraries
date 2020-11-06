//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once
#include "math3d.h"

enum class lsm303AccelRange {
	LSM303_RANGE_2G,  ///< Measurement range from +2G to -2G (19.61 m/s^2)
	LSM303_RANGE_4G,  ///< Measurement range from +4G to -4G (39.22 m/s^2)
	LSM303_RANGE_8G,  ///< Measurement range from +8G to -8G (78.45 m/s^2)
	LSM303_RANGE_16G, ///< Measurement range from +16G to -16G (156.9 m/s^2)
} ;

enum class lsm303AccelMode {
	LSM303_MODE_NORMAL,          ///< Normal measurement mode; 10-bit
	LSM303_MODE_HIGH_RESOLUTION, ///< High resolution mode; 12-bit
	LSM303_MODE_LOW_POWER,       ///< Low power mode; 8-bit
} ;

enum class lsm303MagGain{
	LSM303_MAGGAIN_1_3, ///< +/- 1.3
	LSM303_MAGGAIN_1_9, ///< +/- 1.9
	LSM303_MAGGAIN_2_5, ///< +/- 2.5
	LSM303_MAGGAIN_4_0, ///< +/- 4.0
	LSM303_MAGGAIN_4_7, ///< +/- 4.7
	LSM303_MAGGAIN_5_6, ///< +/- 5.6
	LSM303_MAGGAIN_8_1  ///< +/- 8.1
};

enum class lsm303MagRate{
	LSM303_MAGRATE_0_7, ///< 0.75 Hz
	LSM303_MAGRATE_1_5, ///< 1.5 Hz
	LSM303_MAGRATE_3_0, ///< 3.0 Hz
	LSM303_MAGRATE_7_5, ///< 7.5 Hz
	LSM303_MAGRATE_15,  ///< 15 Hz
	LSM303_MAGRATE_30,  ///< 30 Hz
	LSM303_MAGRATE_75,  ///< 75 Hz
	LSM303_MAGRATE_220  ///< 220 Hz
};

enum class l3gd20GyroSensitivity {
    L3GD20_SENSITIVITY_250DPS,  ///< Sensitivity at 250 dps
    L3GD20_SENSITIVITY_500DPS,  ///< Sensitivity at 500 dps
    L3GD20_SENSITIVITY_2000DPS, ///< Sensitivity at 2000 dps
} ;

class imu10dof {
public:
	// construct, init
	imu10dof();

	// calibration
	void calibration();

	// measure & store
	const vec3f& measureAccel(){m_accel();return __acc;}
	vec3f measureAccelNoGravity(){m_gyro();return __acc-__calibrated;}
	const vec3f& measureGyro(){m_gyro();return __gyro;}
	const vec3f& measureMagneto(){m_Magneto();return __mag;}
	const float& measureTemperature(){m_Temperature();return __temp;}
	const float& measurePressure(){m_Pressure();return __pres;}
	void measureAll(){m_accel();m_gyro();m_Magneto();m_Temperature();m_Pressure();}

	// getter
	const vec3f& getAccel()const noexcept{return __acc;};
	const vec3f& getAccelNoGravity()const noexcept{return __acc;};
	const vec3f& getGyro()const noexcept{return __gyro;};
	const vec3f& getMagneto()const noexcept{return __mag;};
	const float& getTemperature()const noexcept{return __temp;};
	const float& getPressure()const noexcept{return __pres;};

	// settings
	void setRange(lsm303AccelRange range);
	void setMode(lsm303AccelMode mode);
	void setMagGain(lsm303MagGain gain);
	void setMagRate(lsm303MagRate rate);
	void set

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
	l3gd20GyroSensitivity g_sensitivity;

	// calibration
	bool __calibrated;
	vec3f __gravity;
};