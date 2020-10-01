//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
#include <Arduino.h>
#include "i2c_utils.h"
#include "bmp085.h"
#include "lsm303.h"
#include "l3gd20.h"


struct vec3d{
	float x,y,z;
	vec3d() = default;
	vec3d(const vec3d&) = default;
	vec3d(vec3d&&) = default;
	~vec3d() = default;
	vec3d& operator=(const vec3d&) = default;
	vec3d& operator=(vec3d&&) = default;
	vec3d(const i2c_utils::v3& o){x=(float)o.x;y=(float)o.y;z=(float)o.z;}
	vec3d& operator*=(const float a){x*=a;y*=a;z*=a;return *this}
	vec3d operator*(const float a) const{v(*this);v*=a;return v;}
};

class imu{
	imu(const float currentAltitude=0);
	vec3d getAcceleration() const;
	vec3D getRotationRate() const;
	void setGyroRange(const l3gd20::gyroRange& range);
	vec3D getMagneticField() const;
	void setMagGain(const lsm303::MagGain& gain);
	void setMagRate(const lsm303::MagRate& rate);
	float getPressure() const;
	float getAltitude() const;
	float getTemperature() const;
	void setReferenceAltitude(const float currentAltitude);
private:
	float getTemperatureK()const;
	float qnh;
	float Gauss_LSB_XY;  // Varies with gain
	float Gauss_LSB_Z;   // Varies with gain
	l3gd20::gyroRange gRange;
	bmp085::calibrationData cal;
	void getCalibrationData();
};
