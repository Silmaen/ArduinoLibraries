/*
  Driver_AJ-SR04M - Library for arduino, for Driver_AJ-SR04M ultrasonic distance sensor.
  Created by Silmaen, September 25, 2020.
*/

#include "AJ-SR04M.h"

AJSR04M::AJSR04M(const int triggerPin, const int echoPin, const float maxDistance, const DistanceUnit& defaultUnit):
        _defaultUnit{defaultUnit}, _triggerPin{triggerPin}, _echoPin{echoPin},_maxDistance{maxDistance}, Cair{0}{
    pinMode(_triggerPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    setTemperatureK(288.15); // default value at 15°C
    digitalWrite(_triggerPin, LOW);
}

void AJSR04M::setTemperature(const float temp) {
    setTemperatureK(temp + 273.15);
}

void AJSR04M::setTemperatureK(const float temp) {
    switch (_defaultUnit) {
        case DistanceUnit::Centimeter:
            Cair = 0.001003;
            break;
        case DistanceUnit::Millimeter:
            Cair = 0.01003;
            break;
        case DistanceUnit::Inch:
            Cair = 0.00039488;
            break;
        case DistanceUnit::Foot:
            Cair = 0.0000329068;
            break;
        case DistanceUnit::Meter:
        default:
            Cair = 0.00001003;
            break;
    }
    Cair *= sqrt(temp);
}

unsigned long AJSR04M::ping()const{
    unsigned long timeout = _maxDistance / Cair;
    // Make sure that trigger pin is LOW.
    //digitalWrite(triggerPin, LOW);
    //delayMicroseconds(2);
    // Hold trigger for 10 microseconds, which is signal for sensor to measure distance.
    digitalWrite(_triggerPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(_triggerPin, LOW);
    // Measure the length of echo signal, which is equal to the time needed for sound to go there and back.
    unsigned long durationMicroSec = pulseIn(_echoPin, HIGH, timeout);
    return (durationMicroSec==0)?1:durationMicroSec;
}

double AJSR04M::measureDistance()const {
    unsigned long durationMicroSec = ping();
    if (durationMicroSec==0)
        return -1;
    return durationMicroSec * Cair;
}
