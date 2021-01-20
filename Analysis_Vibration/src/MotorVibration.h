//
// Created by damien.lachouette on 30/12/2020.
//

#pragma once
#include <esc.h>
#include <imu10dof.h>
#include <pm06.h>
#include <vector>

constexpr uint16_t rc_low  = 982;
constexpr uint16_t rc_high = 2006;
constexpr uint8_t  rc_pin  = 5;

struct result {
    result() = default;
    result(uint32_t ts, math::vec3f a, pm06Measure p) : timestamp{ts}, acceleration{a}, voltages{p} {}
    uint64_t    timestamp{0};
    math::vec3f acceleration;
    pm06Measure voltages;
};

struct options {
    uint32_t wait          = 100;     // millisecond to wait between motor throttle and measures
    uint32_t motorThrottle = 40;      // motor throttle during test
    uint64_t testDuration  = 5000000; // test duration in µ seconds
};

class motorVibration {
public:
    explicit motorVibration(uint8_t rpin, uint16_t rlow = rc_low, uint16_t rhigh = rc_high);
    void                       begin();
    void                       measure(bool binary = false);

    void setTestDuration(uint32_t newDuration) { option.testDuration = newDuration; }
    void setInitialWait(uint32_t newWait) { option.wait = newWait; }
    void setMotorThrottle(uint32_t newThrottle) { option.motorThrottle = newThrottle; }

    uint64_t getTestDuration() { return option.testDuration; }
    uint32_t getInitialWait() { return option.wait; }
    uint32_t getMotorThrottle() { return option.motorThrottle; }

    void setAccelRange(uint8_t newRange) { accDevice.setRange(lsm303::Accel::Range(newRange)); }
    void setAccelResolution(uint8_t newResolution) { accDevice.setResolution(lsm303::Accel::Resolution(newResolution)); }
    int  getAccelRange() { return (int)accDevice.getRange(); }
    int  getAccelResolution() { return (int)accDevice.getResolution(); }

private:
    options       option{};
    lsm303::Accel accDevice;
    esc           motorDevice;
    pm06          powerModule;
};
