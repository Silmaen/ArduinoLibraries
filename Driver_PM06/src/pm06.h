//
// Created by damien.lachouette on 30/12/2020.
//

#pragma once
#include <Arduino.h>

constexpr float defaultVoltageDivider = 17.8304806F;
constexpr float defaultCurrentDivider = 36.F;
constexpr float defaultVoltage5VDivider = 2.0004811F;

#ifdef ARDUINO_ARCH_SAMD
constexpr float resolution = 3.27 / 4095.;
#else
constexpr float resolution = 5.0 / 1023.;
#endif

struct pm06Measure {
    explicit pm06Measure(float v5=0,float vb=0,float ib=0):V5{v5},Vbatt{vb},Ibatt(ib){}
    float V5 = 0;
    float Vbatt = 0;
    float Ibatt = 0;
};

class pm06 {
public:
    explicit pm06(uint8_t p5 = A1, uint8_t pV = A2, uint8_t pI = A3);
    void setMeasured5V(float measured);
    void setMeasuredV(float measured);
    void setMeasuredI(float measured);
    pm06Measure measure()const;

    pm06Measure getCoef()const{
        pm06Measure mes;
        mes.V5=voltage5VDivider;
        mes.Vbatt=voltageDivider;
        mes.Ibatt=currentDivider;
        return mes;
    }

    void setVoltage5VDivider(float measured);
    void setVoltageDivider(float measured);
    void setCurrentDivider(float measured);
private:
    float voltageDivider = defaultVoltageDivider;
    float currentDivider = defaultCurrentDivider;
    float voltage5VDivider = defaultVoltage5VDivider;
    uint8_t pin5V = A1;
    uint8_t pinVSense = A2;
    uint8_t pinISense = A3;
};