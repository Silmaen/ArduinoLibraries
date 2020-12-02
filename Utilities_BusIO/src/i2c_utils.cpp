/**
 * \author argawaen
 * \date 22/11/2020
 *
 */
#include "i2c_utils.h"

#include "knownDevices.h"

namespace i2c {

    void scanner() {
        uint8_t error;
        int ndevice = 0;
        Address it;
        it = it.next();
        Wire.begin();
        for(;!it.isLast();it=it.next()){
            Wire.beginTransmission(it.to7bits());
            error = Wire.endTransmission();
            if (error == 0) {
                Serial.print("I2C device found at address 0x");
                Serial.print(it.to7bits(), HEX);
                Serial.println("  !");
                ndevice++;
            }
        }
        if (ndevice == 0)
            Serial.println("No I2C devices found\n");
        else
            Serial.println("done\n");
    }

    Address Address::next() {
        Address t(*this);
        t++;
        while (t.isReserved() && !t.isLast())
            t++;
        return t;
    }

    Address Address::previous() {
        Address t(*this);
        t--;
        while (t.isReserved() && !t.isFirst())
            t--;
        return t;
    }

    bool Address::isReserved() {
        return ((_addr & addrMask) == GeneralCall) || ((_addr & addrMask) == CBusAddress) ||
               ((_addr & addrMask) == DifferentBusFormat) || ((_addr & addrMask) == FuturePurpose1) ||
               ((_addr & addrMask) == HiSpdMaster1) || ((_addr & addrMask) == HiSpdMaster2) ||
               ((_addr & addrMask) == HiSpdMaster3) || ((_addr & addrMask) == HiSpdMaster4) ||
               ((_addr & addrMask) == Slv10BitAddr1) || ((_addr & addrMask) == Slv10BitAddr2) ||
               ((_addr & addrMask) == Slv10BitAddr3) || ((_addr & addrMask) == Slv10BitAddr4) ||
               ((_addr & addrMask) == FuturePurpose2) || ((_addr & addrMask) == FuturePurpose3) ||
               ((_addr & addrMask) == FuturePurpose4) || ((_addr & addrMask) == FuturePurpose5);
    }
} // namespace i2c
