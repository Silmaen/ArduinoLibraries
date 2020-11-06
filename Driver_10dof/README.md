# Arduino library for IMU_10DOF sensor.

The 10dof IMU device is based on 3 chipsets. 

## L3GDH20H

This is a 3-axis gyroscope: +/-250, +/-500, or +/-2000 degree-per-second scale.

It is possible to find the [datasheet](doc/l3gd20h.pdf)

There is also a driver from adafruit [here](https://github.com/adafruit/Adafruit_L3GD20_U)

## LSM303DLHC

This chipset include two sensors:
* 3-axis compass: +/-1.3 to +/-8.1 gauss magnetic field scale
* 3-axis accelerometer: +/-2g +/-4g +/-8g +/-16g selectable scale

It is possible to find the [datasheet](doc/LSM303DLHC.PDF)

There is also a driver from adafruit for accelerometer [here](https://github.com/adafruit/Adafruit_LSM303_Accel)

There is also a driver from adafruit for magnetometer [here](https://github.com/adafruit/Adafruit_LSM303DLH_Mag)

## BMP180

THis is a barometric pressure/temperature: -40 to 85 °C, 300 - 1100hPa range, 0.17m resolution

It is possible to find the [datasheet](doc/BST-BMP180-DS000-09.pdf)

There is also a driver from adafruit [here](https://github.com/adafruit/Adafruit-BMP085-Library)
