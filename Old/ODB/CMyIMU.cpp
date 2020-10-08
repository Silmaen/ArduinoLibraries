

#include "CMyIMU.h"
#include <math.h>
#include <limits.h>

CMyIMU::CMyIMU(){
	//accel = new Adafruit_LSM303_Accel_Unified(30301);
	//mag   = new Adafruit_LSM303_Mag_Unified(30302);
	//bmp   = new Adafruit_BMP085_Unified(18001);
	//gyro  = new Adafruit_L3GD20_Unified(20);
	_QNH=1013;
	init=false;
	currentOrient.Tangage=0;
	currentOrient.Roulis=0;
	currentOrient.Cap=0;
	raworient.Tangage=0;
	raworient.Roulis=0;
	raworient.Cap=0;
	gyroorient.Tangage=0;
	gyroorient.Roulis=0;
	gyroorient.Cap=0;
	_l3gd20_autoRangeEnabled = false;
	_l3gd20_range=GYRO_RANGE_250DPS;
	_bmp085Mode=BMP085_MODE_ULTRAHIGHRES;
	_lsm303_magGain=LSM303_MAGGAIN_1_3;
	_lsm303_autoRangeEnabled = false;
	_lsm303Accel_MG_LSB     = 0.001F;   // 1, 2, 4 or 12 mg per lsb
	_lsm303Mag_Gauss_LSB_XY = 1100.0F;  // Varies with gain
	_lsm303Mag_Gauss_LSB_Z  = 980.0F;   // Varies with gain
}

bool CMyIMU::begin(bmp085_mode_t  bmp085_mode,gyroRange_t l3gd20_rng){
	time=millis();
	bool noerror=true;
	Wire.begin();
/*==============================================================================================*/
	/*------------INITIALIZATION DU LSM303--------------------------*/
/*==============================================================================================*/
	// Enable the accelerometer (100Hz)
	writeCommand(LSM303_REGISTER_ACCEL_CTRL_REG1_A, 0x57,(uint8_t)LSM303_ADDRESS_ACCEL);
	writeCommand(LSM303_REGISTER_MAG_MR_REG_M, 0x00,(uint8_t)LSM303_ADDRESS_MAG);
	// Set the gain to a known level
	setMagGain(LSM303_MAGGAIN_1_3);
/*==============================================================================================*/
	/*----------FIN INITIALIZATION DU LSM303------------------------*/
/*==============================================================================================*/

/*==============================================================================================*/
	/*------------INITIALIZATION DU BMP085--------------------------*/
/*==============================================================================================*/
	/* Mode boundary check */
	if ((bmp085_mode > BMP085_MODE_ULTRAHIGHRES) || (bmp085_mode < 0)) {
		bmp085_mode = BMP085_MODE_ULTRAHIGHRES;
	}
	/* Make sure we have the right device */
	uint8_t id;
	read8(BMP085_REGISTER_CHIPID, &id,(uint8_t)BMP085_ADDRESS);
	if(id != 0x55) {
		return false;
	}
	/* Set the mode indicator */
	_bmp085Mode = bmp085_mode;
	/* Coefficients need to be read once */
	readBMP085Coefficients();
/*==============================================================================================*/
	/*----------FIN INITIALIZATION DU BMP085------------------------*/
/*==============================================================================================*/

/*==============================================================================================*/
	/*------------INITIALIZATION DU L3GGD20--------------------------*/
/*==============================================================================================*/
	/* Set the range the an appropriate value */
	if ((l3gd20_rng > GYRO_RANGE_2000DPS) || (l3gd20_rng < 0)) {
		l3gd20_rng = GYRO_RANGE_2000DPS;
	}
	_l3gd20_range = l3gd20_rng;
	 /* Set CTRL_REG1 (0x20)
	   ====================================================================
	   BIT  Symbol    Description                                   Default
	  ---  ------    --------------------------------------------- -------
	  7-6  DR1/0     Output data rate                                   00
	  5-4  BW1/0     Bandwidth selection                                00
	    3  PD        0 = Power-down mode, 1 = normal/sleep mode          0
	    2  ZEN       Z-axis enable (0 = disabled, 1 = enabled)           1
	    1  YEN       Y-axis enable (0 = disabled, 1 = enabled)           1
	    0  XEN       X-axis enable (0 = disabled, 1 = enabled)           1 */

	/* Reset then switch to normal mode and enable all three channels */
	 writeCommand(GYRO_REGISTER_CTRL_REG1, 0x00,(uint8_t)L3GD20_ADDRESS);
	 writeCommand(GYRO_REGISTER_CTRL_REG1, 0x0F,(uint8_t)L3GD20_ADDRESS);
	 /* ------------------------------------------------------------------ */

	 /* Set CTRL_REG2 (0x21)
	  ====================================================================
	  BIT  Symbol    Description                                   Default
	  ---  ------    --------------------------------------------- -------
	  5-4  HPM1/0    High-pass filter mode selection                    00
	  3-0  HPCF3..0  High-pass filter cutoff frequency selection      0000 */

	 /* Nothing to do ... keep default values */
	 /* ------------------------------------------------------------------ */

	 /* Set CTRL_REG3 (0x22)
	  ====================================================================
	  BIT  Symbol    Description                                   Default
	   ---  ------    --------------------------------------------- -------
	     7  I1_Int1   Interrupt enable on INT1 (0=disable,1=enable)       0
	     6  I1_Boot   Boot status on INT1 (0=disable,1=enable)            0
	     5  H-Lactive Interrupt active config on INT1 (0=high,1=low)      0
	     4  PP_OD     Push-Pull/Open-Drain (0=PP, 1=OD)                   0
	     3  I2_DRDY   Data ready on DRDY/INT2 (0=disable,1=enable)        0
	     2  I2_WTM    FIFO wtrmrk int on DRDY/INT2 (0=dsbl,1=enbl)        0
	     1  I2_ORun   FIFO overrun int on DRDY/INT2 (0=dsbl,1=enbl)       0
	     0  I2_Empty  FIFI empty int on DRDY/INT2 (0=dsbl,1=enbl)         0 */

	 /* Nothing to do ... keep default values */
	 /* ------------------------------------------------------------------ */

	 /* Set CTRL_REG4 (0x23)
	   ====================================================================
	   BIT  Symbol    Description                                   Default
	   ---  ------    --------------------------------------------- -------
	     7  BDU       Block Data Update (0=continuous, 1=LSB/MSB)         0
	     6  BLE       Big/Little-Endian (0=Data LSB, 1=Data MSB)          0
	   5-4  FS1/0     Full scale selection                               00
	                                  00 = 250 dps
	                                  01 = 500 dps
	                                  10 = 2000 dps
	                                  11 = 2000 dps
	     0  SIM       SPI Mode (0=4-wire, 1=3-wire)                       0 */

	 /* Adjust resolution if requested */
	switch(_l3gd20_range) {
	case GYRO_RANGE_250DPS:
		writeCommand(GYRO_REGISTER_CTRL_REG4, 0x00,(uint8_t)L3GD20_ADDRESS);
		break;
	case GYRO_RANGE_500DPS:
		writeCommand(GYRO_REGISTER_CTRL_REG4, 0x10,(uint8_t)L3GD20_ADDRESS);
		break;
	case GYRO_RANGE_2000DPS:
		writeCommand(GYRO_REGISTER_CTRL_REG4, 0x20,(uint8_t)L3GD20_ADDRESS);
		break;
	}
	/* ------------------------------------------------------------------ */

	/* Set CTRL_REG5 (0x24)
	   ====================================================================
	   BIT  Symbol    Description                                   Default
	   ---  ------    --------------------------------------------- -------
	     7  BOOT      Reboot memory content (0=normal, 1=reboot)          0
	     6  FIFO_EN   FIFO enable (0=FIFO disable, 1=enable)              0
	     4  HPen      High-pass filter enable (0=disable,1=enable)        0
	   3-2  INT1_SEL  INT1 Selection config                              00
	   1-0  OUT_SEL   Out selection config                               00 */

	/* Nothing to do ... keep default values */
/*==============================================================================================*/
	/*----------FIN INITIALIZATION DU L3GGD20------------------------*/
/*==============================================================================================*/
	readSensors(0);
// initialization of the filter
	updatePitchRoll(newData.accX,newData.accY,newData.accZ);
	updateYaw(newData.magX,newData.magY,newData.magZ);
	kalmanX.setAngle(raworient.Roulis); // First set roll starting angle
	gyroorient.Roulis = raworient.Roulis;
	kalmanY.setAngle(raworient.Tangage); // Then pitch
	gyroorient.Tangage = raworient.Tangage;

	kalmanZ.setAngle(raworient.Cap); // And finally yaw
	gyroorient.Cap = raworient.Cap;

	init=true;
	return noerror;
}

void CMyIMU::calcAltitude(){
	/* Hyposometric formula:                      *
	 *                                            *
	 *     ((P0/P)^(1/5.257) - 1) * (T + 273.15)  *
	 * h = -------------------------------------  *
	 *                   0.0065                   *
	 *                                            *
	 * where: h   = height (in meters)            *
	 *        P0  = sea-level pressure (in hPa)   *
	 *        P   = atmospheric pressure (in hPa) *
	 *        T   = temperature (in °C)           */
	newData.altitude=((pow((_QNH/newData.pression), 0.190223) - 1.0)
					* (newData.temperature + 273.15)) / 0.0065;
}

void CMyIMU::alttoQNH(double altitude){
	/* Hyposometric formula:                                 *
	 *                                                       *
	 * P0=((((h*0.0065)/(temp + 273.15))+1)^(^/0.190223))*P  *
	 *                                                       *
	 * where: h   = height (in meters)                       *
	 *        P0  = sea-level pressure (in hPa)              *
	 *        P   = atmospheric pressure (in hPa)            *
	 *        T   = temperature (in °C)                      */
	_QNH=pow((((altitude*0.0065)/(newData.temperature + 273.15))+1), (1.0/0.190223))*newData.pression;
}

void CMyIMU::adjustQNH(double QNH){
	_QNH=QNH;
	calcAltitude();
}

void CMyIMU::readSensors(double dt){
	oldData=newData;

	/**
	 * Rotation des axes!
	 *     /\                            / X \   /  y \
	 *    /! \   l'apareil étant tourné: | Y | = | -x |
	 *   *----*                          \ Z /   \  z /
	 */

	/* Read the accelerometer */
	double ax,ay,az;
	getLSM303AccData(&ax,&ay,&az);
	newData.accX=(newData.accX+ay)/2.0;
	newData.accY=(newData.accY-ax)/2.0;
	newData.accZ=(newData.accZ+az)/2.0;
	/* read the magnetometer */
	double mx,my,mz;
	getLSM303MagData(&mx,&my,&mz);
	newData.magX=(newData.magX+my)/2.0;
	newData.magY=(newData.magY-mx)/2.0;
	newData.magZ=(newData.magZ+mz)/2.0;
	/* Calculate the altitude using the barometric pressure sensor */
	double temp,press;
	getBMP085Data(&press,&temp);
	newData.temperature=(temp+newData.temperature)/2.0;
	newData.pression=(press+newData.pression)/2.0;
	calcAltitude();
	/* Calcul gyroscope */
	// raw gyro en radians par secondes
	double gx,gy,gz;
	getL3GD20Data(&gx,&gy,&gz);
	newData.gyroX=(newData.gyroX+gy)/2.0;
	newData.gyroY=(newData.gyroY-gx)/2.0;
	newData.gyroZ=(newData.gyroZ+gz)/2.0;
	newData.timestamp=millis();
	init=false;
	updatePitchRoll(newData.accX,newData.accY,newData.accZ);
	double gyroXrate = newData.gyroX;
	double gyroYrate = newData.gyroY;
#ifdef RESTRICT_PITCH
	// This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
	if ((raworient.Roulis < -90 && currentOrient.Roulis > 90) || (raworient.Roulis > 90 && currentOrient.Roulis < -90)) {
		kalmanX.setAngle(raworient.Roulis);
		currentOrient.Roulis = raworient.Roulis;
		gyroorient.Roulis = raworient.Roulis;
	} else {
		currentOrient.Roulis = kalmanX.getAngle(raworient.Roulis, gyroXrate, dt); // Calculate the angle using a Kalman filter
	}

	if (abs(currentOrient.Roulis) > 90)
		gyroYrate = -gyroYrate; // Invert rate, so it fits the restricted accelerometer reading
	currentOrient.Tangage = kalmanY.getAngle(raworient.Tangage, gyroYrate, dt);
#else
	// This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
	if ((raworient.Tangage < -90 && currentOrient.Tangage > 90) || (raworient.Tangage > 90 && currentOrient.Tangage < -90)) {
		kalmanY.setAngle(raworient.Tangage);
		currentOrient.Tangage = raworient.Tangage;
		gyroorient.Tangage = raworient.Tangage;
	} else {
		currentOrient.Tangage = kalmanY.getAngle(raworient.Tangage, gyroYrate, dt); // Calculate the angle using a Kalman filter
	}

	if (abs(currentOrient.Tangage) > 90)
		gyroXrate = -gyroXrate; // Invert rate, so it fits the restricted accelerometer reading
	currentOrient.Roulis = kalmanX.getAngle(raworient.Roulis, gyroXrate, dt); // Calculate the angle using a Kalman filter
#endif
	/* Yaw estimation */
	updateYaw(newData.magX,newData.magY,newData.magZ);
	//currentOrient.Cap=raworient.Cap;
	double gyroZrate = newData.gyroZ / 131.0; // Convert to deg/s
	// This fixes the transition problem when the yaw angle jumps between 0 and 360 degrees
	if ((raworient.Cap < 90 && currentOrient.Cap > 270) || (raworient.Cap > 270 && currentOrient.Cap < 90)) {
		kalmanZ.setAngle(raworient.Cap);
		currentOrient.Cap = raworient.Cap;
		gyroorient.Cap = raworient.Cap;
	} else
		currentOrient.Cap = kalmanZ.getAngle(raworient.Cap, gyroZrate, dt); // Calculate the angle using a Kalman filter

	if (currentOrient.Cap<0)
		currentOrient.Cap+=360;
	/* Estimate angles using gyro only */
	gyroorient.Roulis += gyroXrate * dt; // Calculate gyro angle without any filter
	gyroorient.Tangage += gyroYrate * dt;
	gyroorient.Cap += gyroZrate * dt;
	//gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate from the Kalman filter
	//gyroYangle += kalmanY.getRate() * dt;
	//gyroZangle += kalmanZ.getRate() * dt;

	// Reset the gyro angles when they has drifted too much
	if (gyroorient.Roulis < -180 || gyroorient.Roulis > 180)
		gyroorient.Roulis = currentOrient.Roulis;
	if (gyroorient.Tangage < -180 || gyroorient.Tangage > 180)
		gyroorient.Tangage = currentOrient.Tangage;
	if (gyroorient.Cap < -180 || gyroorient.Cap > 180)
		gyroorient.Cap = currentOrient.Cap;

}

double CMyIMU::getGyro(uint8_t composante,bool oriented) {
	switch (composante) {
	case 1:
		if (oriented) return rotated.gyroX;
		return newData.gyroX;
		break;
	case 2:
		if (oriented) return rotated.gyroY;
		return newData.gyroY;
		break;
	case 3:
		if (oriented) return rotated.gyroZ;
		return newData.gyroZ;
		break;
	default:
		return 0;
	}
}

double CMyIMU::getAccel(uint8_t composante,bool oriented) {
	switch (composante) {
	case 1:
		if (oriented) return rotated.accX;
		return newData.accX;
		break;
	case 2:
		if (oriented) return rotated.accY;
		return newData.accY;
		break;
	case 3:
		if (oriented) return rotated.accZ;
		return newData.accZ;
		break;
	default:
		return 0;
	}
}

double CMyIMU::getMagnetic(uint8_t composante,bool oriented) {
	switch (composante) {
	case 1:
		if (oriented) return rotated.magX;
		return newData.magX;
		break;
	case 2:
		if (oriented) return rotated.magY;
		return newData.magY;
		break;
	case 3:
		if (oriented) return rotated.magZ;
		return newData.magZ;
		break;
	default:
		return 0;
	}
}


void CMyIMU::updatePitchRoll(double acX,double acY,double acZ) {
	double norm=sqrt(acX*acX+acY*acY+acZ*acZ);
	acX/=norm;
	acY/=norm;
	acZ/=norm;
#ifdef RESTRICT_PITCH // Eq. 25 and 26
	raworient.Roulis = -atan2(acY, acZ) * RAD_TO_DEG;
	raworient.Tangage = -atan(-acX / sqrt(acY * acY + acZ * acZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
	raworient.Roulis = atan(acY / sqrt(acX * acX + acZ * acZ)) * RAD_TO_DEG;
	raworient.Tangage = atan2(-acX, acZ) * RAD_TO_DEG;
#endif
}

void CMyIMU::updateYaw(double magX,double magY,double magZ) { 
	double rollAngle = currentOrient.Roulis * DEG_TO_RAD;
	double pitchAngle = currentOrient.Tangage * DEG_TO_RAD;
	double Bfy = magZ * sin(rollAngle) + magY * cos(rollAngle);
	double Bfx = magX * cos(pitchAngle) + magY * sin(pitchAngle) * sin(rollAngle) - magZ * sin(pitchAngle) * cos(rollAngle);
	raworient.Cap = atan2(Bfy, Bfx) * RAD_TO_DEG;
	if (raworient.Cap<0) raworient.Cap+=360;
}
/* --------------------------------------------------------------
 *   LSM303 FUNCTIONS
 * --------------------------------------------------------------*/

void CMyIMU::getLSM303AccData(double *ax,double *ay,double *az){
	// Read the accelerometer
	Wire.beginTransmission((byte)LSM303_ADDRESS_ACCEL);
#if ARDUINO >= 100
	Wire.write(LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80);
#else
	Wire.send(LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80);
#endif
	Wire.endTransmission();
	Wire.requestFrom((byte)LSM303_ADDRESS_ACCEL, (byte)6);
	// Wait around until enough data is available
	while (Wire.available() < 6);
#if ARDUINO >= 100
	uint8_t xlo = Wire.read();
	uint8_t xhi = Wire.read();
	uint8_t ylo = Wire.read();
	uint8_t yhi = Wire.read();
	uint8_t zlo = Wire.read();
	uint8_t zhi = Wire.read();
#else
	uint8_t xlo = Wire.receive();
	uint8_t xhi = Wire.receive();
	uint8_t ylo = Wire.receive();
	uint8_t yhi = Wire.receive();
	uint8_t zlo = Wire.receive();
	uint8_t zhi = Wire.receive();
#endif
	// Shift values to create properly formed integer (low byte first)
	*ax = (int16_t)(xlo | (xhi << 8)) >> 4;
	*ay = (int16_t)(ylo | (yhi << 8)) >> 4;
	*az = (int16_t)(zlo | (zhi << 8)) >> 4;
	*ax *= _lsm303Accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
	*ay *= _lsm303Accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
	*az *= _lsm303Accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
}

void CMyIMU::getLSM303MagData(double *mx,double *my,double *mz){
	bool readingValid = false;
	while(!readingValid) {
		/* Read new data */
		// Read the magnetometer
		Wire.beginTransmission((byte)LSM303_ADDRESS_MAG);
#if ARDUINO >= 100
		Wire.write(LSM303_REGISTER_MAG_OUT_X_H_M);
#else
		Wire.send(LSM303_REGISTER_MAG_OUT_X_H_M);
#endif
		Wire.endTransmission();
		Wire.requestFrom((byte)LSM303_ADDRESS_MAG, (byte)6);
		// Wait around until enough data is available
		while (Wire.available() < 6);
		// Note high before low (different than accel)
#if ARDUINO >= 100
		uint8_t xhi = Wire.read();
		uint8_t xlo = Wire.read();
		uint8_t zhi = Wire.read();
		uint8_t zlo = Wire.read();
		uint8_t yhi = Wire.read();
		uint8_t ylo = Wire.read();
#else
		uint8_t xhi = Wire.receive();
		uint8_t xlo = Wire.receive();
		uint8_t zhi = Wire.receive();
		uint8_t zlo = Wire.receive();
		uint8_t yhi = Wire.receive();
		uint8_t ylo = Wire.receive();
#endif
		// Shift values to create properly formed integer (low byte first)
		*mx = (int16_t)(xlo | ((int16_t)xhi << 8));
		*my = (int16_t)(ylo | ((int16_t)yhi << 8));
		*mz = (int16_t)(zlo | ((int16_t)zhi << 8));
			// ToDo: Calculate orientation
		/* Make sure the sensor isn't saturating if auto-ranging is enabled */
		if (!_lsm303_autoRangeEnabled) {
			readingValid = true;
		} else {
			Serial.print(*mx); Serial.print(" ");
			Serial.print(*my); Serial.print(" ");
			Serial.print(*mz); Serial.println(" ");
			/* Check if the sensor is saturating or not */
			if ( (*mx >= 4090) | (*mx <= -4090) |
					(*my >= 4090) | (*my <= -4090) |
					(*mz >= 4090) | (*mz <= -4090) ) {
				/* Saturating .... increase the range if we can */
				switch(_lsm303_magGain) {
				case LSM303_MAGGAIN_5_6:
					setMagGain(LSM303_MAGGAIN_8_1);
					readingValid = false;
					Serial.println("Changing range to +/- 8.1");
					break;
				case LSM303_MAGGAIN_4_7:
					setMagGain(LSM303_MAGGAIN_5_6);
					readingValid = false;
					Serial.println("Changing range to +/- 5.6");
					break;
				case LSM303_MAGGAIN_4_0:
					setMagGain(LSM303_MAGGAIN_4_7);
					readingValid = false;
					Serial.println("Changing range to +/- 4.7");
					break;
				case LSM303_MAGGAIN_2_5:
					setMagGain(LSM303_MAGGAIN_4_0);
					readingValid = false;
					Serial.println("Changing range to +/- 4.0");
					break;
				case LSM303_MAGGAIN_1_9:
					setMagGain(LSM303_MAGGAIN_2_5);
					readingValid = false;
					Serial.println("Changing range to +/- 2.5");
					break;
				case LSM303_MAGGAIN_1_3:
					setMagGain(LSM303_MAGGAIN_1_9);
					readingValid = false;
					Serial.println("Changing range to +/- 1.9");
					break;
				default:
					readingValid = true;
					break;
				}
			} else {
				/* All values are withing range */
				readingValid = true;
			}
		}
	}
	*mx /= _lsm303Mag_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
	*my /= _lsm303Mag_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
	*mz /=  _lsm303Mag_Gauss_LSB_Z * SENSORS_GAUSS_TO_MICROTESLA;
}

void CMyIMU::setMagGain(enum lsm303MagGain gain) {
	writeCommand(LSM303_REGISTER_MAG_CRB_REG_M, (byte)gain,(uint8_t)LSM303_ADDRESS_MAG);
	_lsm303_magGain = gain;
	switch(gain) {
	case LSM303_MAGGAIN_1_3:
		_lsm303Mag_Gauss_LSB_XY = 1100;
		_lsm303Mag_Gauss_LSB_Z  = 980;
		break;
	case LSM303_MAGGAIN_1_9:
		_lsm303Mag_Gauss_LSB_XY = 855;
		_lsm303Mag_Gauss_LSB_Z  = 760;
		break;
	case LSM303_MAGGAIN_2_5:
		_lsm303Mag_Gauss_LSB_XY = 670;
		_lsm303Mag_Gauss_LSB_Z  = 600;
		break;
	case LSM303_MAGGAIN_4_0:
		_lsm303Mag_Gauss_LSB_XY = 450;
		_lsm303Mag_Gauss_LSB_Z  = 400;
		break;
	case LSM303_MAGGAIN_4_7:
		_lsm303Mag_Gauss_LSB_XY = 400;
		_lsm303Mag_Gauss_LSB_Z  = 255;
		break;
	case LSM303_MAGGAIN_5_6:
		_lsm303Mag_Gauss_LSB_XY = 330;
		_lsm303Mag_Gauss_LSB_Z  = 295;
		break;
	case LSM303_MAGGAIN_8_1:
		_lsm303Mag_Gauss_LSB_XY = 230;
		_lsm303Mag_Gauss_LSB_Z  = 205;
		break;
	}
}

/* --------------------------------------------------------------
 *  BMP085 FUNCTIONS
 * --------------------------------------------------------------*/
void CMyIMU::readBMP085Coefficients(void) {
#if BMP085_USE_DATASHEET_VALS
	_bmp085_coeffs.ac1 = 408;
	_bmp085_coeffs.ac2 = -72;
	_bmp085_coeffs.ac3 = -14383;
	_bmp085_coeffs.ac4 = 32741;
	_bmp085_coeffs.ac5 = 32757;
	_bmp085_coeffs.ac6 = 23153;
	_bmp085_coeffs.b1  = 6190;
	_bmp085_coeffs.b2  = 4;
	_bmp085_coeffs.mb  = -32768;
	_bmp085_coeffs.mc  = -8711;
	_bmp085_coeffs.md  = 2868;
	_bmp085Mode        = 0;
#else
	readS16(BMP085_REGISTER_CAL_AC1, &_bmp085_coeffs.ac1,(uint8_t)BMP085_ADDRESS);
	readS16(BMP085_REGISTER_CAL_AC2, &_bmp085_coeffs.ac2,(uint8_t)BMP085_ADDRESS);
	readS16(BMP085_REGISTER_CAL_AC3, &_bmp085_coeffs.ac3,(uint8_t)BMP085_ADDRESS);
	read16(BMP085_REGISTER_CAL_AC4, &_bmp085_coeffs.ac4,(uint8_t)BMP085_ADDRESS);
	read16(BMP085_REGISTER_CAL_AC5, &_bmp085_coeffs.ac5,(uint8_t)BMP085_ADDRESS);
	read16(BMP085_REGISTER_CAL_AC6, &_bmp085_coeffs.ac6,(uint8_t)BMP085_ADDRESS);
	readS16(BMP085_REGISTER_CAL_B1, &_bmp085_coeffs.b1,(uint8_t)BMP085_ADDRESS);
	readS16(BMP085_REGISTER_CAL_B2, &_bmp085_coeffs.b2,(uint8_t)BMP085_ADDRESS);
	readS16(BMP085_REGISTER_CAL_MB, &_bmp085_coeffs.mb,(uint8_t)BMP085_ADDRESS);
	readS16(BMP085_REGISTER_CAL_MC, &_bmp085_coeffs.mc,(uint8_t)BMP085_ADDRESS);
	readS16(BMP085_REGISTER_CAL_MD, &_bmp085_coeffs.md,(uint8_t)BMP085_ADDRESS);
#endif
}

void CMyIMU::getBMP085Data(double *pressure,double *temperature){
	int32_t  ut = 0, up = 0, compp = 0;
	int32_t  x1, x2, b5, b6, x3, b3;
	float p, t;
	uint32_t b4, b7;
	uint8_t  p8;
	uint16_t p16;
	int32_t  p32;

	/* Get the raw pressure and temperature values */
	//---------------------------------------
#if BMP085_USE_DATASHEET_VALS
	ut = 27898;
	up = 23843;
#else
	writeCommand(BMP085_REGISTER_CONTROL, BMP085_REGISTER_READTEMPCMD,(uint8_t)BMP085_ADDRESS);
	delay(5);
	read16(BMP085_REGISTER_TEMPDATA, &p16,(uint8_t)BMP085_ADDRESS);
	ut = p16;
	delay(5);
	writeCommand(BMP085_REGISTER_CONTROL, BMP085_REGISTER_READPRESSURECMD + (_bmp085Mode << 6),(uint8_t)BMP085_ADDRESS);
	switch(_bmp085Mode) {
	case BMP085_MODE_ULTRALOWPOWER:
		delay(5);
		break;
	case BMP085_MODE_STANDARD:
		delay(8);
		break;
	case BMP085_MODE_HIGHRES:
		delay(14);
		break;
	case BMP085_MODE_ULTRAHIGHRES:
	default:
		delay(26);
		break;
	}
#endif
	read16(BMP085_REGISTER_PRESSUREDATA, &p16,(uint8_t)BMP085_ADDRESS);
	p32 = (uint32_t)p16 << 8;
	read8(BMP085_REGISTER_PRESSUREDATA+2, &p8,(uint8_t)BMP085_ADDRESS);
	p32 += p8;
	p32 >>= (8 - _bmp085Mode);

	up = p32;

	////////////////////////////////

	/* Temperature compensation */
	x1 = (ut - (int32_t)(_bmp085_coeffs.ac6))*((int32_t)(_bmp085_coeffs.ac5))/pow(2,15);
	x2 = ((int32_t)(_bmp085_coeffs.mc*pow(2,11)))/(x1+(int32_t)(_bmp085_coeffs.md));
	b5 = x1 + x2;
	// step 1
	t = (b5+8)/pow(2,4);
	t /= 10;

	/* Pressure compensation */
	b6 = b5 - 4000;
	x1 = (_bmp085_coeffs.b2 * ((b6 * b6) >> 12)) >> 11;
	x2 = (_bmp085_coeffs.ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((((int32_t) _bmp085_coeffs.ac1) * 4 + x3) << _bmp085Mode) + 2) >> 2;
	x1 = (_bmp085_coeffs.ac3 * b6) >> 13;
	x2 = (_bmp085_coeffs.b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (_bmp085_coeffs.ac4 * (uint32_t) (x3 + 32768)) >> 15;
	b7 = ((uint32_t) (up - b3) * (50000 >> _bmp085Mode));

	if (b7 < 0x80000000) {
		p32 = (b7 << 1) / b4;
	} else {
		p32 = (b7 / b4) << 1;
	}

	x1 = (p32 >> 8) * (p32 >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p32) >> 16;
	p = p32 + ((x1 + x2 + 3791) >> 4);

	/* Assign compensated pressure value */
	*pressure = p;
	*temperature = t;
	if (millis()-time>10000){
		time=millis();
		Serial.print("Raw Temp: ");Serial.print(ut);Serial.print(" Calc Temp: ");Serial.println(t);
		Serial.print("Raw pres: ");Serial.print(up);Serial.print(" Calc pres: ");Serial.println(p);
		Serial.print("_bmp085_coeffs ac1: ");Serial.print(_bmp085_coeffs.ac1);Serial.print(" ac2 ");Serial.print(_bmp085_coeffs.ac2);
		Serial.print(" ac3 ");Serial.print(_bmp085_coeffs.ac3);Serial.print(" ac4 ");Serial.print(_bmp085_coeffs.ac4);
		Serial.print(" ac5 ");Serial.print(_bmp085_coeffs.ac5);Serial.print(" ac6 ");Serial.print(_bmp085_coeffs.ac6);
		Serial.print(" b1 ");Serial.print(_bmp085_coeffs.b1);Serial.print(" b2 ");Serial.print(_bmp085_coeffs.b2);
		Serial.print(" mb ");Serial.print(_bmp085_coeffs.mb);Serial.print(" mc ");Serial.print(_bmp085_coeffs.mc);
		Serial.print(" md ");Serial.println(_bmp085_coeffs.md);
	}

}

/* --------------------------------------------------------------
 *   L3GD20 Functions
 * --------------------------------------------------------------*/
void CMyIMU::getL3GD20Data(double *gx,double *gy,double *gz){
	bool readingValid = false;
	while(!readingValid) {
		/* Read 6 bytes from the sensor */
		Wire.beginTransmission((byte)L3GD20_ADDRESS);
	#if ARDUINO >= 100
		Wire.write(GYRO_REGISTER_OUT_X_L | 0x80);
	#else
		Wire.send(GYRO_REGISTER_OUT_X_L | 0x80);
	#endif
		Wire.endTransmission();
		Wire.requestFrom((byte)L3GD20_ADDRESS, (byte)6);
		/* Wait around until enough data is available */
		while (Wire.available() < 6);
	#if ARDUINO >= 100
		uint8_t xlo = Wire.read();
		uint8_t xhi = Wire.read();
		uint8_t ylo = Wire.read();
		uint8_t yhi = Wire.read();
		uint8_t zlo = Wire.read();
		uint8_t zhi = Wire.read();
	#else
		uint8_t xlo = Wire.receive();
		uint8_t xhi = Wire.receive();
		uint8_t ylo = Wire.receive();
		uint8_t yhi = Wire.receive();
		uint8_t zlo = Wire.receive();
		uint8_t zhi = Wire.receive();
	#endif
		/* Shift values to create properly formed integer (low byte first) */
		*gx = (int16_t)(xlo | (xhi << 8));
		*gy = (int16_t)(ylo | (yhi << 8));
		*gz = (int16_t)(zlo | (zhi << 8));
		/* Make sure the sensor isn't saturating if auto-ranging is enabled */
		if (!_l3gd20_autoRangeEnabled) {
			readingValid = true;
		} else {
			/* Check if the sensor is saturating or not */
			if ( (*gx >= 32760) | (*gx <= -32760) |
					(*gy >= 32760) | (*gy <= -32760) |
					(*gz >= 32760) | (*gz <= -32760) ) {
				/* Saturating .... increase the range if we can */
				switch(_l3gd20_range) {
				case GYRO_RANGE_500DPS:
					/* Push the range up to 2000dps */
					_l3gd20_range = GYRO_RANGE_2000DPS;
					writeCommand(GYRO_REGISTER_CTRL_REG1, 0x00,(uint8_t)L3GD20_ADDRESS);
					writeCommand(GYRO_REGISTER_CTRL_REG1, 0x0F,(uint8_t)L3GD20_ADDRESS);
					writeCommand(GYRO_REGISTER_CTRL_REG4, 0x20,(uint8_t)L3GD20_ADDRESS);
					writeCommand(GYRO_REGISTER_CTRL_REG5, 0x80,(uint8_t)L3GD20_ADDRESS);
					readingValid = false;
					// Serial.println("Changing range to 2000DPS");
					break;
				case GYRO_RANGE_250DPS:
					/* Push the range up to 500dps */
					_l3gd20_range = GYRO_RANGE_500DPS;
					writeCommand(GYRO_REGISTER_CTRL_REG1, 0x00,(uint8_t)L3GD20_ADDRESS);
					writeCommand(GYRO_REGISTER_CTRL_REG1, 0x0F,(uint8_t)L3GD20_ADDRESS);
					writeCommand(GYRO_REGISTER_CTRL_REG4, 0x10,(uint8_t)L3GD20_ADDRESS);
					writeCommand(GYRO_REGISTER_CTRL_REG5, 0x80,(uint8_t)L3GD20_ADDRESS);
					readingValid = false;
					// Serial.println("Changing range to 500DPS");
					break;
				default:
					readingValid = true;
					break;
				}
			} else {
				/* All values are withing range */
				readingValid = true;
			}
		}
	}
	/* Compensate values depending on the resolution */
	switch(_l3gd20_range) {
	case GYRO_RANGE_250DPS:
		*gx *= GYRO_SENSITIVITY_250DPS;
		*gy *= GYRO_SENSITIVITY_250DPS;
		*gz *= GYRO_SENSITIVITY_250DPS;
		break;
	case GYRO_RANGE_500DPS:
		*gx *= GYRO_SENSITIVITY_500DPS;
		*gy *= GYRO_SENSITIVITY_500DPS;
		*gz *= GYRO_SENSITIVITY_500DPS;
		break;
	case GYRO_RANGE_2000DPS:
		*gx *= GYRO_SENSITIVITY_2000DPS;
		*gy *= GYRO_SENSITIVITY_2000DPS;
		*gz *= GYRO_SENSITIVITY_2000DPS;
		break;
	}
	/* Convert values to rad/s */
	*gx *= SENSORS_DPS_TO_RADS;
	*gy *= SENSORS_DPS_TO_RADS;
	*gz *= SENSORS_DPS_TO_RADS;
}

/* --------------------------------------------------------------
 *   I2C functions
 * --------------------------------------------------------------*/
void CMyIMU::read8(byte reg, uint8_t *value, uint8_t address) {
	Wire.beginTransmission(address);
#if ARDUINO >= 100
	Wire.write((uint8_t)reg);
#else
	Wire.send(reg);
#endif
	Wire.endTransmission();
	Wire.requestFrom(address, (byte)1);
#if ARDUINO >= 100
	*value = Wire.read();
#else
	*value = Wire.receive();
#endif
	Wire.endTransmission();
}

void CMyIMU::read16(byte reg, uint16_t *value, uint8_t address) {
	Wire.beginTransmission(address);
#if ARDUINO >= 100
	Wire.write((uint8_t)reg);
#else
	Wire.send(reg);
#endif
	Wire.endTransmission();
	Wire.requestFrom(address, (byte)2);
#if ARDUINO >= 100
	*value = (Wire.read() << 8) | Wire.read();
#else
	*value = (Wire.receive() << 8) | Wire.receive();
#endif
	Wire.endTransmission();
}

void CMyIMU::readS16(byte reg, int16_t *value, uint8_t address) {
	uint16_t i;
	read16(reg, &i,address);
	*value = (int16_t)i;
}

void CMyIMU::writeCommand(byte reg, byte value, uint8_t address) {
	Wire.beginTransmission(address);
#if ARDUINO >= 100
	Wire.write((uint8_t)reg);
	Wire.write((uint8_t)value);
#else
	Wire.send(reg);
	Wire.send(value);
#endif
	Wire.endTransmission();
}

