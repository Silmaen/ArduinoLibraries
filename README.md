# Arduino Libraries

My gathering in a single git repository. All of my home-made arduino libraries lies here.

## installation

Clone this repository. 
If put into your `<Arduino Sketchbook Location>/libraries`, the libraries becomes available in Arduino IDE.

## building

You can directly use the Arduino IDE, or any IDE that works with CMake.

A `cmake` directory, clone of [this one](https://github.com/a9183756-gh/Arduino-CMake-Toolchain) allow the support
of any CMake-based IDE.

## Board support

The supported boards depends on the installed tools on the building machine. 
First, it requires the installation of the Arduino IDE [here](https://www.arduino.cc/en/software).
Secondly, it requires the installation of the build tools required by any board.

The CMakeLists supports only some boards (only the boards that I have). 
It just needs to set up the `BOARD` variable. `cmake -DBOARD=<...>` 
Find the list of directly supported boards:
 * `micro`: Arduino Micro (Native, `Arduino AVR Boards`)
 * `mega`: Arduino Mega or Mega 2560 (Native, `Arduino AVR Boards`)
 * `mkrwifi`: Arduino MKR Wifi 1010 (Require `Arduino SAMD (32-bits ARM Cortex-M0+) Boards`)
 * `wemos`: LOLIN(WEMOS) D1 mini (Require [custom](https://arduino.esp8266.com/stable/package_esp8266com_index.json) `ESP8266 Boards`)

Adding any board remain as simple as editing the base CMakeLists.

## content

### Library content

Library lies in a sub folder. The folder gives the library's title.

The library folder should contain:
 * example
    * [...] (any example code in sub folders)
 * src
    * [...] (any files or sub folder containing the source code of the library)
 * CMakelists.txt (just copy an existing one, and setup the required libraries)
 * keywords.txt (designs additional keywords for the Arduino IDE)
 * library.properties (defines some properties used by Arduino IDE)
 * LICENSE (the License definitions)
 * README.md (the documentation of the library, should be linked into the main documentation)

Other folders or files may exists in specific cases.

### List of available libs

This library contains some useful stuffs:

* Cloud application
  * [ ] Home Assistant direct connector [here](Cloud_HomeAssistant/README.md)
* Data management
  * [x] Filter [here](Data_Filters/README.md)
  * [X] Linear algebra [here](Data_LinearAlgebra/README.md)
* Device Drivers
  * [x] 10-dof device [here](Driver_10dof/README.md)
  * [ ] MKR env
  * [X] BME280 device (Temperature, Humidity, Pressure) [here](Driver_BME280/README.md)
  * [x] AJ-SR04M device [here](Driver_AJ-SR04M/README.md)
  * [x] Home Made hall effect tachometer [here](Driver_Tachymeter_hall/README.md)
  * [ ] RA8875 TFT driver
* Utilities 
  * [x] busIO [here](Utilities_BusIO/README.md)

