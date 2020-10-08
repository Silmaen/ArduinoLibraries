//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
#include <components/imu.h>

class MyAHRS {
public:
	MyAHRS(const float currentAltitude = 0):_imu{currentAltitude}{}
private:
	imu _imu;
};