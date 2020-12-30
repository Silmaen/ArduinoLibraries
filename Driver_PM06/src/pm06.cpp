//
// Created by damien.lachouette on 30/12/2020.
//

#include "pm06.h"

pm06::pm06(uint8_t p5, uint8_t pV, uint8_t pI):
    pin5V{p5},pinVSense{pV},pinISense{pI} {
#ifdef ARDUINO_ARCH_SAMD
    analogReadResolution(12);
#endif
}

void pm06::setMeasured5V(float measured) {
    float raw = static_cast<float>(analogRead(pin5V)) * resolution;
    if (raw == 0) return;
    voltage5VDivider = measured/raw;
}
void pm06::setMeasuredV(float measured) {
    float raw = static_cast<float>(analogRead(pinVSense)) * resolution;
    if (raw == 0) return;
    voltageDivider = measured/raw;
}
void pm06::setMeasuredI(float measured) {
    float raw = static_cast<float>(analogRead(pinISense)) * resolution;
    if (raw == 0) return;
    currentDivider = measured/raw;
}
pm06Measure pm06::measure() const {
    pm06Measure res;
    res.V5 = static_cast<float>(analogRead(pin5V)) * resolution * voltage5VDivider;
    res.Vbatt = static_cast<float>(analogRead(pinVSense)) * resolution * voltageDivider;
    res.Ibatt = static_cast<float>(analogRead(pinISense)) * resolution * currentDivider;
    return res;
}
void pm06::setVoltage5VDivider(float measured) {
    voltage5VDivider = measured;
}
void pm06::setVoltageDivider(float measured) {
    voltageDivider = measured;
}
void pm06::setCurrentDivider(float measured) {
    currentDivider = measured;
}
