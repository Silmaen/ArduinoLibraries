//
// Created by damien.lachouette on 01/10/2020.
//

#include "imu.h"
namespace l3gd20 {
	float getSensitivity(gyroRange r){
		switch (r){
			case gyroRange::R250DPS:
				return GyroSensitivity_250DPS;
			case gyroRange::R500DPS:
				return GyroSensitivity_500DPS;
			case gyroRange::R2000DPS:
				return GyroSensitivity_2000DPS;
		}
		return 0;
	}
}

imu::imu(const float currentAltitude): qnh{1013.25}{
	Wire.begin();
	setReferenceAltitude(currentAltitude);
	// BMP085
	// temperature pressure calibration
	if (i2c_utils::read8(bmp085::AddressPress, (byte)bmp085::pressRegister::CHIPID) == 0x5)
		getCalibrationData();
	// LSM303
	// Enable the magnetometer
	i2c_utils::writeCommand(lsm303::AddressMag, (byte)lsm303::MagRegisters::MR_REG_M, 0x00);
	if (i2c_utils::read8(lsm303::AddressMag, (byte)lsm303::MagRegisters::CRA_REG_M) == 0x10)
		// Set the gain to a known level (Make it as parameter)
		setMagGain(lsm303::MagGain::G1_3);
	// Enable the accelerometer (100Hz)
	i2c_utils::writeCommand(lsm303::AddressAccel, (byte)lsm303::AccelRegisters::CTRL_REG1_A, 0x57);
	// L3GD20
	// init gyro
	i2c_utils::writeCommand(l3gd20::AdressGyro, (byte)l3gd20::gyroRegisters::CTRL_REG1, 0x00);
	i2c_utils::writeCommand(l3gd20::AdressGyro, (byte)l3gd20::gyroRegisters::CTRL_REG1, 0x0F);
	setGyroRange(l3gd20::gyroRange::R2000DPS);
}

/* Constants */
constexpr float GRAVITY_EARTH = 9.80665F; /**< Earth's gravity in m/s^2 */
constexpr float GRAVITY_MOON = 1.6F;      /**< The moon's gravity in m/s^2 */
constexpr float GRAVITY_SUN = 275.0F;     /**< The sun's gravity in m/s^2 */
constexpr float GRAVITY_STANDARD = GRAVITY_EARTH;
constexpr float MG_LSB     = 0.001F * GRAVITY_EARTH;   // 1, 2, 4 or 12 mg per lsb

vec3d imu::getAcceleration()const{
	i2c_utils::v3 raw = i2c_utils::readV16(lsm303::AddressAccel, (byte)lsm303::AccelRegisters::OUT_X_L_A|0x80, false);
	// WHY??? these three lines?
	raw.x >>= 4;
	raw.y >>= 4;
	raw.z >>= 4;
	// conversion
	vec3d resu{raw};
	resu *= MG_LSB;
	return resu;
}

constexpr float DPS_TO_RADS = 0.017453293F;

vec3d imu::getRotationRate()const{
	vec3d resu = i2c_utils::readV16(l3gd20::AdressGyro, (byte)l3gd20::gyroRegisters::OUT_X_L | 0x80, false);
	/* Convert values to rad/s */
	resu *= l3gd20::getSensitivity(gRange) * DPS_TO_RADS;
	return resu;
}

constexpr float GAUSS_TO_MICROTESLA = 100.0f;

vec3d imu::getMagneticField()const{
	i2c_utils::v3 raw = i2c_utils::readV16(lsm303::AddressMag, (byte)lsm303::MagRegisters::OUT_X_H_M);
	vec3d resu;
	resu.x = (float)raw.x / Gauss_LSB_XY * GAUSS_TO_MICROTESLA;
	resu.y = (float)raw.y / Gauss_LSB_XY * GAUSS_TO_MICROTESLA;
	resu.z = (float)raw.z / Gauss_LSB_Z * GAUSS_TO_MICROTESLA;
	return resu;
}

float imu::getPressure()const{
	int32_t UT, UP,  B6, X1, p;
	uint32_t B4, B7;
	// in further release, make this as a parameter
	uint8_t oversampling= (uint8_t)bmp085::mode::ULTRAHIGHRES; // maximum resolution

	i2c_utils::writeCommand(bmp085::AddressPress, (byte)bmp085::pressRegister::CONTROL, (byte)bmp085::pressRegister::READTEMPCMD);
	UT = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::TEMPDATA);

	i2c_utils::writeCommand(bmp085::AddressPress, (byte)bmp085::pressRegister::CONTROL, (byte)bmp085::pressRegister::READPRESSURECMD + (oversampling << 6));
	if (oversampling == (uint8_t)bmp085::mode::ULTRALOWPOWER)
		delay(5);
	else if (oversampling == (uint8_t)bmp085::mode::STANDARD)
		delay(8);
	else if (oversampling == (uint8_t)bmp085::mode::HIGHRES)
		delay(14);
	else
		delay(26);
	UP = i2c_utils::read16(bmp085::AddressPress, (byte)bmp085::pressRegister::PRESSUREDATA);
	UP <<= 8;
	UP |= i2c_utils::read8(bmp085::AddressPress, (byte)bmp085::pressRegister::PRESSUREDATA + 2);
	UP >>= (8 - oversampling);

	X1 = (UT - (int32_t)cal.ac6) * ((int32_t)cal.ac5) >> 15;
	B6 = X1 + ((int32_t)cal.mc << 11) / (X1 + (int32_t)cal.md) - 4000;
	B4 = ((uint32_t)cal.ac4 * (uint32_t)(((((((int32_t)cal.ac3 * B6) >> 13) + (((int32_t)cal.b1 * ((B6 * B6) >> 12)) >> 16)) + 2) >> 2) + 32768)) >> 15;
	B7 = ((uint32_t)UP - (((((int32_t)cal.ac1 * 4 + (((int32_t)cal.b2 * ((B6 * B6) >> 12)) >> 11) + (((int32_t)cal.ac2 * B6) >> 11)) << oversampling) + 2) / 4)) * (uint32_t)(50000UL >> oversampling);
	p = (B7 < 0x80000000)?(B7 * 2) / B4:(B7 / B4) * 2;
	p += ((((((p >> 8) * (p >> 8)) * 3038) >> 16) + ((-7357 * p) >> 16) + (int32_t)3791) >> 4);
	return p;
}

float imu::getAltitude()const{
	return getTemperatureK()/0.0065 * ( 1.0 - pow(getPressure() / qnh, 0.19029496)  );// 1/5.255 = 0.19029496
}

float imu::getTemperature()const{
	int32_t UT, B5; // following ds convention
	float temp;
	i2c_utils::writeCommand(bmp085::AddressPress, (byte)bmp085::pressRegister::CONTROL, (byte)bmp085::pressRegister::READTEMPCMD);
	UT = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::TEMPDATA);
	int32_t X1 = (UT - (int32_t)cal.ac6) * ((int32_t)cal.ac5) >> 15;
	B5 = X1 + ((int32_t)cal.mc << 11) / (X1 + (int32_t)cal.md);
	temp = (B5 + 8) >> 4;
	temp /= 10;
	return temp;
}

float imu::getTemperatureK()const{
	return 273.15 + getTemperature();
}

void imu::setReferenceAltitude(const float currentAltitude){
	qnh = getPressure() * pow(1.0 - 0.0065 / getTemperatureK() * currentAltitude , -5.255);
}

void imu::getCalibrationData(){
	cal.ac1 = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_AC1);
	cal.ac2 = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_AC2);
	cal.ac3 = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_AC3);
	cal.ac4 = i2c_utils::read16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_AC4);
	cal.ac5 = i2c_utils::read16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_AC5);
	cal.ac6 = i2c_utils::read16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_AC6);
	cal.b1 = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_B1);
	cal.b2 = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_B2);
	cal.mb = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_MB);
	cal.mc = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_MC);
	cal.md = i2c_utils::readS16(bmp085::AddressPress, (byte)bmp085::pressRegister::CAL_MD);
}

void imu::setMagGain(const lsm303::MagGain& gain) {
	i2c_utils::writeCommand(lsm303::AddressMag, (byte)lsm303::MagRegisters::CRB_REG_M, (byte)gain);
	switch (gain) {
		case lsm303::MagGain::G1_3:
			Gauss_LSB_XY = 1100;
			Gauss_LSB_Z = 980;
			break;
		case lsm303::MagGain::G1_9:
			Gauss_LSB_XY = 855;
			Gauss_LSB_Z = 760;
			break;
		case lsm303::MagGain::G2_5:
			Gauss_LSB_XY = 670;
			Gauss_LSB_Z = 600;
			break;
		case lsm303::MagGain::G4_0:
			Gauss_LSB_XY = 450;
			Gauss_LSB_Z = 400;
			break;
		case lsm303::MagGain::G4_7:
			Gauss_LSB_XY = 400;
			Gauss_LSB_Z = 355;
			break;
		case lsm303::MagGain::G5_6:
			Gauss_LSB_XY = 330;
			Gauss_LSB_Z = 295;
			break;
		case lsm303::MagGain::G8_1:
			Gauss_LSB_XY = 230;
			Gauss_LSB_Z = 205;
			break;
	}
}

void imu::setMagRate(const lsm303::MagRate& rate){
	i2c_utils::writeCommand(lsm303::AddressMag, (byte)lsm303::MagRegisters::CRA_REG_M, ((byte)rate & 0x07) << 2);
}

void imu::setGyroRange(const l3gd20::gyroRange& range){
	gRange = range;
	switch (gRange) {
		case l3gd20::gyroRange::R250DPS:
			i2c_utils::writeCommand(l3gd20::AdressGyro, (byte)l3gd20::gyroRegisters::CTRL_REG4, 0x00);
			break;
		case l3gd20::gyroRange::R500DPS:
			i2c_utils::writeCommand(l3gd20::AdressGyro, (byte)l3gd20::gyroRegisters::CTRL_REG4, 0x10);
			break;
		case l3gd20::gyroRange::R2000DPS:
			i2c_utils::writeCommand(l3gd20::AdressGyro, (byte)l3gd20::gyroRegisters::CTRL_REG4, 0x20);
			break;
	}
}
