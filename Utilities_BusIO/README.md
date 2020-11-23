# Arduino library for IO structure

This library gathers utilities to manage the different buses. 

## The supported buses

 * [ ] CAN [specs](https://en.wikipedia.org/wiki/CAN_bus)
 * [x] I2C [specs](https://en.wikipedia.org/wiki/I%C2%B2C)
   * [x] Sensor device
   * [ ] Other device
 * [ ] SPI [specs](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
 * [ ] UART [specs](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
 
## The I2C utilities

### SensorDevice

The SensorDevice class designs a templated abstract class. The template arguments setup the device 
address and data type.

Example:
```c++
#include <i2c_utils.h>

class mySensor: public i2c::SensorDevice<0b00110110, float> {
	// ...
};
```
Setup a sensor with the i2c address set as `00110110` and its data of float type.
