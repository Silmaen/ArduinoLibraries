//
// Created by Silmaen on 29/09/2020.
//

#pragma once
#include "math3d.h"

#include <Wire.h>

#if ARDUINO < 100
#    error This library need Arduino  greater the 1.0.0
#endif

constexpr float   absoluteZero      = -273.15;
constexpr uint8_t byte_shift        = 8U;
constexpr uint8_t double_byte_shift = 16U;

namespace i2c {
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

        virtual /**
                 * @brief starts the communication
                 */
          void
          begin() {
            Wire.begin();
        }

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
        void writeCommand(byte reg, byte value) const {
            Wire.beginTransmission(device_addr);
            Wire.write(reg);
            Wire.write(value);
            Wire.endTransmission();
        }
        /**
         * \brief read one bytes at the given register in the given device
         * \param reg the register to read
         * \return the content of the byte
         */
        [[nodiscard]] uint8_t read8(byte reg) const {
            Wire.beginTransmission(device_addr);
            Wire.write(reg);
            Wire.endTransmission();
            Wire.requestFrom(device_addr, static_cast<byte>(1));
            uint8_t value = _read();
            Wire.endTransmission();
            return value;
        }

        /**
         * \brief read the 2 bytes at the given register in the given device
         * \param reg the register to read
         * \param lowFirst if true the lowest byte is get first
         * \return the value as unsigned integer
         */
        [[nodiscard]] uint16_t read16(byte reg, bool lowFirst = false) const {
            Wire.beginTransmission(device_addr);
            Wire.write(reg);
            Wire.endTransmission();
            Wire.requestFrom(device_addr, static_cast<byte>(2));
            if (lowFirst) {
                uint16_t value = _read() | static_cast<uint8_t>(_read() << byte_shift);
                Wire.endTransmission();
                return value;
            }
            uint16_t value = static_cast<uint8_t>(_read() << byte_shift) | _read();
            Wire.endTransmission();
            return value;
        }

        /**
         * \brief read the 2 bytes at the given register in the given device
         * \param reg the register to read
         * \return the value as signed integer
         */
        [[nodiscard]] int16_t readS16(byte reg) const { return static_cast<int16_t>(read16(reg)); }

        /**
         * \brief read the 3 bytes at the given register in the given device
         * \param reg the register to read
         * \param lowFirst if true the lowest byte is get first
         * \return the value as unsigned integer
         */
        [[nodiscard]] uint32_t read24(byte reg, bool lowFirst = false) const {
            Wire.beginTransmission(device_addr);
            Wire.write(reg);
            Wire.endTransmission();
            Wire.requestFrom(device_addr, static_cast<byte>(3));
            if (lowFirst) {
                uint16_t value =
                  _read() | static_cast<uint8_t>(_read() << byte_shift) | static_cast<uint8_t>(_read() << double_byte_shift);
                Wire.endTransmission();
                return value;
            }
            uint16_t value =
              static_cast<uint8_t>(_read() << double_byte_shift) | static_cast<uint8_t>(_read() << byte_shift) | _read();
            Wire.endTransmission();
            return value;
        }

        /**
         * \brief get
         * \param reg the begin register to read in the device
         * \param highFirst if true the highest byte is get first
         * \return the vector
         */
        [[nodiscard]] vec3s16 readV16(byte reg, bool highFirst = false) const {
            Wire.beginTransmission(device_addr);
            Wire.write(reg);
            constexpr uint8_t nb_byte_in_vector = 6;
            Wire.endTransmission();
            Wire.requestFrom(device_addr, nb_byte_in_vector);
            // Wait around until enough data is available
            while (Wire.available() < nb_byte_in_vector) {}
            if (highFirst) {
                // Shift values to create properly formed integer
                return vec3s16{static_cast<int16_t>(static_cast<uint16_t>(_read() << byte_shift) | _read()),
                               static_cast<int16_t>(static_cast<uint16_t>(_read() << byte_shift) | _read()),
                               static_cast<int16_t>(static_cast<uint16_t>(_read() << byte_shift) | _read())};
            }
            // Shift values to create properly formed integer
            return vec3s16{static_cast<int16_t>(_read() | static_cast<uint16_t>(_read() << byte_shift)),
                           static_cast<int16_t>(_read() | static_cast<uint16_t>(_read() << byte_shift)),
                           static_cast<int16_t>(_read() | static_cast<uint16_t>(_read() << byte_shift))};
        }

        /**
         * \brief read on unsigned byte on the bus
         * \return the byte read
         */
        static uint8_t _read() { return static_cast<uint8_t>(Wire.read()); }
    };

} // namespace i2c
