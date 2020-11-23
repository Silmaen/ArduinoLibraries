# ArduinoLibraries

This is only a gathering into a single git repository of all my home-made arduino libraries.

## installation

Simply clone this repository onto your `<Arduino Sketchbook Location>/libraries` and the libraries should became 
available.

## building

You can directly use the Arduino IDE, or any IDE that is able to work with CMake.

A `cmake` directory is bundled in this repository, this is a clone of 
[this one](https://github.com/a9183756-gh/Arduino-CMake-Toolchain).

## content

This library contains some useful stufs:

* Cloud application
  * [ ] Home Assistant direct connector [here](Cloud_HomeAssistant/README.md)
* Data management
  * [x] Filter [here](Data_Filters/README.md)
  * [ ] Linear algebra [here](Data_LinearAlgebra/README.md)
* Device Drivers
  * [x] 10-dof device [here](Driver_10dof/README.md)
  * [ ] MKR env 
  * [x] AJ-SR04M device [here](Driver_AJ-SR04M/README.md)
  * [x] Home Made hall effect tachymeter [here](Driver_Tachymeter_hall/README.md)
  * [ ] RA8875 TFT driver
* Utilities 
  * [x] busIO [here](Utilities_BusIO/README.md)

