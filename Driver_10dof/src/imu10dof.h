//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once

#define LSM303_ACCEL
#define LSM303_MAG
#define L3GD20
#define BMP085

#include "composants/bmp085.h"
#include "composants/l3gd20.h"
#include "composants/lsm303.h"

class imu10dof {
public:
    struct measure_data {
        vec3f accel;
        vec3f gyro;
        vec3f magento;
        float pressure;
        float temperature;
    };

    // construct, init
    imu10dof() = default;
    imu10dof(const lsm303::Accel::Setting &s_accel,
             const lsm303::Mag::Setting &  s_mag,
             const l3gd20::Gyro::Setting & s_gyro,
             const bmp085::Baro::Setting & s_baro) :
        AccSensor(s_accel),
        MagSensor(s_mag), GyrSensor(s_gyro), BarSensor(s_baro) {}

    bool begin() {
        return AccSensor.begin() &&
        MagSensor.begin() &&
        GyrSensor.begin() &&
        BarSensor.begin();
    }

    void checks(){
        Serial.print("Accelerometer: ");
        if (AccSensor.get_presence()) {
            Serial.println(" [  OK  ]");
        }else{
            Serial.println(" [ FAIL ]");
        }
        AccSensor.print_config();
        Serial.print("Magnetometer : ");
        if (MagSensor.get_presence()) {
            Serial.println(" [  OK  ]");
        }else{
            Serial.println(" [ FAIL ]");
        }
        MagSensor.print_config();
        Serial.print("Gyroscope    : ");
        if (GyrSensor.get_presence()) {
            Serial.println(" [  OK  ]");
        }else{
            Serial.println(" [ FAIL ]");
        }
        GyrSensor.print_config();
        Serial.print("Barometer    : ");
        if (BarSensor.get_presence()) {
            Serial.println(" [  OK  ]");
        }else{
            Serial.println(" [ FAIL ]");
        }
        BarSensor.print_config();
    }

    // measure, store return the value
    [[nodiscard]] const vec3f &measureAccel() { return AccSensor.measure(); }
    [[nodiscard]] const vec3f &measureGyro() { return GyrSensor.measure(); }
    [[nodiscard]] const vec3f &measureMagneto() { return MagSensor.measure(); }
    [[nodiscard]] const float &measureBaro() { return BarSensor.measure(); }

    /**
     * @brief get all the data at once
     * @return the data object
     */
    [[nodiscard]] measure_data measureAll() {
        measure_data data;
        data.accel       = measureAccel();
        data.gyro        = measureGyro();
        data.magento     = measureMagneto();
        data.pressure    = measureBaro();
        data.temperature = getTemerature();
        return data;
    }

    // only return the actual stored value
    [[nodiscard]] const vec3f &getAccel() const noexcept { return AccSensor.getMeasure(); };
    [[nodiscard]] const vec3f &getGyro() const noexcept { return GyrSensor.getMeasure(); };
    [[nodiscard]] const vec3f &getMagneto() const noexcept { return MagSensor.getMeasure(); };
    [[nodiscard]] const float &getBaro() const noexcept { return BarSensor.getMeasure(); }
    [[nodiscard]] float getTemerature() const noexcept { return BarSensor.getTemperature(); }

    /**
     * @brief get all the data at once
     * @return the data object
     */
    [[nodiscard]] measure_data getAll() const noexcept {
        measure_data data;
        data.accel       = getAccel();
        data.gyro        = getGyro();
        data.magento     = getMagneto();
        data.pressure    = getBaro();
        data.temperature = getTemerature();
        return data;
    }

    // settings
    //   accelerometer
    void setAccelRate(lsm303::Accel::Rate rate) { AccSensor.setRate(rate); }
    void setAccelMode(lsm303::Accel::Mode mode) { AccSensor.setMode(mode); }
    void setAccelRange(lsm303::Accel::Range range) { AccSensor.setRange(range); }
    void setAccelResolution(lsm303::Accel::Resolution hiRes) { AccSensor.setResolution(hiRes); }

    [[nodiscard]] const lsm303::Accel::Rate &      getAccelRate() const { return AccSensor.getRate(); }
    [[nodiscard]] const lsm303::Accel::Mode &      getAccelMode() const { return AccSensor.getMode(); }
    [[nodiscard]] const lsm303::Accel::Range &     getAccelRange() const { return AccSensor.getRange(); }
    [[nodiscard]] const lsm303::Accel::Resolution &getAccelResolution() const { return AccSensor.getResolution(); }

    //   magnetometer
    void setMagRate(lsm303::Mag::Rate rate) { MagSensor.setRate(rate); }
    void setMagRange(lsm303::Mag::Range range) { MagSensor.setRange(range); }

    [[nodiscard]] const lsm303::Mag::Rate & getMagRate() const { return MagSensor.getRate(); }
    [[nodiscard]] const lsm303::Mag::Range &getMagGain() const { return MagSensor.getGain(); }

    //   gyroscope
    void setGyroMode(const l3gd20::Gyro::Mode &mode) { GyrSensor.setMode(mode); }
    void setGyroRate(const l3gd20::Gyro::RateBand &rate) { GyrSensor.setRate(rate); }
    void setGyroRange(const l3gd20::Gyro::Range &range) { GyrSensor.setRange(range); }

    [[nodiscard]] const l3gd20::Gyro::RateBand &getGyroRate() const { return GyrSensor.getRate(); }
    [[nodiscard]] const l3gd20::Gyro::Range &   getGyroRange() const { return GyrSensor.getRange(); }
    [[nodiscard]] const l3gd20::Gyro::Mode &    getGyroMode() const { return GyrSensor.getMode(); }

    //   barometer / thermometer
    void setBaroMode(const bmp085::Baro::Mode &mode) { BarSensor.setMode(mode); }

    [[nodiscard]] const bmp085::Baro::Mode &getBaroMode() const { return BarSensor.getMode(); }

private:
    lsm303::Accel AccSensor;
    lsm303::Mag   MagSensor;
    l3gd20::Gyro  GyrSensor;
    bmp085::Baro  BarSensor;
};