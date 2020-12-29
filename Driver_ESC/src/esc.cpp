//
// Created by Damien Lachouette on 29/12/2020.
//
#include "esc.h"

Esc::Esc(uint8_t pin, uint16_t outputMin, uint16_t outputMax, uint16_t armVal) :
    pin{pin}, oMin{outputMin}, oMax{outputMax}, oArm{armVal} {}
Esc::~Esc() = default;

void Esc::arm() {
    myESC.attach(pin); // attaches the ESC on pin oPin to the ESC object
    myESC.writeMicroseconds(oArm);
    oEsc = oArm;
    armed = true;
}

void Esc::stop() {
    myESC.writeMicroseconds(stopPulse);
    oEsc = stopPulse;
    armed = false;
}

void Esc::output(uint16_t escVal) {
    if (! armed)
        return;
    oEsc = constrain(escVal, oMin, oMax);
    myESC.writeMicroseconds(oEsc);
}

void Esc::setMinMax(uint16_t outMin, uint16_t outMax){
    if (armed)
        stop();
    oMin = min(outMin, outMax);
    oMax = max(outMin, outMax);
}

void Esc::throttle(float percent) {
    if (! armed)
        return;
    oEsc = oMin + static_cast<uint16_t>(percent/100.F * static_cast<float>(oMax-oMin));
    myESC.writeMicroseconds(oEsc);
}

float Esc::getThrottle() const {
    if (!armed)
        return -1;
    return 100.F * static_cast<float>(oEsc-oMin)/static_cast<float>(oMax-oMin);
}
