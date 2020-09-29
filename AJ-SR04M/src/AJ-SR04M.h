/*
  AJ-SR04M - Library for arduino, for AJ-SR04M ultrasonic distance sensor.
  Created by Silmaen, September 25, 2020.
*/

#ifndef AJ_SR04M_H
#define AJ_SR04M_H

#include "Arduino.h"

/**
 * \brief list of possible unit for measure
 */
enum struct DistanceUnit {
    Meter,     ///< S.I unit standard meter (default)
    Centimeter,///< Centimeter = 0.01 meter
    Millimeter,///< Millimeter = 0.001 meter
    Inch,      ///< Imperial unit inch = 0.0254 meter
    Foot,      ///< Imperial foot unit = 0.3048
};

/**
 * \brief class to handle sensor actions
 */
class AJSR04M {
 public:
    /**
     * \brief Constructor
     * \param[in] triggerPin  Digital pin that is used for controlling sensor (output).
     * \param[in] echoPin  Digital pin that is used to get information from sensor (input).
     * \param[in] maxDistance maximum distance range don't wait for a ping longer (given in the default unit system)
     * \param[in] defaultUint the default unit system
     */
    AJSR04M(const int triggerPin, const int echoPin, const float maxDistance = 4.5, const DistanceUnit& defaultUnit=DistanceUnit::Meter);

    /**
     * \brief compute the wave speed in dry air based on temperature
     * \param[in] temp the temperature in °C
     */
    void setTemperature(const float temp);

    /**
     * \brief compute the wave speed in dry air based on temperature
     * \param[in] temp the temperature in °K
     */
    void setTemperatureK(const float temp);

    /**
     * \brief just ping and get the  timing response in microsecond
     * \return the ping duration in microsecond, 0 if timeout
     */
    unsigned long ping()const;

    /**
     * \brief Measures distance by sending ultrasonic waves and measuring time it takes them
     * to return.
     * \return Distance in _defaultUnit, or negative value if distance is greater than _maxDistance.
     */
    double measureDistance()const;

 private:
    DistanceUnit _defaultUnit; ///< default unit system
    int _triggerPin;    ///< the trigger pin
    int _echoPin;       ///< the echo pin
    float _maxDistance; ///< the maximum sensor range (in _defaultUnit)
    float Cair;         ///< the sound velocity (in _defaultUnit/µs)
};

#endif // AJ_SR04M_H
