//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
#include "math3d.h"

namespace i2c_utils {

    /**
     * \brief write one bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \param value the value to write
     */
    void writeCommand(uint8_t address, byte reg,byte value);

    /**
     * \brief read one bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the content of the byte
     */
	[[nodiscard]] uint8_t read8(uint8_t address, byte reg);

    /**
     * \brief read the 2 bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the value as unsigned integer
     */
	[[nodiscard]] uint16_t read16(uint8_t address, byte reg);

    /**
     * \brief read the 2 bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \return the value as signed integer
     */
    [[nodiscard]] int16_t readS16(uint8_t address, byte reg);

    /**
     * \brief get
     * \param address the address of the device
     * \param reg the begin register to read in the device
     * \param highFirst if true the highest byte is get first
     * \return the vector
     */
	[[nodiscard]] vec3s16 readV16(uint8_t address, byte reg,bool highFirst=true);

}
