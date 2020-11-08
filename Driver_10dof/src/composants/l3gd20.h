//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once
#include "i2c_utils.h"

namespace l3gd20 {
    /*=========================================================================
        I2C ADDRESS/BITS AND SETTINGS
    ---------------------------------------------------------------------------*/
    constexpr uint8_t gyroscope_address = 0x6B; // 1101011
    constexpr uint8_t PollTimeout       = 100;  // Maximum number of read attempts
    constexpr uint8_t L3GD20_ID         = 0xD4; //!< L3GD20 ID
    constexpr uint8_t L3GD20H_ID        = 0xD7; //!< L3GD20H ID

    constexpr float GyroSensitivity_250DPS  = 0.00875F; // Roughly 22/256 for fixed point match
    constexpr float GyroSensitivity_500DPS  = 0.0175F;  // Roughly 45/256
    constexpr float GyroSensitivity_2000DPS = 0.070F;   // Roughly 18/256

    constexpr uint8_t last4bits  = 0b11110000;
    constexpr uint8_t first4bits = 0b00001111;

    class Gyro : public i2c::device<gyroscope_address, vec3f> { // 0011110x { {
    public:
        /**
         * \brief define the modes for data rate and bandwidth
         */
        enum struct RateBand {
            LR12_5     = 0b00010000, ///< Out Data Rate: 12.5Hz, no cut-Off (Low_ODR bit must be set)
            LR25       = 0b01010000, ///< Out Data Rate: 25Hz, no cut-Off (Low_ODR bit must be set)
            LR50_C16_6 = 0b11100000, ///< Out Data Rate: 50Hz, cut-Off 16.6Hz (Low_ODR bit must be set)
            R100_C12_5 = 0b00000000, ///< Out Data Rate: 100Hz, cut-Off 12.5Hz
            R100_C25   = 0b00100000, ///< Out Data Rate: 100Hz, cut-Off 25Hz
            R200_C12_5 = 0b01000000, ///< Out Data Rate: 200Hz, cut-Off 12.5Hz
            R200_C70   = 0b01110000, ///< Out Data Rate: 200Hz, cut-Off 70Hz
            R400_C20   = 0b10000000, ///< Out Data Rate: 400Hz, cut-Off 20Hz
            R400_C25   = 0b10010000, ///< Out Data Rate: 400Hz, cut-Off 25Hz
            R400_C50   = 0b10100000, ///< Out Data Rate: 400Hz, cut-Off 50Hz
            R400_C110  = 0b10110000, ///< Out Data Rate: 400Hz, cut-Off 110Hz
            R800_C30   = 0b11000000, ///< Out Data Rate: 800Hz, cut-Off 30Hz
            R800_C35   = 0b11010000, ///< Out Data Rate: 800Hz, cut-Off 35Hz
            R800_C100  = 0b11110000, ///< Out Data Rate: 800Hz, cut-Off 100Hz
        };

        /**
         * \brief the possible ranges of the sensor
         */
        enum struct Range {
            R250DPS  = 0b00000000, ///<   +/- 250 degree per second
            R500DPS  = 0b00010000, ///< +/- 500 degree per second
            R2000DPS = 0b00100000  ///< +/- 2000 degree per second
        };

        /**
         * \brief power mode
         */
        enum struct Mode {
            OFF    = 0,      ///< Power down
            Normal = 0b1111, ///< Normal mode
            Sleep  = 0b1000, ///< Sleep mode
        };

        /**
         * \brief structure gathering gyroscope settings
         */
        struct Setting {
            Mode     mode  = Mode::OFF;
            RateBand rate  = RateBand::R100_C12_5;
            Range    range = Range::R250DPS;
        };

        /**
         * \brief default constructor
         */
        Gyro() {
            setMode(Mode::Normal); ///< activate
        }

        /**
         * \brief constructor based on settings
         * \param settings the initialization settings
         */
        explicit Gyro(const Setting &settings) {
            setRange(settings.range);
            setRate(settings.rate);
            setMode(settings.mode);
        }

        /**
         * \brief define the new mode of the device
         * \param mode the new mode
         */
        void setMode(const Mode &mode) {
            if (mode == getMode())
                return;
            writeCommand(CTRL_REG1, static_cast<uint8_t>(read8(CTRL_REG1) & last4bits) | static_cast<uint8_t>(mode));
            setting.mode = mode;
        }

        /**
         * \brief define the new rate of the deice
         * \param rate the new rate
         */
        void setRate(const RateBand &rate) {
            if (rate == getRate())
                return;
            if ((rate == RateBand::LR12_5) || (rate == RateBand::LR25) || (rate == RateBand::LR50_C16_6)) {
                writeCommand(LOW_ODR, 1);
            } else {
                writeCommand(LOW_ODR, 0);
            }
            writeCommand(CTRL_REG1, static_cast<uint8_t>(rate));
            setting.rate = rate;
        }

        /**
         * \brief defines the new range of the device
         * \param range the new range
         */
        void setRange(const Range &range) {
            // setting the register REG_4 to all default (=00000000) and the range value
            if (range == setting.range)
                return;
            writeCommand(CTRL_REG4, static_cast<uint8_t>(range));
            setting.range = range;
            switch (range) {
            case Range::R250DPS: factor = GyroSensitivity_250DPS; break;
            case Range::R500DPS: factor = GyroSensitivity_500DPS; break;
            case Range::R2000DPS: factor = GyroSensitivity_2000DPS;
            }
        }

        /**
         * \brief get the actual Rate setting
         * \return the rate setting
         */
        [[nodiscard]] const RateBand &getRate() const { return setting.rate; }

        /**
         * \brief get the actual Range setting
         * \return the range setting
         */
        [[nodiscard]] const Range &getRange() const { return setting.range; }

        /**
         * \brief get the actual mode setting
         * \return the mode setting
         */
        [[nodiscard]] const Mode &getMode() const { return setting.mode; }

        /**
         * \brief measure the temperature of the device
         * \return the measured temperature (the absolute 0 if not available)
         */
        [[nodiscard]] float getTemperature() override { return static_cast<int8_t>(read8(OUT_TEMP)); }

    private:
        float factor = GyroSensitivity_250DPS; ///< foctor to convert raw data into physical value

        Setting setting; ///< device setting

        /**
         * \brief read the device for measure data
         */
        void m_Measure() override {
            uint8_t status_reg_A = read8(STATUS_REG);
            if (status_reg_A == 0) // nothing has changed
                return;
            vec3s16 raw = readV16(OUT_X_L);
            _dta.x()    = static_cast<float>(raw.x()) * factor;
            _dta.y()    = static_cast<float>(raw.y()) * factor;
            _dta.z()    = static_cast<float>(raw.z()) * factor;
        }

        /**
         * \brief list of the registers
         */
        enum Registers {          // DEFAULT    TYPE
            WHO_AM_I      = 0x0F, // 11010100   r
            CTRL_REG1     = 0x20, // 00000111   rw
            CTRL_REG2     = 0x21, // 00000000   rw
            CTRL_REG3     = 0x22, // 00000000   rw
            CTRL_REG4     = 0x23, // 00000000   rw
            CTRL_REG5     = 0x24, // 00000000   rw
            REFERENCE     = 0x25, // 00000000   rw
            OUT_TEMP      = 0x26, //            r
            STATUS_REG    = 0x27, //            r
            OUT_X_L       = 0x28, //            r
            OUT_X_H       = 0x29, //            r
            OUT_Y_L       = 0x2A, //            r
            OUT_Y_H       = 0x2B, //            r
            OUT_Z_L       = 0x2C, //            r
            OUT_Z_H       = 0x2D, //            r
            FIFO_CTRL_REG = 0x2E, // 00000000   rw
            FIFO_SRC_REG  = 0x2F, //            r
            INT1_CFG      = 0x30, // 00000000   rw
            INT1_SRC      = 0x31, //            r
            TSH_XH        = 0x32, // 00000000   rw
            TSH_XL        = 0x33, // 00000000   rw
            TSH_YH        = 0x34, // 00000000   rw
            TSH_YL        = 0x35, // 00000000   rw
            TSH_ZH        = 0x36, // 00000000   rw
            TSH_ZL        = 0x37, // 00000000   rw
            INT1_DURATION = 0x38, // 00000000   rw
            LOW_ODR       = 0x39  // 00000000   rw
        };
    };

} // namespace l3gd20