//
// Created by Silmaen on 29/09/2020.
//

#include "i2c_utils.h"

#include <Arduino.h>
#include <Wire.h>

namespace i2c_utils {

    constexpr uint8_t byte_shift = 8U;

    /**
     * \brief write one bytes at the given register in the given device
     * \param address the device ID tyo read
     * \param reg the register to read
     * \param value the value to write
     */
    void writeCommand(const uint8_t address, const byte reg, const byte value) {
        Wire.beginTransmission(address);
#if ARDUINO >= 100
        Wire.write(static_cast<uint8_t>(reg));
        Wire.write(static_cast<uint8_t>(value));
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
    [[nodiscard]] uint8_t read8(const uint8_t address, const byte reg) {
        Wire.beginTransmission(address);
#if ARDUINO >= 100
        Wire.write(static_cast<uint8_t>(reg));
#else
        Wire.send(reg);
#endif
        Wire.endTransmission();
        Wire.requestFrom(address, static_cast<byte>(1));
#if ARDUINO >= 100
        uint8_t value = Wire.read();
#else
        uint8_t value = Wire.receive();
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
    [[nodiscard]] uint16_t read16(const uint8_t address, const byte reg) {
        Wire.beginTransmission(address);
#if ARDUINO >= 100
        Wire.write(static_cast<uint8_t>(reg));
#else
        Wire.send(reg);
#endif
        Wire.endTransmission();
        Wire.requestFrom(address, static_cast<byte>(2));
#if ARDUINO >= 100
        uint16_t value =
          static_cast<uint8_t>(static_cast<uint8_t>(Wire.read()) << byte_shift) | static_cast<uint8_t>(Wire.read());
#else
        uint16_t value = (Wire.receive() << 8U) | Wire.receive();
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
    [[nodiscard]] int16_t readS16(const uint8_t address, const byte reg) {
        return static_cast<int16_t>(i2c_utils::read16(address, reg));
    }

    /**
     * \brief get
     * \param address the address of the device
     * \param reg the begin register to read in the device
     * \param highFirst if true the highest byte is get first
     * \return the vector
     */
    [[nodiscard]] vec3s16 readV16(const uint8_t address, const byte reg, const bool highFirst) {
        Wire.beginTransmission(address);
#if ARDUINO >= 100
        Wire.write(reg);
#else
        Wire.send(reg);
#endif
        constexpr uint8_t nb_byte_in_vector = 6;
        Wire.endTransmission();
        Wire.requestFrom(address, nb_byte_in_vector);
        // Wait around until enough data is available
        while (Wire.available() < nb_byte_in_vector) {}
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
            // Shift values to create properly formed integer
            return vec3s16{static_cast<int16_t>(xlo | static_cast<uint16_t>(xhi << byte_shift)),
                           static_cast<int16_t>(ylo | static_cast<uint16_t>(yhi << byte_shift)),
                           static_cast<int16_t>(zlo | static_cast<uint16_t>(zhi << byte_shift))};
        }
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
        // Shift values to create properly formed integer
        return vec3s16{static_cast<int16_t>(xlo | static_cast<uint16_t>(xhi << byte_shift)),
                       static_cast<int16_t>(ylo | static_cast<uint16_t>(yhi << byte_shift)),
                       static_cast<int16_t>(zlo | static_cast<uint16_t>(zhi << byte_shift))};
    }
} // namespace i2c_utils
