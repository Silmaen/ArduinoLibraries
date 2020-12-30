//
// Created by damien.lachouette on 30/12/2020.
//
#include "MotorVibration.h"

motorVibration::motorVibration(uint8_t rpin, uint16_t rlow, uint16_t rhigh):
    motorDevice(rpin, rlow, rhigh, rlow){}

void motorVibration::begin() {
    accDevice.begin();
    accDevice.setResolution(lsm303::Accel::Resolution::HIGH_RES);
    accDevice.setRate(lsm303::Accel::Rate::HZ_ULTRA);
    accDevice.setRange(lsm303::Accel::Range::R_16G);
    motorDevice.arm();
}
void motorVibration::measure() {
    Serial.println("Clear Buffer");
    motorDevice.throttle(option.motorThrottle);
    delay(option.wait); // wait for the motor to accelerate before measure
    Serial.println("Begin Measure");
    uint64_t tstart = micros();
    uint64_t ts,i=0;
    do {
        ts = micros() - tstart;
        Serial.print(static_cast<int>(ts));
        Serial.print(" ");
        math::vec3f r = accDevice.measure_fast();
        Serial.print(r.x(),6);
        Serial.print(" ");
        Serial.print(r.y(),6);
        Serial.print(" ");
        Serial.print(r.z(),6);
        Serial.print(" ");
        pm06Measure v = powerModule.measure();
        Serial.print(v.V5,6);
        Serial.print(" ");
        Serial.print(v.Vbatt,6);
        Serial.print(" ");
        Serial.println(v.Ibatt,6);
    }while(ts < option.testDuration);
    motorDevice.throttle(0);
    Serial.println("End Measure");
}
