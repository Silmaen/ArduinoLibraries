//
// Created by damien.lachouette on 02/12/2020.
//

#pragma once

namespace i2c {

    enum Reserved {
        // === I2C Reserved Address ===
        GeneralCall        = 0x00,
        CBusAddress        = 0x02,
        DifferentBusFormat = 0x04,
        FuturePurpose1     = 0x06,
        HiSpdMaster1       = 0x08,
        HiSpdMaster2       = 0x10,
        HiSpdMaster3       = 0x12,
        HiSpdMaster4       = 0x14,
        Slv10BitAddr1      = 0xF0,
        Slv10BitAddr2      = 0xF2,
        Slv10BitAddr3      = 0xF4,
        Slv10BitAddr4      = 0xF6,
        FuturePurpose2     = 0xF8,
        FuturePurpose3     = 0xFA,
        FuturePurpose4     = 0xFC,
        FuturePurpose5     = 0xFE,
    };

    enum KnownDevice {
        // === I2C known used address (https://learn.adafruit.com/i2c-addresses/the-list) ===
        LSM303_Accel  = 0x19,
        LSM303_Magnet = 0x1E,
        L3GD20        = 0x6B,
        BMP280        = 0x76,
        BMP085        = 0x77,
    };
}  // namespace i2c