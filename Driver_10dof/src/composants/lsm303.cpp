//
// Created by damien.lachouette on 06/11/2020.
//
#include "lsm303.h"

namespace lsm303 {

	uint8_t AccelReg1AValue(const AccelMode& mode, const AccelRate& rate, bool X, bool Y, bool Z){
		if(mode==AccelMode::OFF)
			return 0;
		return ((uint8_t)rate) | ((uint8_t)(mode==AccelMode::LOW_POWER) << 3u) | ((uint8_t)Z << 2u) | ((uint8_t)Y << 1u) | (uint8_t)X;
	}

	uint8_t AccelReg4AValue(const AccelRange& range, const AccelResolution& HighResMode, bool LittleEndian){
		// other bits:
		// bit 7 - Block data update: leave to 0 = continuous updatre
		// bit 0 - SPI Iterface: we don't use SPI....
		return (uint8_t)LittleEndian << 6u | (uint8_t)range << 4u | (uint8_t)HighResMode << 3u;
	}

	float getGperBit(const AccelRange& range){
		switch(range){
			case AccelRange::R_2G:
				return 1.0F/1000.0F;
			case AccelRange::R_4G:
				return 2.0F/1000.0F;
			case AccelRange::R_8G:
				return 4.0F/1000.0F;
			case AccelRange::R_16G:
				return 12.0F/1000.0F;
		}
		return 0;
	}

	float getMSperBit(const AccelRange& range){
		return 9.80665F * getGperBit(range);
	}

}