//
// Created by Silmaen on 29/09/2020.
//

#pragma once
#include "math3d.h"

constexpr float absoluteZero = -273.15;

namespace i2c {
    namespace helper {
        /**
         * \brief write one bytes at the given register in the given device
         * \param address the device ID tyo read
         * \param reg the register to read
         * \param value the value to write
         */
        void writeCommand(uint8_t address, byte reg, byte value);

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
        [[nodiscard]] vec3s16 readV16(uint8_t address, byte reg, bool highFirst = true);
    } // namespace helper

    /**
     * \brief base class for i2c Devices
     */
    template<uint8_t device_addr, class resultType>
    class device {
    public:
        /**
         * \brief default constructor
         */
        device() = default;

        /**
         * \brief get the stored acceleration
         * \return the stored acceleration
         */
        [[nodiscard]] const resultType &getMeasure() const { return _dta; };

        /**
         * \brief do a measure, store the value and return it
         * \return the acceleration (the stored one)
         */
        const resultType &measure() {
            m_Measure();
            return _dta;
        }

        /**
         * \brief measure the temperature of the device
         * \return the measured temperature (the absolute 0 if not available)
         */
        [[nodiscard]] virtual float getTemperature() { return absoluteZero; }

    protected:
        resultType _dta; ///< the raw data

        virtual void m_Measure() = 0;

        /**
         * \brief \brief write one bytes at the given register
         * \param reg the register to read
         * \param value the value to write
         */
        void writeCommand(byte reg, byte value) const { i2c::helper::writeCommand(device_addr, reg, value); }
        /**
         * \brief read one bytes at the given register in the given device
         * \param reg the register to read
         * \return the content of the byte
         */
        [[nodiscard]] uint8_t read8(byte reg) const { return i2c::helper::read8(device_addr, reg); }
        /**
         * \brief read the 2 bytes at the given register in the given device
         * \param reg the register to read
         * \return the value as unsigned integer
         */
        [[nodiscard]] uint16_t read16(byte reg) const { return i2c::helper::read16(device_addr, reg); }
        /**
         * \brief read the 2 bytes at the given register in the given device
         * \param reg the register to read
         * \return the value as signed integer
         */
        [[nodiscard]] int16_t readS16(byte reg) const { return i2c::helper::readS16(device_addr, reg); }
        /**
         * \brief get
         * \param reg the begin register to read in the device
         * \param highFirst if true the highest byte is get first
         * \return the vector
         */
        [[nodiscard]] vec3s16 readV16(byte reg, bool highFirst = false) const {
            return i2c::helper::readV16(device_addr, reg, highFirst);
        }
    };

} // namespace i2c
