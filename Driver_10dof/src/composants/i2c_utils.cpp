//
// Created by damien.lachouette on 29/09/2020.
//

#include <Arduino.h>
#include <Wire.h>
#include "i2c_utils.h"

namespace i2c_utils {

    /**
     * \brief write one bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \param value the value to write
     */
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

    /**
     * \brief read one bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the content of the byte
     */
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

    /**
     * \brief read the 2 bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the value as unsigned integer
     */
    uint16_t read16(const uint8_t address, const byte reg){
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
        value = (uint8_t)((uint8_t)Wire.read() << 8u) | (uint8_t)Wire.read();
#else
        value = (Wire.receive() << 8) | Wire.receive();
#endif
        Wire.endTransmission();
        return value;
    }

    /**
     * \brief read the 2 bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the value as signed integer
     */
    int16_t readS16(const uint8_t address, const byte reg){
        return (int16_t)i2c_utils::read16(address, reg);
    }

    /**
     * \brief get
     * \param address the address of the device
     * \param reg the begin register to read in the device
     * \param highFirst if true the highest byte is get first
     * \return the vector
     */
	vec3s16 readV16(const uint8_t address, const byte reg,const bool highFirst) {
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
        uint8_t xhi, xlo, yhi, ylo, zhi, zlo;
        if (highFirst) {
#if ARDUINO >= 100
            xhi = Wire.read(); xlo = Wire.read();
			yhi = Wire.read(); ylo = Wire.read();
			zhi = Wire.read(); zlo = Wire.read();
#else
            xhi = Wire.receive(); xlo = Wire.receive();
            yhi = Wire.receive(); ylo = Wire.receive();
            zhi = Wire.receive(); zlo = Wire.receive();
#endif
        } else {
#if ARDUINO >= 100
            xlo = Wire.read(); xhi = Wire.read();
			ylo = Wire.read(); yhi = Wire.read();
			zlo = Wire.read(); zhi = Wire.read();
#else
            xlo = Wire.receive(); xhi = Wire.receive();
            ylo = Wire.receive(); yhi = Wire.receive();
            zlo = Wire.receive(); zhi = Wire.receive();
#endif
        }
        // Shift values to create properly formed integer
        return vec3s16{(int16_t)(xlo | (xhi << 8)), (int16_t)(ylo | (yhi << 8)), (int16_t)(zlo | (zhi << 8))};
    }
}

