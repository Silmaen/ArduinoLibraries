//
// Created by Silmaen on 29/09/2020.
//

#pragma once
#include "i2c_utils.h"

namespace lsm303 {

    constexpr uint8_t Id                    = 0b11010100;  ///< the ID of the chip
    constexpr uint8_t accelerometer_address = 0x32U >> 1U; ///< the address of the Accelerometer in the I2C bus
    constexpr uint8_t magnetometer_address  = 0x3CU >> 1U; ///< the address of the Accelerometer in the I2C bus

    // shifting constants
    constexpr uint8_t shift_3         = 3U;
    constexpr uint8_t shift_demi_byte = 4U;
    // bit masks
    constexpr uint8_t threeFirstBitMask = 0b111U;
    constexpr uint8_t fourFirstBitMask  = 0b1111U;
    constexpr uint8_t mask_third        = 0b00001000U;
    constexpr uint8_t mask_not_third    = 0b11110111U;
    constexpr uint8_t mask_5_6          = 0b11001111U;

    // conversion factors
    //   Accel
    constexpr static float factor_2g  = 0.00980665;
    constexpr static float factor_4g  = 0.0196133;
    constexpr static float factor_8g  = 0.0392266;
    constexpr static float factor_16g = 0.1176798;
    //   Mag
    constexpr static float factor_1_3_XY = 1.F / 1100.F;
    constexpr static float factor_1_9_XY = 1.F / 855.F;
    constexpr static float factor_2_5_XY = 1.F / 670.F;
    constexpr static float factor_4_0_XY = 1.F / 450.F;
    constexpr static float factor_4_7_XY = 1.F / 400.F;
    constexpr static float factor_5_6_XY = 1.F / 330.F;
    constexpr static float factor_8_1_XY = 1.F / 230.F;
    constexpr static float factor_1_3_Z  = 1.F / 980.F;
    constexpr static float factor_1_9_Z  = 1.F / 760.F;
    constexpr static float factor_2_5_Z  = 1.F / 600.F;
    constexpr static float factor_4_0_Z  = 1.F / 400.F;
    constexpr static float factor_4_7_Z  = 1.F / 355.F;
    constexpr static float factor_5_6_Z  = 1.F / 295.F;
    constexpr static float factor_8_1_Z  = 1.F / 205.F;

    /**
     * \brief class to handle accelerometer
     */
    class Accel : public i2c::device<accelerometer_address, vec3f> { // 0011001x
    public:
        /**
         * @brief Mode of the Accelerometer
         */
        enum class Mode {
            OFF,       ///< off mode
            LOW_POWER, ///< Low power mode
            NORMAL,    ///< Normal measurement mode (default)
        };

        /**
         * @brief Data rate of the accelerometer
         */
        enum class Rate {
            HZ_1      = 0b00010000, ///< 1Hz
            HZ_10     = 0b00100000, ///< 10Hz
            HZ_25     = 0b00110000, ///< 25Hz
            HZ_50     = 0b01000000, ///< 50Hz
            HZ_100    = 0b01010000, ///< 100Hz (default)
            HZ_200    = 0b01100000, ///< 200Hz
            HZ_400    = 0b01110000, ///< 200Hz
            HZ_LP1620 = 0b10000000, ///< 1620Hz Low Power only
            HZ_ULTRA  = 0b10010000, ///< 1344Hz in normal mode, 5376Hz in Low Power Mode
        };

        /**
         * @brief FullScale selection
         */
        enum class Range {
            R_2G  = 0b00, ///< +/- 2g
            R_4G  = 0b01, ///< +/- 4g
            R_8G  = 0b10, ///< +/- 8g
            R_16G = 0b11, ///< +/- 4g
        };

        /**
         * @brief Resolution of the measure
         */
        enum class Resolution {
            LOW_RES  = 0, ///< High resolution mode disable
            HIGH_RES = 1, ///< High resolution mode
        };

        /**
         * \brief structure gathering accelerometer settings
         */
        struct Setting {
            Rate       rate       = Rate::HZ_100;        ///< Rate of sensor
            Mode       mode       = Mode::NORMAL;        ///< Mode of the sensor
            Range      range      = Range::R_2G;         ///< Range of the sensor
            Resolution resolution = Resolution::LOW_RES; ///< Resolution of the sensor
        };

        /**
         * \brief default constructor
         */
        Accel() { applySettings(); }

        /**
         * \brief construct with initial settings
         * \param settings the initial settings to apply
         */
        explicit Accel(const Setting &settings) : setting{settings} { applySettings(); }

        /**
         * \brief define the rate of measure of the device
         * \param rate the new rate to save and apply
         */
        void setRate(Rate rate) {
            if (rate == getRate())
                return;
            uint8_t reg1A = read8(CTRL_REG1_A);
            reg1A &= fourFirstBitMask;           // keep only the 4 first bits
            reg1A |= static_cast<uint8_t>(rate); // setup the rate
            writeCommand(CTRL_REG1_A, 0);        // clear all
            writeCommand(CTRL_REG1_A, reg1A);    // set
            setting.rate = rate;
        }

        /**
         * \brief define the new acceleration mode of the device
         * \param mode the new mode to save and apply
         */
        void setMode(Mode mode) {
            if (mode == getMode())
                return;
            uint8_t reg1A = read8(CTRL_REG1_A);
            if (mode == Mode::OFF) {
                writeCommand(CTRL_REG1_A, 0);
                return;
            }
            if (mode == Mode::LOW_POWER)
                reg1A |= mask_third;
            else
                reg1A &= mask_not_third;
            writeCommand(CTRL_REG1_A, reg1A);
            setting.mode = mode;
        }

        /**
         * \brief defines the new range of the device
         * \param range the new range to save and apply
         */
        void setRange(Range range) {
            if (range == getRange())
                return;
            uint8_t reg4A = read8(CTRL_REG4_A);
            reg4A &= mask_5_6; // erase range bits
            reg4A |= static_cast<uint8_t>(static_cast<uint8_t>(range) << shift_demi_byte);
            writeCommand(Registers::CTRL_REG4_A, reg4A);
            setting.range = range;
            setFactor(range);
        }

        /**
         * \brief defines the new resolution of the device
         * \param hiRes the new resolution to save and apply
         */
        void setResolution(Resolution hiRes) {
            if (hiRes == getResolution())
                return;
            uint8_t reg4A = read8(CTRL_REG4_A);
            reg4A &= mask_not_third; // erase range bits
            reg4A |= static_cast<uint8_t>(static_cast<uint8_t>(hiRes) << shift_3);
            writeCommand(CTRL_REG4_A, reg4A);
            setting.resolution = hiRes;
        }

        /**
         * \brief get the actual Rate setting
         * \return the rate setting
         */
        [[nodiscard]] const Rate &getRate() const { return setting.rate; }

        /**
         * \brief get the actual Mode setting
         * \return the mode setting
         */
        [[nodiscard]] const Mode &getMode() const { return setting.mode; }

        /**
         * \brief get the actual Range setting
         * \return the range setting
         */
        [[nodiscard]] const Range &getRange() const { return setting.range; }

        /**
         * \brief get the actual resolution setting
         * \return the resolution setting
         */
        [[nodiscard]] const Resolution &getResolution() const { return setting.resolution; }

    private:
        /**
         * \brief transmit all settings to the device
         */
        void applySettings() {
            // Enable the accelerometer (normal, 100Hz, all axis activated)
            if (setting.mode == Mode::OFF)
                writeCommand(Registers::CTRL_REG1_A, 0);
            else
                writeCommand(
                  Registers::CTRL_REG1_A,
                  static_cast<uint8_t>(setting.rate) |
                    static_cast<uint8_t>(static_cast<uint8_t>(static_cast<uint8_t>(setting.mode == Mode::LOW_POWER) << shift_3) |
                                         threeFirstBitMask));
            // set the accelerometer behavior (bigendian, 2G range, Low
            // resolution)
            writeCommand(Registers::CTRL_REG4_A, static_cast<uint8_t>(static_cast<uint8_t>(setting.range) << shift_demi_byte) |
                                                   static_cast<uint8_t>(static_cast<uint8_t>(setting.resolution) << shift_3));
            // setup the factor between raw value to physical value
            setFactor(setting.range);
        }

        Setting setting; ///< the settings of the device

        float factor = factor_2g; ///< scaling factor from raw input to m/s/s

        /**
         * \brief read in the device for the acceleration data
         */
        void m_Measure() override {
            uint8_t status_reg_A = read8(STATUS_REG_A);
            if (status_reg_A == 0) // nothing has changed
                return;
            vec3s16 raw = readV16(OUT_X_L_A);
            _dta.x()    = static_cast<float>(raw.x()) * factor;
            _dta.y()    = static_cast<float>(raw.y()) * factor;
            _dta.z()    = static_cast<float>(raw.z()) * factor;
        }

        /**
         * \brief
         * \param range
         */
        void setFactor(const Range &range) {
            switch (range) {
            case Range::R_2G: factor = factor_2g; break;
            case Range::R_4G: factor = factor_4g; break;
            case Range::R_8G: factor = factor_8g; break;
            case Range::R_16G: factor = factor_16g; break;
            }
        }

        /**
         * \brief list of registers
         */
        enum Registers {            // DEFAULT    TYPE
            CTRL_REG1_A     = 0x20, // 00000111   rw
            CTRL_REG2_A     = 0x21, // 00000000   rw
            CTRL_REG3_A     = 0x22, // 00000000   rw
            CTRL_REG4_A     = 0x23, // 00000000   rw
            CTRL_REG5_A     = 0x24, // 00000000   rw
            CTRL_REG6_A     = 0x25, // 00000000   rw
            REFERENCE_A     = 0x26, // 00000000   r
            STATUS_REG_A    = 0x27, // 00000000   r
            OUT_X_L_A       = 0x28,
            OUT_X_H_A       = 0x29,
            OUT_Y_L_A       = 0x2A,
            OUT_Y_H_A       = 0x2B,
            OUT_Z_L_A       = 0x2C,
            OUT_Z_H_A       = 0x2D,
            FIFO_CTRL_REG_A = 0x2E,
            FIFO_SRC_REG_A  = 0x2F,
            INT1_CFG_A      = 0x30,
            INT1_SOURCE_A   = 0x31,
            INT1_THS_A      = 0x32,
            INT1_DURATION_A = 0x33,
            INT2_CFG_A      = 0x34,
            INT2_SOURCE_A   = 0x35,
            INT2_THS_A      = 0x36,
            INT2_DURATION_A = 0x37,
            CLICK_CFG_A     = 0x38,
            CLICK_SRC_A     = 0x39,
            CLICK_THS_A     = 0x3A,
            TIME_LIMIT_A    = 0x3B,
            TIME_LATENCY_A  = 0x3C,
            TIME_WINDOW_A   = 0x3D
        };
    };

    /*=========================================================================
        MAGNETOMETER GAIN SETTINGS
    ---------------------------------------------------------------------------*/

    /**
     * \brief class to handle magnetometer
     */
    class Mag : public i2c::device<magnetometer_address, vec3f> { // 0011110x {
    public:
        /**
         * \brief list of possible range (large range means coarser resolution)
         */
        enum class Range {
            G1_3 = 0x20, ///< +/- 1.3
            G1_9 = 0x40, ///< +/- 1.9
            G2_5 = 0x60, ///< +/- 2.5
            G4_0 = 0x80, ///< +/- 4.0
            G4_7 = 0xA0, ///< +/- 4.7
            G5_6 = 0xC0, ///< +/- 5.6
            G8_1 = 0xE0  ///< +/- 8.1
        };
        /*=========================================================================
            MAGNETOMETER UPDATE RATE SETTINGS
        -----------------------------------------------------------------------*/
        /**
         * \brief the sensor rate of measure
         */
        enum class Rate {
            R0_7 = 0x00, ///< 0.75 Hz
            R1_5 = 0x01, ///< 1.5 Hz
            R3_0 = 0x62, ///< 3.0 Hz
            R7_5 = 0x03, ///< 7.5 Hz
            R15  = 0x04, ///< 15 Hz
            R30  = 0x05, ///< 30 Hz
            R75  = 0x06, ///< 75 Hz
            R220 = 0x07  ///< 200 Hz
        };

        /**
         * \brief structure gathering magnetometer settings
         */
        struct Setting {
            Range range = Range::G1_3; ///< Rate of sensor
            Rate  rate  = Rate::R15;   ///< Mode of the sensor
        };

        /**
         * \brief default constructor
         */
        Mag() {
            // Magnetometer
            writeCommand(Registers::MR_REG_M, 0x00U);
        }

        /**
         * \brief constructor based on settings
         * \param settings the initialization settings
         */
        explicit Mag(const Setting &settings) : setting{settings} {
            // Magnetometer
            writeCommand(Registers::MR_REG_M, 0x00U);
            setRange(setting.range);
            setRate(setting.rate);
        }

        /**
         * \brief defines the new rate of the device
         * \param rate the new rtate
         */
        void setRate(Rate rate) {
            if (rate == getRate())
                return;
            writeCommand(CRA_REG_M, static_cast<uint8_t>(rate));
            setting.rate = rate;
        }

        /**
         * \brief defines the new range of the device
         * \param range the new range
         */
        void setRange(Range range) {
            if (range == getGain())
                return;
            writeCommand(CRB_REG_M, static_cast<uint8_t>(range));
            setting.range = range;
            factorXY      = getXYGain(range);
            factorZ       = getZGain(range);
        }

        /**
         * \brief get the actual rate of the device
         * \return the rate of the device
         */
        [[nodiscard]] const Rate &getRate() const { return setting.rate; }

        /**
         * \brief get the actual gain of the device
         * \return the gain of the device
         */
        [[nodiscard]] const Range &getGain() const { return setting.range; }

        /*=========================================================================*/
    private:
        float factorXY = factor_1_3_XY; ///< scaling factor from raw input to gauss for X and Y
        float factorZ  = factor_1_3_Z;  ///< scaling factor from raw input to gauss for Z

        Setting setting; ///< settings of the device
        /**
         * \brief list of magnetometer registers
         */
        enum Registers {
            CRA_REG_M    = 0x00,
            CRB_REG_M    = 0x01,
            MR_REG_M     = 0x02,
            OUT_X_H_M    = 0x03,
            OUT_X_L_M    = 0x04,
            OUT_Z_H_M    = 0x05,
            OUT_Z_L_M    = 0x06,
            OUT_Y_H_M    = 0x07,
            OUT_Y_L_M    = 0x08,
            SR_REG_Mg    = 0x09,
            IRA_REG_M    = 0x0A,
            IRB_REG_M    = 0x0B,
            IRC_REG_M    = 0x0C,
            TEMP_OUT_H_M = 0x31,
            TEMP_OUT_L_M = 0x32
        };

        /**
         * \brief get the value of gain for X and Y at the specific range
         * \param range the range
         * \return teh gain value
         */
        static float getXYGain(Range range) {
            switch (range) {
            case Range::G1_3: return factor_1_3_XY;
            case Range::G1_9: return factor_1_9_XY;
            case Range::G2_5: return factor_2_5_XY;
            case Range::G4_0: return factor_4_0_XY;
            case Range::G4_7: return factor_4_7_XY;
            case Range::G5_6: return factor_5_6_XY;
            case Range::G8_1: return factor_8_1_XY;
            }
            return 0.F;
        }
        /**
         * \brief get the value of gain for Z at the specific range
         * \param range the range
         * \return teh gain value
         */
        static float getZGain(Range range) {
            switch (range) {
            case Range::G1_3: return factor_1_3_Z;
            case Range::G1_9: return factor_1_9_Z;
            case Range::G2_5: return factor_2_5_Z;
            case Range::G4_0: return factor_4_0_Z;
            case Range::G4_7: return factor_4_7_Z;
            case Range::G5_6: return factor_5_6_Z;
            case Range::G8_1: return factor_8_1_Z;
            }
            return 0.F;
        }

        /**
         * \brief read in the device for the magnetic data
         */
        void m_Measure() override {
            uint8_t status_reg_A = read8(SR_REG_Mg);
            if (status_reg_A == 0) // nothing has changed
                return;
            vec3s16 raw = readV16(OUT_X_H_M, true);
            _dta.x()    = static_cast<float>(raw.x()) * factorXY;
            _dta.y()    = static_cast<float>(raw.y()) * factorXY;
            _dta.z()    = static_cast<float>(raw.z()) * factorZ;
        }
    };
} // namespace lsm303
