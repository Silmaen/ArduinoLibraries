//
// Created by damien.lachouette on 05/11/2020.
//

#include "imu10dof.h"
#include "composants/i2c_utils.h"


imu10dof::imu10dof():
	__acc{}, __gyro{}, __mag{}, __temp{}, __pres{}{

	// Accelerometer
	// Enable the accelerometer (normal, 100Hz, all axis activated)
	i2c_utils::writeCommand(lsm303::AddressAccel,
						 lsm303::AccelRegisters::CTRL_REG1_A,
						 lsm303::AccelReg1AValue(__accelMode, __accelRate));
	// set the accelerometer behavior (bigendian, 2G range, Low resolution)
	i2c_utils::writeCommand(lsm303::AddressAccel,
						 lsm303::AccelRegisters::CTRL_REG4_A,
						 lsm303::AccelReg4AValue(__accelRange, __accelResolution));

}

/* *******************************************
 *  ACCELEROMETER Functions
 * *******************************************/

void imu10dof::setAccelRate(lsm303::AccelRate rate){
	if (rate == getAccelRate())
		return;
	uint8_t reg1A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG1_A);
	reg1A &= 0b1111u; // keep only the 4 first bist
	reg1A |= (uint8_t)rate; // setup the rate
	i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG1_A, 0); // clear all
	i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG1_A, reg1A); // set
}
void imu10dof::setAccelMode(lsm303::AccelMode mode){
	if (mode == getAccelMode())
		return;
	uint8_t reg1A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG1_A);
	if (mode == lsm303::AccelMode::OFF) {
		i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG1_A, 0);
		return;
	}
	if (mode == lsm303::AccelMode::LOW_POWER)
		reg1A |= 0b00001000u;
	else
		reg1A &= 0b11110111u;
	i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG1_A, reg1A);
}
void imu10dof::setAccelRange(lsm303::AccelRange range){
	if (range == getAccelRange())
		return;
	uint8_t reg4A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A);
	reg4A &= 0b11001111u; // erase range bits
	reg4A |=(uint8_t)((uint8_t)range << 4u);
	i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A, reg4A);
	__factor = lsm303::getMSperBit(range);
}
void imu10dof::setAccelResolution(lsm303::AccelResolution hiRes){
	if (hiRes == getAccelResolution())
		return;
	uint8_t reg4A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A);
	reg4A &= 0b11110111u; // erase range bits
	reg4A |= (uint8_t)((uint8_t)hiRes << 3u);
	i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A, reg4A);
}

void imu10dof::m_accel() {
	uint8_t status_reg_A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::STATUS_REG_A);
	if (!status_reg_A) // nothing has changed
		return;
	vec3s16 raw = i2c_utils::readV16(lsm303::AddressAccel, lsm303::AccelRegisters::OUT_X_L_A);
	__acc.x() = (float)raw.x() * __factor;
	__acc.y() = (float)raw.y() * __factor;
	__acc.z() = (float)raw.z() * __factor;
}

void imu10dof::calibration() {

}


void imu10dof::m_Magneto() {

}

void imu10dof::m_gyro() {

}

void imu10dof::m_Temperature() {

}

void imu10dof::m_Pressure() {

}