//
// Created by damien.lachouette on 29/12/2020.
//

#pragma once
#include <Arduino.h>
#include <Servo.h>

constexpr uint16_t defaultOutputMin        = 1000U;
constexpr uint16_t defaultOutputMax        = 2000U;
constexpr uint16_t defaultOutputArming     = 500U;
constexpr uint16_t defaultStopPulse        = 500U;

class esc {
public:
    explicit esc(uint8_t  pin,
                 uint16_t outputMin = defaultOutputMin,
                 uint16_t outputMax = defaultOutputMax,
                 uint16_t armVal    = defaultOutputArming);

    /**
     * \brief Sent a signal to Arm the ESC
     * depends on the Arming value from the constructor
     */
    void arm();

    /**
     * Sent a signal to stop the ESC
     * depends on the ESC stop pulse value
     */
    void stop();

    /**
     * \brief Sent a signal to set the ESC speed
     * value is clamped by minimum and maximum values
     * \param[in] escVal new value to apply
     */
    void output(uint16_t escVal);

    /**
     * \brief get the value of the actual output
     * \return the actual output
     */
    const uint16_t& getOutSignal()const {return oEsc; }

    /**
     * \brief Set the current Stop pulse in microseconds
     * \param[in] pulse send a stop pulse
     */
    void setStopPulse(uint16_t pulse) { stopPulse = pulse; }

    /**
     * \brief Set the current Stop pulse in microseconds
     * \param[in] min the min value
     * \param[in] max  the max value
     * \param[in] arm  the max value
     */
    void setMinMax(uint16_t min, uint16_t max, uint16_t arm=0);

    /**
     * \brief set the throttle as a percent
     * \param percent throttle percent
     */
    void throttle(float percent);

    /**
     * \brief get the actual output value as a percent
     * \return the output throttle
     */
    float getThrottle()const;
private:
    uint8_t pin;
    // Calibration
    uint16_t oMin             = defaultOutputMin;
    uint16_t oMax             = defaultOutputMax;
    uint16_t oEsc             = defaultOutputMin;
    uint16_t oArm             = defaultOutputArming;
    uint16_t stopPulse        = defaultStopPulse;        // Stop pulse (microseconds)
    bool armed = false;
    Servo    myEsc;                                      // create servo object to control an ESC
};
