//
// Created by Silmaen on 29/09/2020.
//

#pragma once
#include "i2c_utils.h"

constexpr uint8_t shift4  = 4U;
constexpr uint8_t shift11 = 11U;
constexpr uint8_t shift12 = 12U;
constexpr uint8_t shift13 = 13U;
constexpr uint8_t shift15 = 15U;

namespace bmp085 {
    /*=========================================================================
      I2C ADDRESS/BITS
    ---------------------------------------------------------------------------*/
    constexpr uint8_t barometer_address = 0x77;

    constexpr uint32_t delay_temp = 4500U;
    constexpr uint16_t delayULP   = 4500U;  // delay for pressure in ultra low power
    constexpr uint16_t delaySTD   = 7500U;  // delay for pressure in normal mode
    constexpr uint16_t delayHiR   = 13500U; // delay for pressure in High resolution
    constexpr uint16_t delayUHR   = 25500U; // delay for pressure in ultra High resolution

    constexpr float degPerLsb = 0.1;  // convert raw bit into temperature in C
    constexpr float HPafromPa = 0.01; // convert Pa into hPa

    constexpr uint8_t chip_id = 0x55;

    constexpr int16_t  B_AC1 = 408;
    constexpr int16_t  B_AC2 = -72;
    constexpr int16_t  B_AC3 = -14383;
    constexpr uint16_t B_AC4 = 32741;
    constexpr uint16_t B_AC5 = 32757;
    constexpr uint16_t B_AC6 = 23153;
    constexpr int16_t  B_B1  = 6190;
    constexpr int16_t  B_B2  = 4;
    constexpr int16_t  B_MB  = -32768;
    constexpr int16_t  B_MC  = -8711;
    constexpr int16_t  B_MD  = 2868;
    /*=========================================================================*/
    class Baro : public i2c::device<barometer_address, float> {
    public:
        /**
         * \brief mode of the device
         */
        enum struct Mode {
            ULTRALOWPOWER = 0, ///< LowPower
            STANDARD      = 1, ///< Normal Mode
            HIGHRES       = 2, ///< Hires mode
            ULTRAHIGHRES  = 3  ///< Ultra HiRes mode
        };

        /**
         * \brief device settings
         */
        struct Setting {
            Mode mode = Mode::STANDARD;
        };

        /**
         * \brief default constructor
         */
        Baro() {
            if (read8(CHIPID) != chip_id)
                return;
            readCalibration();
        }

        /**
         * \brief constructor based on settings
         * \param settings the initialization settings
         */
        explicit Baro(const Setting &settings) : setting{settings} {
            if (read8(CHIPID) != chip_id)
                return;
            readCalibration();
        }

        /**
         * \brief measure the temperature of the device
         * \return the measured temperature (the absolute 0 if not available)
         */
        [[nodiscard]] float getTemperature() override { return ((computeB5() + 8U) >> shift4) * degPerLsb; }

        /**
         * \brief defines the mode of the sensor
         * \param mode the new mode
         */
        void setMode(const Mode &mode) { setting.mode = mode; }

        /**
         * \brief get the actual mode of the device
         * \return the mode of the device
         */
        [[nodiscard]] const Mode &getMode() const { return setting.mode; }

    private:
        Setting setting; ///< settings of the device
        /**
         * \brief device registers
         */
        enum Register {
            CAL_AC1         = 0xAA, // R   Calibration data (16 bits)
            CAL_AC2         = 0xAC, // R   Calibration data (16 bits)
            CAL_AC3         = 0xAE, // R   Calibration data (16 bits)
            CAL_AC4         = 0xB0, // R   Calibration data (16 bits)
            CAL_AC5         = 0xB2, // R   Calibration data (16 bits)
            CAL_AC6         = 0xB4, // R   Calibration data (16 bits)
            CAL_B1          = 0xB6, // R   Calibration data (16 bits)
            CAL_B2          = 0xB8, // R   Calibration data (16 bits)
            CAL_MB          = 0xBA, // R   Calibration data (16 bits)
            CAL_MC          = 0xBC, // R   Calibration data (16 bits)
            CAL_MD          = 0xBE, // R   Calibration data (16 bits)
            CHIPID          = 0xD0,
            VERSION         = 0xD1,
            SOFTRESET       = 0xE0,
            CONTROL         = 0xF4,
            TEMPDATA        = 0xF6,
            PRESSUREDATA    = 0xF6,
            READTEMPCMD     = 0x2E,
            READPRESSURECMD = 0x34
        };

        void m_Measure() override {
            constexpr int32_t  magic_number1 = 4000;
            constexpr int32_t  magic_number2 = 32768;
            constexpr uint32_t magic_number3 = 50000;
            constexpr uint32_t magic_number4 = 0x80000000;
            constexpr int32_t  magic_number5 = 3038;
            constexpr int32_t  magic_number6 = -7357;
            constexpr int32_t  magic_number7 = -3791;

            int32_t b6 = computeB5() - magic_number1;
            int32_t x1 = static_cast<uint32_t>(cal.b2 * static_cast<uint32_t>(b6 * b6) >> shift12) >> shift11;
            int32_t x2 = static_cast<uint32_t>(cal.ac2 * b6) >> shift11;
            int32_t x3 = x1 + x2;
            int32_t b3 =
              static_cast<uint32_t>(
                (static_cast<uint32_t>((static_cast<uint16_t>(cal.ac1) << 2U) + x3) << static_cast<uint8_t>(setting.mode)) + 2) >>
              2U;
            x1          = static_cast<uint32_t>(cal.ac3 * b6) >> shift13;
            x2          = static_cast<uint32_t>(cal.b1 * (static_cast<uint32_t>(b6 * b6) >> shift12)) >> double_byte_shift;
            x3          = static_cast<uint32_t>((x1 + x2) + 2) >> 2U;
            uint32_t b4 = cal.ac4 * static_cast<uint32_t>(x3 + magic_number2) >> shift15;
            uint32_t b7 = (readUncompensatedPressure() - b3) * (magic_number3 >> static_cast<uint8_t>(setting.mode));
            int32_t  p  = 0;
            if (b7 < magic_number4) {
                p = (b7 * 2) / b4;
            } else {
                p = (b7 / b4) * 2;
            }
            x1   = (static_cast<uint32_t>(p) >> byte_shift) * (static_cast<uint32_t>(p) >> byte_shift);
            x1   = static_cast<uint32_t>(x1 - magic_number5) >> double_byte_shift;
            x2   = static_cast<uint32_t>(magic_number6 * p) >> double_byte_shift;
            _dta = (p + (static_cast<uint32_t>(x1 + x2 + magic_number7) >> shift4)) * HPafromPa;
        }

        /**
         * \brief raw read of temperature
         * \return
         */
        [[nodiscard]] int16_t readUncompensatedTemperature() {
            writeCommand(CONTROL, READTEMPCMD);
            delayMicroseconds(delay_temp);
            return readS16(TEMPDATA);
        }

        /**
         * \brief raw read of temperature
         * \return
         */
        [[nodiscard]] uint32_t readUncompensatedPressure() {
            constexpr uint8_t shift6 = 6U;

            writeCommand(CONTROL, READPRESSURECMD + (static_cast<uint8_t>(setting.mode) << shift6));
            switch (setting.mode) {
            case Mode::ULTRALOWPOWER: delayMicroseconds(delayULP); break;
            case Mode::STANDARD: delayMicroseconds(delaySTD); break;
            case Mode::HIGHRES: delayMicroseconds(delayHiR); break;
            case Mode::ULTRAHIGHRES: delayMicroseconds(delayUHR); break;
            }
            return read24(TEMPDATA) >> static_cast<uint8_t>(byte_shift - static_cast<uint8_t>(setting.mode));
        }

        /**
         * \brief compute the 'B5' value based on raw temperature
         * \return the B5 value
         */
        [[nodiscard]] int16_t computeB5() {
            int16_t X1 = (readUncompensatedTemperature() - cal.ac6) * cal.ac5 >> shift15;
            int16_t X2 = (static_cast<uint16_t>(cal.mc) << shift11) / (X1 + cal.md);
            return X2 + X1;
        }

        /**
         * \brief device calibration data
         */
        struct calibrationData {
            int16_t  ac1 = B_AC1;
            int16_t  ac2 = B_AC2;
            int16_t  ac3 = B_AC3;
            uint16_t ac4 = B_AC4;
            uint16_t ac5 = B_AC5;
            uint16_t ac6 = B_AC6;
            int16_t  b1  = B_B1;
            int16_t  b2  = B_B2;
            int16_t  mb  = B_MB;
            int16_t  mc  = B_MC;
            int16_t  md  = B_MD;
        };

        calibrationData cal; ///< storage of calibration data

        /**
         * \brief read & store calibration data
         */
        void readCalibration() {
            cal.ac1 = readS16(CAL_AC1);
            cal.ac2 = readS16(CAL_AC2);
            cal.ac3 = readS16(CAL_AC3);
            cal.ac4 = read16(CAL_AC4);
            cal.ac5 = read16(CAL_AC5);
            cal.ac6 = read16(CAL_AC6);
            cal.b1  = readS16(CAL_B1);
            cal.b2  = readS16(CAL_B2);
            cal.mb  = readS16(CAL_MB);
            cal.mc  = readS16(CAL_MC);
            cal.md  = readS16(CAL_MD);
        }
    };
} // namespace bmp085
