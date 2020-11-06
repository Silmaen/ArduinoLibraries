//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
#include <Arduino.h>

namespace lsm303 {
/*=========================================================================
    I2C ADDRESS/BITS
---------------------------------------------------------------------------*/
	constexpr uint8_t AddressAccel =         (0x32 >> 1);         // 0011001x
	constexpr uint8_t AddressMag =           (0x3C >> 1);         // 0011110x
/*=========================================================================*/

/*=========================================================================
    REGISTERS
---------------------------------------------------------------------------*/
	enum AccelRegisters{                                          // DEFAULT    TYPE
		CTRL_REG1_A = 0x20,   // 00000111   rw
		CTRL_REG2_A = 0x21,   // 00000000   rw
		CTRL_REG3_A = 0x22,   // 00000000   rw
		CTRL_REG4_A = 0x23,   // 00000000   rw
		CTRL_REG5_A = 0x24,   // 00000000   rw
		CTRL_REG6_A = 0x25,   // 00000000   rw
		REFERENCE_A = 0x26,   // 00000000   r
		STATUS_REG_A = 0x27,   // 00000000   r
		OUT_X_L_A = 0x28,
		OUT_X_H_A = 0x29,
		OUT_Y_L_A = 0x2A,
		OUT_Y_H_A = 0x2B,
		OUT_Z_L_A = 0x2C,
		OUT_Z_H_A = 0x2D,
		FIFO_CTRL_REG_A = 0x2E,
		FIFO_SRC_REG_A = 0x2F,
		INT1_CFG_A = 0x30,
		INT1_SOURCE_A = 0x31,
		INT1_THS_A = 0x32,
		INT1_DURATION_A = 0x33,
		INT2_CFG_A = 0x34,
		INT2_SOURCE_A = 0x35,
		INT2_THS_A = 0x36,
		INT2_DURATION_A = 0x37,
		CLICK_CFG_A = 0x38,
		CLICK_SRC_A = 0x39,
		CLICK_THS_A = 0x3A,
		TIME_LIMIT_A = 0x3B,
		TIME_LATENCY_A = 0x3C,
		TIME_WINDOW_A = 0x3D
	};

	enum struct MagRegisters{
		CRA_REG_M = 0x00,
		CRB_REG_M = 0x01,
		MR_REG_M = 0x02,
		OUT_X_H_M = 0x03,
		OUT_X_L_M = 0x04,
		OUT_Z_H_M = 0x05,
		OUT_Z_L_M = 0x06,
		OUT_Y_H_M = 0x07,
		OUT_Y_L_M = 0x08,
		SR_REG_Mg = 0x09,
		IRA_REG_M = 0x0A,
		IRB_REG_M = 0x0B,
		IRC_REG_M = 0x0C,
		TEMP_OUT_H_M = 0x31,
		TEMP_OUT_L_M = 0x32
	};
/*=========================================================================*/

	/**
	 * @brief Mode of the Accelerometer
	 */
	enum class AccelMode {
		OFF,             ///< off mode
		LOW_POWER,  ///< Low power mode
		NORMAL,      ///< Normal measurement mode (default)
	};

	/**
	 * @brief Data rate of the accelerometer
	 */
	enum class AccelRate {
		HZ_1  = 0b00010000,     ///< 1Hz
		HZ_10 = 0b00100000,     ///< 10Hz
		HZ_25 = 0b00110000,     ///< 25Hz
		HZ_50 = 0b01000000,     ///< 50Hz
		HZ_100 = 0b01010000,    ///< 100Hz (default)
		HZ_200 = 0b01100000,    ///< 200Hz
		HZ_400 = 0b01110000,    ///< 200Hz
		HZ_LP1620 = 0b10000000, ///< 1620Hz Low Power only
		HZ_ULTRA = 0b10010000,  ///< 1344Hz in normal mode, 5376Hz in Low Power Mode
	};

	/**
	 * @brief FullScale selection
	 */
	enum class AccelRange {
		R_2G = 0b00,  ///< +/- 2g
		R_4G = 0b01,  ///< +/- 4g
		R_8G = 0b10,  ///< +/- 8g
		R_16G = 0b11, ///< +/- 4g
	};

	/**
	 * @brief Resolution of the measure
	 */
	enum class AccelResolution {
		LOW_RES = 0, ///< High resolution mode disable
		HIGH_RES = 1, ///< High resolution mode
	};

	/**
	 * @brief construct the CTRL_REG1_A register value based on inputs
	 * @param mode the power mode
	 * @param rate the rate of measure
	 * @param X if the X axis is enabled
	 * @param Y if the Y axis is enabled
	 * @param Z if the Z axis is enabled
	 * @return the register value
	 */
	uint8_t AccelReg1AValue(const AccelMode& mode=AccelMode::NORMAL, const AccelRate& rate=AccelRate::HZ_100,
						 bool X=true, bool Y=true, bool Z=true);
	/**
	 * @brief construct the CTRL_REG4_A register value based on inputs
	 * @param range the range of measure
	 * @param HighResMode if Hight resolution should be enabled
	 * @param LittleEndian false: LSB @ lower address, true: MSB @ lower address
	 * @return the register value
	 */
	uint8_t AccelReg4AValue(const AccelRange& range=AccelRange::R_2G, const AccelResolution& HighResMode=AccelResolution::LOW_RES, bool LittleEndian= false);

	/**
	 * @brief get the factor to convert raw data into 'g' unit
	 * @param range the sensor range parameter
	 * @return the unit conversion factor
	 */
	float getGperBit(const AccelRange& range=AccelRange::R_2G);
	/**
	 * @brief get the factor to convert raw data into 'm/s/s' unit
	 * @param range the sensor range parameter
	 * @return the unit conversion factor
	 */
	float getMSperBit(const AccelRange& range=AccelRange::R_2G);

/*=========================================================================
    MAGNETOMETER GAIN SETTINGS
---------------------------------------------------------------------------*/
	enum struct MagGain {
		G1_3 = 0x20,  ///< +/- 1.3
		G1_9 = 0x40,  ///< +/- 1.9
		G2_5 = 0x60,  ///< +/- 2.5
		G4_0 = 0x80,  ///< +/- 4.0
		G4_7 = 0xA0,  ///< +/- 4.7
		G5_6 = 0xC0,  ///< +/- 5.6
		G8_1 = 0xE0   ///< +/- 8.1
	};
/*=========================================================================
    MAGNETOMETER UPDATE RATE SETTINGS
-----------------------------------------------------------------------*/
	enum struct MagRate {
		R0_7   = 0x00,  ///< 0.75 Hz
		R1_5   = 0x01,  ///< 1.5 Hz
		R3_0   = 0x62,  ///< 3.0 Hz
		R7_5   = 0x03,  ///< 7.5 Hz
		R15    = 0x04,  ///< 15 Hz
		R30    = 0x05,  ///< 30 Hz
		R75    = 0x06,  ///< 75 Hz
		R220   = 0x07   ///< 200 Hz
	};
/*=========================================================================*/

/*=========================================================================
    CHIP ID
---------------------------------------------------------------------------*/
	constexpr uint8_t Id                     (0b11010100);
/*=========================================================================*/
}