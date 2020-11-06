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
						 lsm303::AccelReg1AValue(__accelSetting.accelMode, __accelSetting.accelRate));
	// set the accelerometer behavior (bigendian, 2G range, Low resolution)
	i2c_utils::writeCommand(lsm303::AddressAccel,
						 lsm303::AccelRegisters::CTRL_REG4_A,
						 lsm303::AccelReg4AValue(__accelSetting.accelRange, __accelSetting.accelResolution));
	// Magnetometer
	i2c_utils::writeCommand(lsm303::AddressMag,
						 lsm303::MagRegisters::MR_REG_M,
						 0x00u);

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
	__accelSetting.accelRate = rate;
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
	__accelSetting.accelMode = mode;
}
void imu10dof::setAccelRange(lsm303::AccelRange range){
	if (range == getAccelRange())
		return;
	uint8_t reg4A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A);
	reg4A &= 0b11001111u; // erase range bits
	reg4A |=(uint8_t)((uint8_t)range << 4u);
	i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A, reg4A);
	__accelSetting.accelRange = range;
	__accelSetting.factor = lsm303::getMSperBit(range);
}
void imu10dof::setAccelResolution(lsm303::AccelResolution hiRes){
	if (hiRes == getAccelResolution())
		return;
	uint8_t reg4A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A);
	reg4A &= 0b11110111u; // erase range bits
	reg4A |= (uint8_t)((uint8_t)hiRes << 3u);
	i2c_utils::writeCommand(lsm303::AddressAccel, lsm303::AccelRegisters::CTRL_REG4_A, reg4A);
	__accelSetting.accelResolution = hiRes;
}

void imu10dof::m_accel() {
	uint8_t status_reg_A = i2c_utils::read8(lsm303::AddressAccel, lsm303::AccelRegisters::STATUS_REG_A);
	if (!status_reg_A) // nothing has changed
		return;
	vec3s16 raw = i2c_utils::readV16(lsm303::AddressAccel, lsm303::AccelRegisters::OUT_X_L_A);
	__acc.x() = (float)raw.x() * __accelSetting.factor;
	__acc.y() = (float)raw.y() * __accelSetting.factor;
	__acc.z() = (float)raw.z() * __accelSetting.factor;
}

/* *******************************************
 *  MAGNETTOMETER Functions
 * *******************************************/


void imu10dof::setMagRate(lsm303::MagRate rate){
    if (rate == getMagRate())
        return;
    i2c_utils::writeCommand(lsm303::AddressMag,
                            lsm303::MagRegisters::CRA_REG_M,
                            (uint8_t)rate);
    __magSetting.magRate = rate;
}

void imu10dof::setMagRange(lsm303::MagRange range){
    if (range == getMagGain())
        return;
    i2c_utils::writeCommand(lsm303::AddressMag,
                            lsm303::MagRegisters::CRB_REG_M,
                            (uint8_t)range);
    __magSetting.magRange = range;
    __magSetting.factorXY = lsm303::getXYGain(range);
    __magSetting.factorZ = lsm303::getZGain(range);
}

void imu10dof::m_Magneto() {
    uint8_t status_reg_A = i2c_utils::read8(lsm303::AddressMag, lsm303::MagRegisters::SR_REG_Mg);
    if (!status_reg_A) // nothing has changed
        return;
    vec3s16 raw = i2c_utils::readV16(lsm303::AddressMag, lsm303::MagRegisters::OUT_X_H_M, true);
    __mag.x() = (float)raw.x() * __magSetting.factorXY;
    __mag.y() = (float)raw.y() * __magSetting.factorXY;
    __mag.z() = (float)raw.z() * __magSetting.factorZ;
}


void imu10dof::m_gyro() {

}

void imu10dof::m_Temperature() {

}

void imu10dof::m_Pressure() {

}