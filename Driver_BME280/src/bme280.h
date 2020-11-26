//
// Created by damien.lachouette on 26/11/2020.
//

#pragma once
#include "bme280_settings.h"
#include "i2c_utils.h"

namespace bme280 {

    constexpr uint8_t deviceAddress = 0x76; //0b111011X; X = 0 case set to GND... TODO: check it
    constexpr uint8_t chipId = 0x60;
    constexpr uint8_t resetCode = 0x56;

    struct SensorData{
        float Temperature;
        float Humidity;
        float Pressure;
    };

    class Device:public i2c::SensorDevice<deviceAddress,SensorData> {
    public:

        /**
         * \brief default constructor
         */
        Device() = default;

        /**
         * \brief constructor based on settings
         * \param settings the initialization settings
         */
        explicit Device(const Setting &settings) : setting{settings} {}

        /**
         * @brief initialize the device
         * @return true if everything is ok
         */
        bool begin() override {
            if (!i2c::SensorDevice<deviceAddress, SensorData>::begin())
                return false;
            readCalibration();
            applySetting();
            return true;
        }

        /**
         * \brief measure the temperature of the device
         * \return the measured temperature (the absolute 0 if not available)
         */
        [[nodiscard]] float getTemperature() const override { return data().Temperature; }

        /**
         * @brief get a copy of actual settings
         * @return the actual settings
         */
        [[nodiscard]] Setting getSetting()const{return setting;}

        /**
         * @brief get a copy of actual settings
         * @return the actual settings
         */
        void setSetting(const Setting& set){setting=set;applySetting();}

        /**
         * @brief access to the actual settings
         * @return the actual settings
         */
        [[nodiscard]] Setting& getSetting(){return setting;}

        /**
         * @brief write setting into the registers
         */
        void applySetting(){
            writeCommand(R_CTRL_HUM, setting.toCtrlHumReg());
            writeCommand(R_CONFIG, setting.toConfigReg());
            writeCommand(R_CTRL_MEAS, setting.toCtrlMeasReg());
        }

        /**
         * @brief display in serial port the config of the device
         */
        void print_config() override {
            setting.print();
        }

    private:
        Setting setting; ///< settings of the device
        /**
         * @brief definition of registers constants
         */
        enum Registers {
            // config
            R_ID        = 0xD0, ///< Chip ID
            R_RESET     = 0xE0, ///< soft reset
            R_CTRL_HUM  = 0xF2, ///< Humidity options register
            R_STATUS    = 0xF3, ///< Status of the device bit 0 : im_update (1 copying, 0 ready) bit 3 measuring (0: ready)
            R_CTRL_MEAS = 0xF4, ///< pressure/temp (need to be set after ctrl_hum)
            R_CONFIG    = 0xF5, ///< set rates, filter & interface
            // measure data
            R_PRESS_MSB  = 0xF7, ///< pressure data = `(MSB << 16 + LSB << 8 + XLSB) >> 4`
            R_PRESS_LSB  = 0xF8, ///< pressure data = `(MSB << 16 + LSB << 8 + XLSB) >> 4`
            R_PRESS_XLSB = 0xF9, ///< pressure data = `(MSB << 16 + LSB << 8 + XLSB) >> 4`
            R_TEMP_MSB   = 0xFA, ///< temperature data = `(MSB << 16 + LSB << 8 + XLSB) >> 4`
            R_TEMP_LSB   = 0xFB, ///< temperature data = `(MSB << 16 + LSB << 8 + XLSB) >> 4`
            R_TEMP_XLSB  = 0xFC, ///< temperature data = `(MSB << 16 + LSB << 8 + XLSB) >> 4`
            R_HUM_MSB    = 0xFD, ///< humidity data = `MSB << 8 + LSB`
            R_HUM_LSB    = 0xFE, ///< humidity data = `MSB << 8 + LSB`
            // calibration data
            R_T1_LSB = 0x88, ///< T1 coefficient LSB-first
            R_T2_LSB = 0x8A, ///< T2 coefficient LSB-first
            R_T3_LSB = 0x8C, ///< T3 coefficient LSB-first
            R_P1_LSB = 0x8E, ///< P1 coefficient LSB-first
            R_P2_LSB = 0x90, ///< P2 coefficient LSB-first
            R_P3_LSB = 0x92, ///< P3 coefficient LSB-first
            R_P4_LSB = 0x94, ///< P4 coefficient LSB-first
            R_P5_LSB = 0x96, ///< P5 coefficient LSB-first
            R_P6_LSB = 0x98, ///< P6 coefficient LSB-first
            R_P7_LSB = 0x9A, ///< P7 coefficient LSB-first
            R_P8_LSB = 0x9C, ///< P8 coefficient LSB-first
            R_P9_LSB = 0x9E, ///< P9 coefficient LSB-first
            R_H1     = 0xA1, ///< H1 coefficient
            R_H2_LSB = 0xE1, ///< H2 coefficient LSB-first
            R_H3     = 0xE3, ///< H3
            R_H4_MSB = 0xE4, ///< H4 must bust left-shited by 4
            R_H4_H5  = 0xE5, ///< H4 the firsts 4 bits are the first four bits of H4, the last 4bit are the LSB for H5
            R_H5_MSB = 0xE6, ///< H5 must bust left-shited by 4
            R_H6     = 0xE7, ///< H6
        };

        /**
         * @brief check the presence of the device
         * @return return true if the device is found
         */
        bool is_device_present() override {
            presence() = false;
            if (read8(Registers::R_ID) == chipId) {
                presence() = true;
            }
            return presence();
        }

        /**
         * @brief structure for handling calibraton data
         */
        struct CalibrationData {
            uint16_t T1 = 0;
            int16_t  T2 = 0;
            int16_t  T3 = 0;
            uint16_t P1 = 0;
            int16_t  P2 = 0;
            int16_t  P3 = 0;
            int16_t  P4 = 0;
            int16_t  P5 = 0;
            int16_t  P6 = 0;
            int16_t  P7 = 0;
            int16_t  P8 = 0;
            int16_t  P9 = 0;
            uint8_t  H1 = 0;
            int16_t  H2 = 0;
            uint8_t  H3 = 0;
            int16_t  H4 = 0;
            int16_t  H5 = 0;
            int8_t   H6 = 0;
        };

        /**
         * \brief read the device for measure data
         */
        void m_Measure() override {
            if (!presence()) {
                Serial.println("No BMP280 device");
                return;
            }
            if (setting.mode==WorkingMode::Forced){ // device need to be waked up
                writeCommand(R_CTRL_MEAS, setting.toCtrlMeasReg());
                delay(setting.maxMeasurementTime());
            }
            uint8_t status_reg = read8(R_STATUS);
            if (status_reg != 0) {// device is still busy
                Serial.println("BME280 is not yet ready");
                return;
            }
            readTemperature();
            readPressure();
            readHumidity();
        }

        CalibrationData cal; ///< storage of calibration data
        int32_t fine_t = 0; ///< temp variable to transmit temp info to pressure

        /**
         * \brief read & store calibration data
         */
        void readCalibration();

        /**
         * @brief read and compensate temperature
         */
        void readTemperature();

        /**
         * @brief read and compensate pressure (need to be called after readTemperature)
         */
        void readPressure();

        /**
         * @brief read and compensate humidity (need to be called after readTemperature)
         */
        void readHumidity();

        /**
         * \brief raw read of temperature
         * \return
         */
        [[nodiscard]] int32_t readUncompensatedTemperature() const noexcept{
            return read24(R_TEMP_MSB)>> semi_byte_shift;
        }

        /**
         * \brief raw read of pressure
         * \return
         */
        [[nodiscard]] int32_t readUncompensatedPressure() const noexcept {
            return read24(R_PRESS_MSB)>> semi_byte_shift;
        }

        /**
         * \brief raw read of humidity
         * \return
         */
        [[nodiscard]] uint16_t readUncompensatedHumidity() const noexcept {
            return read16(R_HUM_MSB);
        }
    };

}  // namespace bme280
