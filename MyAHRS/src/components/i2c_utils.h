//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
#include <Arduino.h>
#include <Wire.h>

namespace i2c_utils {
	void writeCommand(const uint8_t address, const byte reg,const byte value){
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

	uint8_t read8(const uint8_t address, const byte reg){
		uint8_t value;
		Wire.beginTransmission(address);
#if ARDUINO >= 100
		Wire.write((uint8_t)reg);
#else
		Wire.send(reg);
#endif
		Wire.endTransmission();
		Wire.requestFrom(address, (byte)1);
#if ARDUINO >= 100
		value = Wire.read();
#else
		value = Wire.receive();
#endif
		Wire.endTransmission();
		return value;
	}

	uint16_t void read16(const uint8_t address, const byte reg){
		uint16_t value;
		Wire.beginTransmission(address);
#if ARDUINO >= 100
		Wire.write((uint8_t)reg);
#else
		Wire.send(reg);
#endif
		Wire.endTransmission();
		Wire.requestFrom(address, (byte)2);
#if ARDUINO >= 100
		value = (Wire.read() << 8) | Wire.read();
#else
		value = (Wire.receive() << 8) | Wire.receive();
#endif
		Wire.endTransmission();
		return value;
	}

	int16_t readS16(const uint8_t address, const byte reg){
		return (int16_t)read16(address, reg);
	}

	struct v3 {
		int16_t x,y,z;
	};
	v3 readV16(const uint8_t address, const byte reg,const bool highFirst=true) {
		v3 value;
		Wire.beginTransmission(address);
#if ARDUINO >= 100
		Wire.write(reg);
#else
		Wire.send(reg);
#endif
		Wire.endTransmission();
		Wire.requestFrom(address, (byte)6);
		// Wait around until enough data is available
		while (Wire.available() < 6);
		if (highFirst) {
#if ARDUINO >= 100
			uint8_t xhi = Wire.read();
			uint8_t xlo = Wire.read();
			uint8_t yhi = Wire.read();
			uint8_t ylo = Wire.read();
			uint8_t zhi = Wire.read();
			uint8_t zlo = Wire.read();
#else
			uint8_t xhi = Wire.receive();
			uint8_t xlo = Wire.receive();
			uint8_t yhi = Wire.receive();
			uint8_t ylo = Wire.receive();
			uint8_t zhi = Wire.receive();
			uint8_t zlo = Wire.receive();
#endif
		} else {
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
		}
		// Shift values to create properly formed integer
		value.x = (int16_t)(xlo | (xhi << 8));
		value.y = (int16_t)(ylo | (yhi << 8));
		value.z = (int16_t)(zlo | (zhi << 8));
		return value;
	}
};
}
