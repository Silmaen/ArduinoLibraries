//
// Created by Damien Lachouette on 29/12/2020.
//
#include "esc.h"

esc::esc(uint8_t pin, uint16_t outputMin, uint16_t outputMax, uint16_t armVal) :
    pin{pin}, oMin{outputMin}, oMax{outputMax}, oArm{armVal} {}


void esc::arm() {
    myEsc.attach(pin); // attaches the ESC on pin oPin to the ESC object
    myEsc.writeMicroseconds(oArm);
    oEsc = oArm;
    armed = true;
}

void esc::stop() {
    myEsc.writeMicroseconds(stopPulse);
    oEsc = stopPulse;
    armed = false;
}

void esc::output(uint16_t escVal) {
    if (! armed)
        return;
    oEsc = constrain(escVal, oMin, oMax);
    myEsc.writeMicroseconds(oEsc);
}

void esc::setMinMax(uint16_t outMin, uint16_t outMax, uint16_t arm){
    if (armed)
        stop();
    oMin = min(outMin, outMax);
    oMax = max(outMin, outMax);
    if (arm==0)
        oArm =oMin;
    else
        oArm = arm;
}

void esc::throttle(float percent) {
    if (! armed)
        return;
    oEsc = oMin + static_cast<uint16_t>(percent/100.F * static_cast<float>(oMax-oMin));
    myEsc.writeMicroseconds(oEsc);
}

float esc::getThrottle() const {
    if (!armed)
        return -1;
    return 100.F * static_cast<float>(oEsc-oMin)/static_cast<float>(oMax-oMin);
}
