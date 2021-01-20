//
// Created by damien.lachouette on 30/12/2020.
//
#include "MotorVibration.h"

motorVibration::motorVibration(uint8_t rpin, uint16_t rlow, uint16_t rhigh) : motorDevice(rpin, rlow, rhigh, rlow) {}

void motorVibration::begin() {
    accDevice.begin();
    accDevice.setResolution(lsm303::Accel::Resolution::HIGH_RES);
    accDevice.setRate(lsm303::Accel::Rate::HZ_ULTRA);
    accDevice.setRange(lsm303::Accel::Range::R_16G);
    motorDevice.arm();
}
void motorVibration::measure(bool binary) {
    motorDevice.throttle(option.motorThrottle);
    delay(option.wait); // wait for the motor to accelerate before measure
    if (binary) {
        Serial.print("MESBIN ");
        Serial.print(sizeof(result));
        Serial.print("\n");
        result   r;
        uint64_t tstart = micros();
        do {
            r.timestamp    = micros() - tstart;
            r.acceleration = accDevice.measure_fast();
            r.voltages     = powerModule.measure();
            Serial.write((char *)&r, sizeof(r));
        } while (r.timestamp < option.testDuration);
        Serial.print("0000000000000000000000000000000000000000");
    } else {
        Serial.print("MESASC 4\n");
        uint64_t tstart = micros();
        uint64_t ts;
        do {
            ts = micros() - tstart;
            Serial.print(static_cast<int>(ts));
            Serial.print(" ");
            math::vec3f r = accDevice.measure_fast();
            Serial.print(r.x(), 4);
            Serial.print(" ");
            Serial.print(r.y(), 4);
            Serial.print(" ");
            Serial.print(r.z(), 4);
            Serial.print(" ");
            pm06Measure v = powerModule.measure();
            Serial.print(v.V5, 4);
            Serial.print(" ");
            Serial.print(v.Vbatt, 4);
            Serial.print(" ");
            Serial.println(v.Ibatt, 4);
        } while (ts < option.testDuration);
        Serial.print("RDY\n");
    }
    motorDevice.throttle(0);
}
