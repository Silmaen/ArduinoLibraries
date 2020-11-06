//
// Created by damien.lachouette on 05/11/2020.
//

#include "imu10dof.h"

imu10dof::imu10dof():__acc{}, __gyro{}, __mag{}, __temp{}, __pres{}, __calibrated{false}, __gravity{} {

}

#define LSM303_ID (0b11010100) //!< Chip ID
#define LSM303_ADDRESS_ACCEL (0x32 >> 1) //!< I2C address/bits, 0011001x
#define LSM303_ADDRESS_MAG 0x1E          //!< I2C address/bits,



void imu10dof::calibration() {

}

void imu10dof::m_accel() {

}

void imu10dof::m_Magneto() {

}

void imu10dof::m_gyro() {

}

void imu10dof::m_Temperature() {

}

void imu10dof::measurePressure() {

}