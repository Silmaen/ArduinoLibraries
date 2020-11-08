//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once
#include "composants/l3gd20.h"
#include "composants/lsm303.h"

struct bmp180Calibration {
    int16_t  ac1;
    int16_t  ac2;
    int16_t  ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t  b1;
    int16_t  b2;
    int16_t  mb;
    int16_t  mc;
    int16_t  md;
};

class imu10dof {
public:
    // construct, init
    imu10dof() = default;
    imu10dof(const lsm303::Accel::Setting &s_accel, const lsm303::Mag::Setting &s_mag, const l3gd20::Gyro::Setting &s_gyro) :
        AccSensor(s_accel), MagSensor(s_mag), GyrSensor(s_gyro) {}

    // measure & store
    const vec3f &measureAccel() { return AccSensor.measure(); }
    const vec3f &measureGyro() { return GyrSensor.measure(); }
    const vec3f &measureMagneto() { return MagSensor.measure(); }

    // getter
    [[nodiscard]] const vec3f &getAccel() const noexcept { return AccSensor.getMeasure(); };
    [[nodiscard]] const vec3f &getGyro() const noexcept { return GyrSensor.getMeasure(); };
    [[nodiscard]] const vec3f &getMagneto() const noexcept { return MagSensor.getMeasure(); };

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
    void setMode(const l3gd20::Gyro::Mode &mode) { GyrSensor.setMode(mode) }
    void setRate(const l3gd20::Gyro::RateBand &rate) { GyrSensor.setRate(rate); }
    void setRange(const l3gd20::Gyro::Range &range) { GyrSensor.setRange(range); }

    [[nodiscard]] const l3gd20::Gyro::RateBand &getRate() const { return GyrSensor.getRate(); }
    [[nodiscard]] const l3gd20::Gyro::Range &   getRange() const { return GyrSensor.getRange(); }
    [[nodiscard]] const l3gd20::Gyro::Mode &    getMode() const { return GyrSensor.getMode(); }
    //   barometer / thermometer

private:
    lsm303::Accel AccSensor;
    lsm303::Mag   MagSensor;
    l3gd20::Gyro  GyrSensor;
};