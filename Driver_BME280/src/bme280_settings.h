//
// Created by damien.lachouette on 26/11/2020.
//

#pragma once
#include <Arduino.h>

namespace bme280 {

    /**
     * @brief the internal modes of work
     */
    enum struct WorkingMode {
        Sleep  = 0b00, ///< no operation, all register accessible, lowest power, startup mode
        Forced = 0b01, ///< perform 1 measurement, store results, then sleep (wake up only on demand)
        Normal = 0b11, ///< perpetual cycling of measurement and inactive period (set by StandyByTime)
    };

    /**
     * @brief oversampling possibilities
     *
     * the oversampling is set for each sensor. the values are the same.
     */
    enum struct Oversampling {
        Off   = 0,     ///< sensor deactivated
        O_X1  = 0b001, ///< one sample
        O_X2  = 0b010, ///< 2 sample
        O_X4  = 0b011, ///< 4 sample
        O_X8  = 0b100, ///< 8 sample
        O_X16 = 0b101, ///< 16 sample
    };

    /**
     * @brief filtering options
     */
    enum struct FilterCoefficient {
        Off  = 0b000, ///< filter off
        F_2  = 0b001, ///< filter coefficient 2
        F_4  = 0b010, ///< filter coefficient 4
        F_8  = 0b011, ///< filter coefficient 8
        F_16 = 0b100, ///< filter coefficient 16
    };

    /**
     * @brief the stand by time between two measure
     */
    enum struct StandyByTime {
        SBT_0_5  = 0b000, ///< 0.5 ms
        SBT_62_5 = 0b001, ///< 62.5 ms
        SBT_125  = 0b010, ///< 125 ms
        SBT_250  = 0b011, ///< 250 ms
        SBT_500  = 0b100, ///< 500 ms
        SBT_100  = 0b101, ///< 100 ms
        SBT_10   = 0b110, ///< 10 ms
        SBT_20   = 0b111, ///< 20 ms
    };

    /**
     * \brief device settings
     */
    struct Setting {
        WorkingMode       mode                    = WorkingMode::Sleep;     ///< operating mode
        Oversampling      pressureOversampling    = Oversampling::Off;      ///< oversampling for pressure
        Oversampling      temperatureOversampling = Oversampling::Off;      ///< oversampling for temperature
        Oversampling      humidityOversampling    = Oversampling::Off;      ///< oversampling for humidity
        StandyByTime      sdTime                  = StandyByTime::SBT_0_5;  ///< stand by time
        FilterCoefficient filter                  = FilterCoefficient::Off; ///< filter coefficient

        constexpr Setting(const WorkingMode &      m,
                          const Oversampling &     po,
                          const Oversampling &     to,
                          const Oversampling &     ho,
                          const StandyByTime &     sdt,
                          const FilterCoefficient &fc) :
            mode{m},
            pressureOversampling{po}, temperatureOversampling{to}, humidityOversampling{ho}, sdTime{sdt}, filter{fc} {}

        uint8_t toCtrlHumReg() { return static_cast<uint8_t>(humidityOversampling); }
        uint8_t toCtrlMeasReg() {
            return (static_cast<uint8_t>(temperatureOversampling) << 5u) + (static_cast<uint8_t>(pressureOversampling) << 2u) +
                   static_cast<uint8_t>(mode);
        }
        uint8_t  toConfigReg() { return (static_cast<uint8_t>(sdTime) << 5u) + (static_cast<uint8_t>(filter) << 2u); }
        void     print() {}
        uint16_t maxMeasurementTime() {
            float estimation = 1.25f + (2.3f * static_cast<float>(temperatureOversampling));
            if (pressureOversampling != Oversampling::Off)
                estimation += (2.3f * static_cast<float>(pressureOversampling) + 0.575f);
            if (humidityOversampling != Oversampling::Off)
                estimation += (2.3f * static_cast<float>(humidityOversampling) + 0.575f);
            return estimation + 0.5f;
        }
    };

    // PREDIFINED SETTINGS as mention in the datasheet

    /**
     * @brief Weather monitoring set of parameters
     *
     *  Only 1 measure/minute is recommended
     *  Current consumption: 0.16µA
     *  RMS Noise: 3.3Pa/30cm, 0.07%RH
     *  Data output rate: 1/60Hz
     */
    constexpr Setting weatherMonitoring{WorkingMode::Forced, Oversampling::O_X1,    Oversampling::O_X1,
                                        Oversampling::O_X1,  StandyByTime::SBT_0_5, FilterCoefficient::Off};

    /**
     * @brief Humidity sensing set of parameters
     *
     *  Only 1 measure/second is recommended, presure measurment is deactivated
     *  Current consumption: 2.9µA
     *  RMS Noise: 0.07%RH
     *  Data output rate: 1Hz
     */
    constexpr Setting humiditySensing{WorkingMode::Forced, Oversampling::Off,     Oversampling::O_X1,
                                      Oversampling::O_X1,  StandyByTime::SBT_0_5, FilterCoefficient::Off};

    /**
     * @brief Indoor navigation set of parameters
     *
     *  Current consumption: 633µA
     *  RMS Noise: 0.2Pa/1.7cm
     *  Data output rate: 25Hz
     *  Filter bandwidth: 0.53Hz
     *  Response time (75%): 0.9s
     */
    constexpr Setting indoorNavigation{WorkingMode::Normal, Oversampling::O_X16,   Oversampling::O_X2,
                                       Oversampling::O_X1,  StandyByTime::SBT_0_5, FilterCoefficient::F_16};

    /**
     * @brief Gaming set of parameters
     *
     *  Humidity sensor is deactivated
     *  Current consumption: 581
     *  RMS Noise: 0.3Pa/2.5cm
     *  Data output rate: 83Hz
     *  Filter bandwidth: 1.75Hz
     *  Response time (75%): 0.3s
     */
    constexpr Setting gaming{WorkingMode::Normal, Oversampling::O_X4,    Oversampling::O_X1,
                             Oversampling::Off,   StandyByTime::SBT_0_5, FilterCoefficient::F_16};
} // namespace bme280
