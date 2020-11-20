//
// Created by damien.lachouette on 05/11/2020.
//

#pragma once

#define LSM303_ACCEL
//#define LSM303_MAG

#include "composants/bmp085.h"
#include "composants/l3gd20.h"
#include "composants/lsm303.h"

class imu10dof {
public:
    // construct, init
    imu10dof() = default;
#if (defined(LSM303_ACCEL) || defined(LSM303_MAG) || defined(L3GD20) || defined(BMP085))
    imu10dof(
#    ifdef LSM303_ACCEL
      const lsm303::Accel::Setting &s_accel
#    endif
#    ifdef LSM303_MAG
#        ifdef LSM303_ACCEL
      ,
#        endif
      const lsm303::Mag::Setting &s_mag
#    endif
#    ifdef L3GD20
#        if (defined(LSM303_ACCEL) || defined(LSM303_MAG))
      ,
#        endif
      const l3gd20::Gyro::Setting &s_gyro
#    endif
#    ifdef BMP085
#        if (defined(LSM303_ACCEL) || defined(LSM303_MAG) || defined(L3GD20))
      ,
#        endif
      const bmp085::Baro::Setting &s_baro
#    endif
      ) :
#    ifdef LSM303_ACCEL
        AccSensor(s_accel)
#    endif
#    ifdef LSM303_MAG
#        ifdef LSM303_ACCEL
        ,
#        endif
        MagSensor(s_mag)
#    endif
#    ifdef L3GD20
#        if (defined(LSM303_ACCEL) || defined(LSM303_MAG))
        ,
#        endif
        GyrSensor(s_gyro)
#    endif
#    ifdef BMP085
#        if (defined(LSM303_ACCEL) || defined(LSM303_MAG) || defined(L3GD20))
        ,
#        endif
        BarSensor(s_baro)
#    endif
    {
    }
#endif

    void begin() {
#ifdef LSM303_ACCEL
        AccSensor.begin();
#endif
#ifdef LSM303_MAG
        MagSensor.begin();
#endif
#ifdef L3GD20
        GyrSensor.begin();
#endif
#ifdef BMP085
        BarSensor.begin();
#endif
    }

    // measure, store return the value
#ifdef LSM303_ACCEL
    [[nodiscard]] const vec3f &measureAccel() { return AccSensor.measure(); }
#endif
#ifdef L3GD20
    [[nodiscard]] const vec3f &measureGyro() { return GyrSensor.measure(); }
#endif
#ifdef LSM303_MAG
    [[nodiscard]] const vec3f &measureMagneto() { return MagSensor.measure(); }
#endif
#ifdef BMP085
    [[nodiscard]] const float &measureBaro() { return BarSensor.measure(); }
#endif

    // only return the actual stored value
#ifdef LSM303_ACCEL
    [[nodiscard]] const vec3f &getAccel() const noexcept { return AccSensor.getMeasure(); };
#endif
#ifdef L3GD20
    [[nodiscard]] const vec3f &getGyro() const noexcept { return GyrSensor.getMeasure(); };
#endif
#ifdef LSM303_MAG
    [[nodiscard]] const vec3f &getMagneto() const noexcept { return MagSensor.getMeasure(); };
#endif
#ifdef BMP085
    [[nodiscard]] const float &getBaro() { return BarSensor.getMeasure(); }
#endif

    // settings
#ifdef LSM303_ACCEL
    //   accelerometer
    void setAccelRate(lsm303::Accel::Rate rate) { AccSensor.setRate(rate); }
    void setAccelMode(lsm303::Accel::Mode mode) { AccSensor.setMode(mode); }
    void setAccelRange(lsm303::Accel::Range range) { AccSensor.setRange(range); }
    void setAccelResolution(lsm303::Accel::Resolution hiRes) { AccSensor.setResolution(hiRes); }

    [[nodiscard]] const lsm303::Accel::Rate &      getAccelRate() const { return AccSensor.getRate(); }
    [[nodiscard]] const lsm303::Accel::Mode &      getAccelMode() const { return AccSensor.getMode(); }
    [[nodiscard]] const lsm303::Accel::Range &     getAccelRange() const { return AccSensor.getRange(); }
    [[nodiscard]] const lsm303::Accel::Resolution &getAccelResolution() const { return AccSensor.getResolution(); }
#endif

#ifdef LSM303_MAG
    //   magnetometer
    void setMagRate(lsm303::Mag::Rate rate) { MagSensor.setRate(rate); }
    void setMagRange(lsm303::Mag::Range range) { MagSensor.setRange(range); }

    [[nodiscard]] const lsm303::Mag::Rate & getMagRate() const { return MagSensor.getRate(); }
    [[nodiscard]] const lsm303::Mag::Range &getMagGain() const { return MagSensor.getGain(); }
#endif

#ifdef L3GD20
    //   gyroscope
    void setGyroMode(const l3gd20::Gyro::Mode &mode) { GyrSensor.setMode(mode); }
    void setGyroRate(const l3gd20::Gyro::RateBand &rate) { GyrSensor.setRate(rate); }
    void setGyroRange(const l3gd20::Gyro::Range &range) { GyrSensor.setRange(range); }

    [[nodiscard]] const l3gd20::Gyro::RateBand &getGyroRate() const { return GyrSensor.getRate(); }
    [[nodiscard]] const l3gd20::Gyro::Range &   getGyroRange() const { return GyrSensor.getRange(); }
    [[nodiscard]] const l3gd20::Gyro::Mode &    getGyroMode() const { return GyrSensor.getMode(); }
#endif

#ifdef BMP085
    //   barometer / thermometer
    void setBaroMode(const bmp085::Baro::Mode &mode) { BarSensor.setMode(mode); }

    [[nodiscard]] const bmp085::Baro::Mode &getBaroMode() const { return BarSensor.getMode(); }
#endif

private:
#ifdef LSM303_ACCEL
    lsm303::Accel AccSensor;
#endif
#ifdef LSM303_MAG
    lsm303::Mag MagSensor;
#endif
#ifdef L3GD20
    l3gd20::Gyro GyrSensor;
#endif
#ifdef BMP085
    bmp085::Baro BarSensor;
#endif
};