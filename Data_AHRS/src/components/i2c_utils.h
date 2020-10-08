//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
#include <Arduino.h>
#include <Wire.h>
namespace i2c_utils {

    /**
     * \brief Micro structure handling vector in sensors
     */
    struct v3 {
        int16_t x,y,z;
    };

    /**
     * \brief write one bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \param value the value to write
     */
    void writeCommand(const uint8_t address, const byte reg,const byte value);

    /**
     * \brief read one bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the content of the byte
     */
    uint8_t read8(const uint8_t address, const byte reg);

    /**
     * \brief read the 2 bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the value as unsigned integer
     */
    uint16_t read16(const uint8_t address, const byte reg);

    /**
     * \brief read the 2 bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the value as signed integer
     */
    int16_t readS16(const uint8_t address, const byte reg);

    /**
     * \brief get
     * \param address the address of the device
     * \param reg the begin register to read in the device
     * \param highFirst if true the highest byte is get first
     * \return the vector
     */
    v3 readV16(const uint8_t address, const byte reg,const bool highFirst=true);

}
