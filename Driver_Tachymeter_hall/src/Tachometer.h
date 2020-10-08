/*
  Driver_AJ-SR04M - Library for arduino, for Tachometer sensor.
  Created by Silmaen, September 27, 2020.
*/

#ifndef TACHOMETER_H
#define TACHOMETER_H

#include "Arduino.h"


/**
 * \brief list of possible unit for measure
 */
enum struct VelocityUnit {
    MeterPerSecond,    ///< S.I unit standard meter per second (default)
    KilometerPerHour,  ///< Commonly used speed unit Km/h = (1 km/h = 0.277778 * m/s)
    MilesPerHour,      ///< imperial unit Miles per hour ( 1 mph = 0.44704 m/s )
    Knots,             ///< Speed used in aviation ( 1 kts = 0.514444 m/s)
    FeetPerSecond,     ///< Feet per second, (1 Fps = 0.3048 m/s)
    FeetPerMinute,     ///< Feet per minute, (1 Fps = 0.00508 m/s)
};


void tick_interrupt();

/**
 * \brief class to handle sensor actions
 */
class Tachometer {
 public:
    /**
     * \brief Constructor
     * \param[in] triggerPin  Digital pin that is used for sensing.
     *                        as this pin is used with Interrupt, please double check that this pin is compatible for
     *                        your board as each board has different behavior with interrupts
     * \param[in] PulseLength the length between 2 pulse, used to compute velocity, for a wheel = Pi * diameter
     * \param[in] defaultUint the default unit system
     */
    Tachometer(const int triggerPin,const float PulseLength, const unsigned long long  timeout, const VelocityUnit& defaultUnit=VelocityUnit::MeterPerSecond);

    /**
     * \brief get the pulse frequency in Hz
     * \return the pulses frequency
     */
    float getFrequency()const;

    /**
     * \brief get the number of pulse per minute
     * \return pulses per minute
     */
    float getRpm()const;

    /**
     * \brief get the velocity based on the length
     * \return the velocity in the default unit
     */
    float getVelocity()const;

    /**
     * \brief get the velocity based on the length
     * \param unit the output velocity unit
     * \return the velocity is the desired unit
     */
    float getVelocity(const VelocityUnit& unit)const;

    int64_t getPeriod()const;
 private:
    VelocityUnit _defaultUnit; ///< default unit system
    float _PulseLength;  ///< length og a pulse
    uint64_t _timeout;
};

#endif // AJ_SR04M_H
