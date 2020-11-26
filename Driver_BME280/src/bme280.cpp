//
// Created by damien.lachouette on 26/11/2020.
//
#include "bme280.h"

namespace bme280 {

    void Device::readCalibration() {
        cal.T1             = read16(R_T1_LSB, true);
        cal.T2             = readS16(R_T2_LSB, true);
        cal.T3             = readS16(R_T3_LSB, true);
        cal.P1             = read16(R_P1_LSB, true);
        cal.P2             = readS16(R_P2_LSB, true);
        cal.P3             = readS16(R_P3_LSB, true);
        cal.P4             = readS16(R_P4_LSB, true);
        cal.P5             = readS16(R_P5_LSB, true);
        cal.P6             = readS16(R_P6_LSB, true);
        cal.P7             = readS16(R_P7_LSB, true);
        cal.P8             = readS16(R_P8_LSB, true);
        cal.P9             = readS16(R_P9_LSB, true);
        cal.H1             = read8(R_H1);
        cal.H2             = readS16(R_H2_LSB, true);
        cal.H3             = read8(R_H3);
        uint8_t complement = read8(R_H4_H5);
        cal.H4 =
          static_cast<uint16_t>((static_cast<uint16_t>(read8(R_H4_MSB)) << byte_shift) + (complement << semi_byte_shift)) >>
          semi_byte_shift;
        cal.H5 = (static_cast<uint16_t>(read8(R_H5_MSB)) << semi_byte_shift) + (complement >> semi_byte_shift);
        cal.H6 = readS8(R_H6);
    }

    void Device::readTemperature() {
        int32_t raw  = readUncompensatedTemperature();
        int32_t var1 = ((((raw >> 3U) - ((int32_t)cal.T1 << 1U))) * ((int32_t)cal.T2)) >> 11U;
        int32_t var2 =
          (((((raw >> 4U) - ((int32_t)cal.T1)) * ((raw >> 4U) - ((int32_t)cal.T1))) >> 12U) * ((int32_t)cal.T3)) >> 14U;
        fine_t             = var1 + var2;
        data().Temperature = ((fine_t * 5 + 128) >> 8U) / 100.0;
    }

    void Device::readPressure() {
        int32_t raw  = readUncompensatedPressure();
        int64_t var1 = ((int64_t)fine_t) - 128000;
        int64_t var2 = var1 * var1 * (int64_t)cal.P6;
        var2         = var2 + ((var1 * (int64_t)cal.P5) << 17);
        var2         = var2 + (((int64_t)cal.P4) << 35);
        var1         = ((var1 * var1 * (int64_t)cal.P3) >> 8) + ((var1 * (int64_t)cal.P2) << 12);
        var1         = (((((int64_t)1) << 47) + var1)) * ((int64_t)cal.P1) >> 33;
        if (var1 == 0) {
            data().Pressure = 0;
            return;
        }
        int64_t p       = 1048576 - raw;
        p               = (((p << 31) - var2) * 3125) / var1;
        var1            = (((int64_t)cal.P9) * (p >> 13) * (p >> 13)) >> 25;
        var2            = (((int64_t)cal.P8) * p) >> 19;
        p               = ((p + var1 + var2) >> 8) + (((int64_t)cal.P7) << 4);
        data().Pressure = p / 25600.F;
    }

    void Device::readHumidity() {
        int32_t raw     = readUncompensatedHumidity();
        int32_t x1      = (fine_t - 76800);
        x1              = (((((raw << 14) - (((int32_t)cal.H4) << 20) - (((int32_t)cal.H5) * x1)) + ((int32_t)16384)) >> 15) *
              (((((((x1 * ((int32_t)cal.H6)) >> 10) * (((x1 * ((int32_t)cal.H3)) >> 11) + ((int32_t)32768))) >> 10) +
                 ((int32_t)2097152)) *
                  ((int32_t)cal.H2) +
                8192) >>
               14));
        x1              = (x1 - (((((x1 >> 15) * (x1 >> 15)) >> 7) * ((int32_t)cal.H1)) >> 4));
        x1              = (x1 < 0 ? 0 : x1);
        x1              = (x1 > 419430400 ? 419430400 : x1);
        data().Humidity = (x1 >> 12) / 1024.F;
    }

} // namespace bme280
