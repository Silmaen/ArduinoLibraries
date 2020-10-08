/*
  Driver_AJ-SR04M - Library for arduino, for Tachometer sensor.
  Created by Silmaen, September 27, 2020.
*/

#include "Tachometer.h"

uint64_t lastPulse;
uint64_t prevPulse;
void tick_interrupt(){
    prevPulse = lastPulse;
    lastPulse = micros();
}

Tachometer::Tachometer(const int triggerPin,const float PulseLength, const unsigned long long timeout, const VelocityUnit& defaultUnit):
_defaultUnit{defaultUnit}, _PulseLength{PulseLength}, _timeout(timeout){
    pinMode(triggerPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(triggerPin), tick_interrupt, RISING);//Initialize the intterrupt pin
    prevPulse = micros();
    lastPulse = micros();
}

int64_t Tachometer::getPeriod()const{
    uint64_t v = micros() - lastPulse;
    if (v > _timeout) {
        return -1;
    }
    return lastPulse - prevPulse;
}

float Tachometer::getFrequency()const{
    int64_t p = getPeriod();
    if (p<=0) return 0.f;
    return 1e6 / p;
}

float Tachometer::getRpm()const{
    return 60.f * getFrequency();
}

float Tachometer::getVelocity()const{
    return getVelocity(_defaultUnit);
}

float Tachometer::getVelocity(const VelocityUnit& unit)const{
    int64_t period = getPeriod();
    if (period <= 0) return 0.f;
    double fperiod = period / 1.e6;
    double length = _PulseLength;
    switch (_defaultUnit) {
        case VelocityUnit::KilometerPerHour:
            length *= 1000.;
            break;
        case VelocityUnit::MilesPerHour:
            length *= 1609.345f;
            break;
        case VelocityUnit::Knots:
            length *= 1852.;
            break;
        case VelocityUnit::FeetPerSecond:
            length *= 0.0254f;
            break;
        case VelocityUnit::FeetPerMinute:
            length *= 0.0254;
            break;
        default:
            break;
    }
    float velocitySI = length / fperiod;
    switch(unit){
        case VelocityUnit::KilometerPerHour:
            velocitySI *= 3.6;
            break;
        case VelocityUnit::MilesPerHour:
            velocitySI *= 2.23694;
            break;
        case VelocityUnit::Knots:
            velocitySI *= 1.94384;
            break;
        case VelocityUnit::FeetPerSecond:
            velocitySI *= 3.28084;
            break;
        case VelocityUnit::FeetPerMinute:
            velocitySI *= 196.8504;
            break;
        default:
            break;
    }
    return velocitySI;
}