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


    float getXYGain(MagRange range){
        switch (range) {
            case MagRange::G1_3:
                return 1.f/1100.f;
            case MagRange::G1_9:
                return 1.f/855.f;
            case MagRange::G2_5:
                return 1.f/670.f;
            case MagRange::G4_0:
                return 1.f/450.f;
            case MagRange::G4_7:
                return 1.f/400.f;
            case MagRange::G5_6:
                return 1.f/330.f;
            case MagRange::G8_1:
                return 1.f/230.f;
        }
        return 0.f;
	}

    float getZGain(MagRange range){
        switch (range) {
            case MagRange::G1_3:
                return 1.f/980.f;
            case MagRange::G1_9:
                return 1.f/760.f;
            case MagRange::G2_5:
                return 1.f/600.f;
            case MagRange::G4_0:
                return 1.f/400.f;
            case MagRange::G4_7:
                return 1.f/355.f;
            case MagRange::G5_6:
                return 1.f/295.f;
            case MagRange::G8_1:
                return 1.f/205.f;
        }
        return 0.f;
	}

}