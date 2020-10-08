/***************************************************************
 * 
 *  mon système de guidage basé sur un IMU 10 axes
 *
 *  l'axe Z est le vecteur vertical pointant vers le haut.
 *  L'axe Y est l'axe horizontal pointant vers l'avant.
 *  L'axe X est donc le vecteur horizontal pointant vers la droite
 *   Ce qui forme un trièdre direct.
 *
 *  Le roulis est donc la rotation autour de Y  (positif lors d'une gite tribord)
 *  Le tangage est donc la rotation autour de X (positif en cabré)
 *  Le Lacet est donc la rotation autour de Z (positif lors d'un virage sur babord)
 *    Le cap est l'équivalent du lacet mais donné en degrés de 0 - 360 où 0=360= Nord!
 *
 ***************************************************************/



#ifndef CMYIMU_H_
#define CMYIMU_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

//#include "Adafruit_Sensor.h"
//#include "Adafruit_LSM303_U.h"
//#include "Adafruit_BMP085_U.h"
//#include "Adafruit_L3GD20_U.h"
#include "kalman.h"
#include <Wire.h>

#define RESTRICT_PITCH

struct Cardan3D {
	double Tangage;
	double Roulis;
	double Cap;
};

class SensorData{
public:
	SensorData(){
		timestamp=0;
		gyroX=0;gyroY=0;gyroZ=0;
		accX=0;accY=0;accZ=0;
		magX=0;magY=0;magZ=0;
		altitude=0;pression=0;temperature=0;
	}
	SensorData(const SensorData& a){
		timestamp=a.timestamp;
		gyroX=a.gyroX;gyroY=a.gyroY;gyroZ=a.gyroZ;
		accX=a.accX;accY=a.accY;accZ=a.accZ;
		magX=a.magX;magY=a.magY;magZ=a.magZ;
		altitude=a.altitude;pression=a.pression;temperature=a.temperature;
	}
	SensorData& operator=(SensorData& a){
		timestamp=a.timestamp;
		gyroX=a.gyroX;gyroY=a.gyroY;gyroZ=a.gyroZ;
		accX=a.accX;accY=a.accY;accZ=a.accZ;
		magX=a.magX;magY=a.magY;magZ=a.magZ;
		temperature=a.temperature;
		pression=a.pression;
		altitude=a.altitude;
		return *this;
	}
	uint32_t timestamp; // time stamp in milliseconds
	double gyroX,gyroY,gyroZ;      // gyrosope in deg/s
	double accX,accY,accZ;     // acceleration in m/s/s
	double magX,magY,magZ;       // magnetic in µT
	double temperature;  // temperature in °C
	double pression;     // pressure in hPa
	double altitude;     // altitude in m
};

#define SENSORS_GRAVITY_EARTH             (9.80665F)              /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_MOON              (1.6F)                  /**< The moon's gravity in m/s^2 */
#define SENSORS_GRAVITY_SUN               (275.0F)                /**< The sun's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD          (SENSORS_GRAVITY_EARTH)
#define SENSORS_MAGFIELD_EARTH_MAX        (60.0F)                 /**< Maximum magnetic field on Earth's surface */
#define SENSORS_MAGFIELD_EARTH_MIN        (30.0F)                 /**< Minimum magnetic field on Earth's surface */
#define SENSORS_PRESSURE_SEALEVELHPA      (1013.25F)              /**< Average sea level pressure is 1013.25 hPa */
#define SENSORS_DPS_TO_RADS               (0.017453293F)          /**< Degrees/s to rad/s multiplier */
#define SENSORS_GAUSS_TO_MICROTESLA       (100)                   /**< Gauss to micro-Tesla multiplier */

/*=========================================================================
   DATA FOR LSM303      (accelrometre/magnetometre)
---------------------------------------------------------------------------
    I2C ADDRESS/BITS
---------------------------------------------------------------------------*/
#define LSM303_ADDRESS_ACCEL          (0x32 >> 1)         // 0011001x
#define LSM303_ADDRESS_MAG            (0x3C >> 1)         // 0011110x
#define LSM303_ID                     (0b11010100)
/*  REGISTERS
---------------------------------------------------------------------------*/
enum {                                          // DEFAULT    TYPE
	LSM303_REGISTER_ACCEL_CTRL_REG1_A         = 0x20,   // 00000111   rw
	LSM303_REGISTER_ACCEL_CTRL_REG2_A         = 0x21,   // 00000000   rw
	LSM303_REGISTER_ACCEL_CTRL_REG3_A         = 0x22,   // 00000000   rw
	LSM303_REGISTER_ACCEL_CTRL_REG4_A         = 0x23,   // 00000000   rw
	LSM303_REGISTER_ACCEL_CTRL_REG5_A         = 0x24,   // 00000000   rw
	LSM303_REGISTER_ACCEL_CTRL_REG6_A         = 0x25,   // 00000000   rw
	LSM303_REGISTER_ACCEL_REFERENCE_A         = 0x26,   // 00000000   r
	LSM303_REGISTER_ACCEL_STATUS_REG_A        = 0x27,   // 00000000   r
	LSM303_REGISTER_ACCEL_OUT_X_L_A           = 0x28,
	LSM303_REGISTER_ACCEL_OUT_X_H_A           = 0x29,
	LSM303_REGISTER_ACCEL_OUT_Y_L_A           = 0x2A,
	LSM303_REGISTER_ACCEL_OUT_Y_H_A           = 0x2B,
	LSM303_REGISTER_ACCEL_OUT_Z_L_A           = 0x2C,
	LSM303_REGISTER_ACCEL_OUT_Z_H_A           = 0x2D,
	LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A     = 0x2E,
	LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A      = 0x2F,
	LSM303_REGISTER_ACCEL_INT1_CFG_A          = 0x30,
	LSM303_REGISTER_ACCEL_INT1_SOURCE_A       = 0x31,
	LSM303_REGISTER_ACCEL_INT1_THS_A          = 0x32,
	LSM303_REGISTER_ACCEL_INT1_DURATION_A     = 0x33,
	LSM303_REGISTER_ACCEL_INT2_CFG_A          = 0x34,
	LSM303_REGISTER_ACCEL_INT2_SOURCE_A       = 0x35,
	LSM303_REGISTER_ACCEL_INT2_THS_A          = 0x36,
	LSM303_REGISTER_ACCEL_INT2_DURATION_A     = 0x37,
	LSM303_REGISTER_ACCEL_CLICK_CFG_A         = 0x38,
	LSM303_REGISTER_ACCEL_CLICK_SRC_A         = 0x39,
	LSM303_REGISTER_ACCEL_CLICK_THS_A         = 0x3A,
	LSM303_REGISTER_ACCEL_TIME_LIMIT_A        = 0x3B,
	LSM303_REGISTER_ACCEL_TIME_LATENCY_A      = 0x3C,
	LSM303_REGISTER_ACCEL_TIME_WINDOW_A       = 0x3D
};

enum {
	LSM303_REGISTER_MAG_CRA_REG_M             = 0x00,
	LSM303_REGISTER_MAG_CRB_REG_M             = 0x01,
	LSM303_REGISTER_MAG_MR_REG_M              = 0x02,
	LSM303_REGISTER_MAG_OUT_X_H_M             = 0x03,
	LSM303_REGISTER_MAG_OUT_X_L_M             = 0x04,
	LSM303_REGISTER_MAG_OUT_Z_H_M             = 0x05,
	LSM303_REGISTER_MAG_OUT_Z_L_M             = 0x06,
	LSM303_REGISTER_MAG_OUT_Y_H_M             = 0x07,
	LSM303_REGISTER_MAG_OUT_Y_L_M             = 0x08,
	LSM303_REGISTER_MAG_SR_REG_Mg             = 0x09,
	LSM303_REGISTER_MAG_IRA_REG_M             = 0x0A,
	LSM303_REGISTER_MAG_IRB_REG_M             = 0x0B,
	LSM303_REGISTER_MAG_IRC_REG_M             = 0x0C,
	LSM303_REGISTER_MAG_TEMP_OUT_H_M          = 0x31,
	LSM303_REGISTER_MAG_TEMP_OUT_L_M          = 0x32
};
/*  MAGNETOMETER GAIN SETTINGS
---------------------------------------------------------------------------*/
typedef enum lsm303MagGain{
	LSM303_MAGGAIN_1_3                        = 0x20,  // +/- 1.3
	LSM303_MAGGAIN_1_9                        = 0x40,  // +/- 1.9
	LSM303_MAGGAIN_2_5                        = 0x60,  // +/- 2.5
	LSM303_MAGGAIN_4_0                        = 0x80,  // +/- 4.0
	LSM303_MAGGAIN_4_7                        = 0xA0,  // +/- 4.7
	LSM303_MAGGAIN_5_6                        = 0xC0,  // +/- 5.6
	LSM303_MAGGAIN_8_1                        = 0xE0   // +/- 8.1
};
/*  INTERNAL MAGNETOMETER DATA TYPE
---------------------------------------------------------------------------*/
typedef struct lsm303MagData_s {
	float x;
	float y;
	float z;
	float orientation;
} lsm303MagData;
/*  INTERNAL ACCELERATION DATA TYPE
---------------------------------------------------------------------------*/
typedef struct lsm303AccelData_s {
	float x;
	float y;
	float z;
} lsm303AccelData;
/*==END DATA LSM303========================================================*/

/*=========================================================================
   DATA FOR BMP085      (pressure/temperature)
---------------------------------------------------------------------------
    I2C ADDRESS/BITS
---------------------------------------------------------------------------*/
#define BMP085_ADDRESS                (0x77)
/*  REGISTERS
---------------------------------------------------------------------------*/
enum {
	BMP085_REGISTER_CAL_AC1            = 0xAA,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC2            = 0xAC,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC3            = 0xAE,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC4            = 0xB0,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC5            = 0xB2,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC6            = 0xB4,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_B1             = 0xB6,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_B2             = 0xB8,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_MB             = 0xBA,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_MC             = 0xBC,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_MD             = 0xBE,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CHIPID             = 0xD0,
	BMP085_REGISTER_VERSION            = 0xD1,
	BMP085_REGISTER_SOFTRESET          = 0xE0,
	BMP085_REGISTER_CONTROL            = 0xF4,
	BMP085_REGISTER_TEMPDATA           = 0xF6,
	BMP085_REGISTER_PRESSUREDATA       = 0xF6,
	BMP085_REGISTER_READTEMPCMD        = 0x2E,
	BMP085_REGISTER_READPRESSURECMD    = 0x34
};
/*  MODE SETTINGS
---------------------------------------------------------------------------*/
typedef enum {
	BMP085_MODE_ULTRALOWPOWER          = 0,
	BMP085_MODE_STANDARD               = 1,
	BMP085_MODE_HIGHRES                = 2,
	BMP085_MODE_ULTRAHIGHRES           = 3
} bmp085_mode_t;
/*  CALIBRATION DATA
---------------------------------------------------------------------------*/
typedef struct {
	int16_t  ac1;
	int16_t  ac2;
	int16_t  ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t  b1;
	int16_t  b2;
	int16_t  mb;
	int16_t  mc;
	int16_t  md;
} bmp085_calib_data;
/*===END DATA BMP085=======================================================*/

/*=========================================================================
   DATA FOR L3GD20     (gyroscope)
---------------------------------------------------------------------------
    I2C ADDRESS/BITS AND SETTINGS
---------------------------------------------------------------------------*/
    #define L3GD20_ADDRESS           (0x6B)        // 1101011
    #define L3GD20_POLL_TIMEOUT      (100)         // Maximum number of read attempts
    #define L3GD20_ID                (0b11010100)
    #define GYRO_SENSITIVITY_250DPS  (0.00875F)    // Roughly 22/256 for fixed point match
    #define GYRO_SENSITIVITY_500DPS  (0.0175F)     // Roughly 45/256
    #define GYRO_SENSITIVITY_2000DPS (0.070F)      // Roughly 18/256
/*  REGISTERS
---------------------------------------------------------------------------*/
typedef enum {                                // DEFAULT    TYPE
	GYRO_REGISTER_WHO_AM_I            = 0x0F,   // 11010100   r
	GYRO_REGISTER_CTRL_REG1           = 0x20,   // 00000111   rw
	GYRO_REGISTER_CTRL_REG2           = 0x21,   // 00000000   rw
	GYRO_REGISTER_CTRL_REG3           = 0x22,   // 00000000   rw
	GYRO_REGISTER_CTRL_REG4           = 0x23,   // 00000000   rw
	GYRO_REGISTER_CTRL_REG5           = 0x24,   // 00000000   rw
	GYRO_REGISTER_REFERENCE           = 0x25,   // 00000000   rw
	GYRO_REGISTER_OUT_TEMP            = 0x26,   //            r
	GYRO_REGISTER_STATUS_REG          = 0x27,   //            r
	GYRO_REGISTER_OUT_X_L             = 0x28,   //            r
	GYRO_REGISTER_OUT_X_H             = 0x29,   //            r
	GYRO_REGISTER_OUT_Y_L             = 0x2A,   //            r
	GYRO_REGISTER_OUT_Y_H             = 0x2B,   //            r
	GYRO_REGISTER_OUT_Z_L             = 0x2C,   //            r
	GYRO_REGISTER_OUT_Z_H             = 0x2D,   //            r
	GYRO_REGISTER_FIFO_CTRL_REG       = 0x2E,   // 00000000   rw
	GYRO_REGISTER_FIFO_SRC_REG        = 0x2F,   //            r
	GYRO_REGISTER_INT1_CFG            = 0x30,   // 00000000   rw
	GYRO_REGISTER_INT1_SRC            = 0x31,   //            r
	GYRO_REGISTER_TSH_XH              = 0x32,   // 00000000   rw
	GYRO_REGISTER_TSH_XL              = 0x33,   // 00000000   rw
	GYRO_REGISTER_TSH_YH              = 0x34,   // 00000000   rw
	GYRO_REGISTER_TSH_YL              = 0x35,   // 00000000   rw
	GYRO_REGISTER_TSH_ZH              = 0x36,   // 00000000   rw
	GYRO_REGISTER_TSH_ZL              = 0x37,   // 00000000   rw
	GYRO_REGISTER_INT1_DURATION       = 0x38    // 00000000   rw
} gyroRegisters_t;
/*  OPTIONAL SPEED SETTINGS
---------------------------------------------------------------------------*/
typedef enum {
	GYRO_RANGE_250DPS  = 250,
	GYRO_RANGE_500DPS  = 500,
	GYRO_RANGE_2000DPS = 2000
} gyroRange_t;
/*===END DATA L3GD20=======================================================*/


class CMyIMU {
public:
	CMyIMU();
	bool begin(bmp085_mode_t  bmp085_mode,gyroRange_t l3gd20_rng);
	Cardan3D getCurrentOrientation(){return currentOrient;}
	Cardan3D getRawOrientation(){return raworient;}
	Cardan3D getGyroOrientation(){return gyroorient;}
	void readSensors(double dt);
	// fonction du baromètre
	void adjustQNH(double QNH);
	void alttoQNH(double altitude);
	double getCurrentQNH(){return _QNH;}
	double getTemperature(){return newData.temperature;}
	double getPression(){return newData.pression;}
	double getAltitude(){return newData.altitude;}
	double getGyro(uint8_t composante,bool oriented=false);
	double getAccel(uint8_t composante,bool oriented=false);
	double getMagnetic(uint8_t composante,bool oriented=false);
	uint32_t getTimeStamp(){return newData.timestamp;}
private:
	void calcAltitude();
	void updatePitchRoll(double acX,double acY,double acZ);
	void updateYaw(double magX,double magY,double magZ);
//
	Kalman kalmanX, kalmanY, kalmanZ;
//
	SensorData oldData,newData,rotated;
	Cardan3D currentOrient,raworient,gyroorient;
//
	double _QNH;
//
	bool init;
//  parametres du LSM303
	lsm303MagGain   _lsm303_magGain;
	bool _lsm303_autoRangeEnabled;
	float _lsm303Accel_MG_LSB   ;   // 1, 2, 4 or 12 mg per lsb
	float _lsm303Mag_Gauss_LSB_XY;  // Varies with gain
	float _lsm303Mag_Gauss_LSB_Z;   // Varies with gain
	void setMagGain(enum lsm303MagGain gain);
	void getLSM303AccData(double *ax,double *ay,double *az);
	void getLSM303MagData(double *mx,double *my,double *mz);
//  paramètres du BMP085 baromètre/thermomètre
	bmp085_calib_data _bmp085_coeffs;
	uint8_t           _bmp085Mode;
	unsigned long int time;
	void readBMP085Coefficients();
	void getBMP085Data(double *pressure,double *temperature);
// Paramètres du L3GD20 gyroscope
	gyroRange_t _l3gd20_range;
	bool        _l3gd20_autoRangeEnabled;
	void getL3GD20Data(double *gx,double *gy,double *gz);
// I2C functions
	void writeCommand(byte reg, byte value, uint8_t address);
	void read8(byte reg, uint8_t *value, uint8_t address);
	void read16(byte reg, uint16_t *value, uint8_t address);
	void readS16(byte reg, int16_t *value, uint8_t address);
};

#endif

