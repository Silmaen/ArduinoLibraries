# My AHRS

This library include the IMU and the fusion of sensor using a simple kalman filter.

This has the goal to determine the position and rotation of the device.

## The IMU

the IMU is composed of 3 chipsets:
 * LSM303DLHC [datasheet](https://cdn-shop.adafruit.com/datasheets/LSM303DLHC.PDF) for accelerometers + Magnetometers
 * L3GD20H [datasheet](https://cdn-shop.adafruit.com/datasheets/L3GD20.pdf) for gyroscopes
 * BMP180 [datasheet](https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf) for temperture and pressure

### Pressure to Altitude conversion

in order to convert the pressure to an altitude we use the following formula:

```math
h = (273.15 + Ts)/0.0065 * ( 1 - (Ps / p0) ^ 1/5.255  )
```
In this relation `Ps` & `Ts` are the pressure & temperature given by the sensor. `p0` is the 
pressure reduced at the see level (the well-known QNH in aeronautics).

This value should be regularily actualised. this could be done by giving him its actual 
altitude.


## The sensor fusion

### Kalman filter




## Annex
 
### Some words about pressure to altitude

To convert Pressure measurement to altitude, we could start with the perfect gaz relation:

```math
PV=nRT
```

where `P` is thePressure, `V` the volume and `T` the temperature

the thermodynamic tells us that `nR = m(Cp - Cv)` (with `Cp` & `Cv` the calorific capacity at constant respectively 
pressure and volume and `m` the mass) and the density is `rho = m / V` then it comes a relation more suitable for meteorology:

```math
P = rho ( Cp - Cv ) T
```

In the litterature we could found a relation: `gamma = Cp / Cv` (with `gamma` = 7/5 for air between 
-63°C and 80°C). By setting `epsilon = (gamma - 1) / gamma` ( = 2/7 ) we could write:

```math
P = rho epsilon Cp T
```

For the air between -63°C and 80°C `Cp` = 1006 kJ/kg

We suppose the linear variation of temperature depending on the altitude:

```math
Th = T0 - Gh h
```
Where `T0` is the temperature at see level (i.e. `h=0`) and `Gh` is the thermal gradient.

