//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
namespace bmp085 {
/*=========================================================================
  I2C ADDRESS/BITS
---------------------------------------------------------------------------*/
	constexpr uint8_t AddressPress                (0x77)
/*=========================================================================*/

/*=========================================================================
  REGISTERS
---------------------------------------------------------------------------*/
	enum struct pressRegister{
		CAL_AC1            = 0xAA,  // R   Calibration data (16 bits)
		CAL_AC2            = 0xAC,  // R   Calibration data (16 bits)
		CAL_AC3            = 0xAE,  // R   Calibration data (16 bits)
		CAL_AC4            = 0xB0,  // R   Calibration data (16 bits)
		CAL_AC5            = 0xB2,  // R   Calibration data (16 bits)
		CAL_AC6            = 0xB4,  // R   Calibration data (16 bits)
		CAL_B1             = 0xB6,  // R   Calibration data (16 bits)
		CAL_B2             = 0xB8,  // R   Calibration data (16 bits)
		CAL_MB             = 0xBA,  // R   Calibration data (16 bits)
		CAL_MC             = 0xBC,  // R   Calibration data (16 bits)
		CAL_MD             = 0xBE,  // R   Calibration data (16 bits)
		CHIPID             = 0xD0,
		VERSION            = 0xD1,
		SOFTRESET          = 0xE0,
		CONTROL            = 0xF4,
		TEMPDATA           = 0xF6,
		PRESSUREDATA       = 0xF6,
		READTEMPCMD        = 0x2E,
		READPRESSURECMD    = 0x34
	};
/*=========================================================================*/

/*=========================================================================
    MODE SETTINGS
---------------------------------------------------------------------------*/
	enum struct mode{
		ULTRALOWPOWER          = 0,
		STANDARD               = 1,
		HIGHRES                = 2,
		ULTRAHIGHRES           = 3
	};
/*=========================================================================*/

/*=========================================================================
    CALIBRATION DATA
---------------------------------------------------------------------------*/
	struct calibrationData;{
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
	};
/*=========================================================================*/
}
