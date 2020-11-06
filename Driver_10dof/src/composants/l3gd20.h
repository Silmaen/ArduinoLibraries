//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
namespace l3gd20 {
/*=========================================================================
    I2C ADDRESS/BITS AND SETTINGS
---------------------------------------------------------------------------*/
	constexpr uint8_t AdressGyro =          0x6B;        // 1101011
	constexpr uint8_t PollTimeout =         100;         // Maximum number of read attempts
	constexpr uint8_t L3GD20_ID = 0xD4;          //!< L3GD20 ID
	constexpr uint8_t L3GD20H_ID = 0xD7;         //!< L3GD20H ID
	/*=========================================================================*/

/*=========================================================================
    REGISTERS
---------------------------------------------------------------------------*/
	enum struct gyroRegisters{                                // DEFAULT    TYPE
		WHO_AM_I            = 0x0F,   // 11010100   r
		CTRL_REG1           = 0x20,   // 00000111   rw
		CTRL_REG2           = 0x21,   // 00000000   rw
		CTRL_REG3           = 0x22,   // 00000000   rw
		CTRL_REG4           = 0x23,   // 00000000   rw
		CTRL_REG5           = 0x24,   // 00000000   rw
		REFERENCE           = 0x25,   // 00000000   rw
		OUT_TEMP            = 0x26,   //            r
		STATUS_REG          = 0x27,   //            r
		OUT_X_L             = 0x28,   //            r
		OUT_X_H             = 0x29,   //            r
		OUT_Y_L             = 0x2A,   //            r
		OUT_Y_H             = 0x2B,   //            r
		OUT_Z_L             = 0x2C,   //            r
		OUT_Z_H             = 0x2D,   //            r
		FIFO_CTRL_REG       = 0x2E,   // 00000000   rw
		FIFO_SRC_REG        = 0x2F,   //            r
		INT1_CFG            = 0x30,   // 00000000   rw
		INT1_SRC            = 0x31,   //            r
		TSH_XH              = 0x32,   // 00000000   rw
		TSH_XL              = 0x33,   // 00000000   rw
		TSH_YH              = 0x34,   // 00000000   rw
		TSH_YL              = 0x35,   // 00000000   rw
		TSH_ZH              = 0x36,   // 00000000   rw
		TSH_ZL              = 0x37,   // 00000000   rw
		INT1_DURATION       = 0x38    // 00000000   rw
	};
/*=========================================================================*/

/*=========================================================================
    OPTIONAL SPEED SETTINGS
---------------------------------------------------------------------------*/
	enum struct gyroRange{
		R250DPS  = 250,
		R500DPS  = 500,
		R2000DPS = 2000
	};
/*=========================================================================*/
	constexpr float GyroSensitivity_250DPS  = 0.00875F;    // Roughly 22/256 for fixed point match
	constexpr float GyroSensitivity_500DPS  = 0.0175F;     // Roughly 45/256
	constexpr float GyroSensitivity_2000DPS = 0.070F;      // Roughly 18/256

}