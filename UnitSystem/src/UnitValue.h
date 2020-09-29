/**
 * \author argawaen
 * \date 27/09/2020
 *
 */

#pragma once

enum struct LengthUnit {
    Meter,
};

enum struct MassUnit {
    Kilogram,
};

enum struct TimeUnit {
    Second,
};

enum struct CurrentUnit {
    Amper,
};

enum struct TemperatureUnit {
    Kelvin,
    Celsius,
    Farenheit,
};

enum struct LightIntensityUnit {
    Candela,
};

class Unit{
public:
    // basic constructions
    Unit(int8_t le=1, int8_t me=0, int8_t te=0, int8_t ce=0, int8_t the=0, int8_t je=0,
         LengthUnit lu = LengthUnit::Meter,MassUnit mu = MassUnit::Kilogram,
         TimeUnit tu = TimeUnit::Second,CurrentUnit iu = CurrentUnit::Amper,
         TemperatureUnit thu = TemperatureUnit::Kelvin,LightIntensityUnit ju = LightIntensityUnit::Candela):
         L{lu},LengthExp{le},M{mu},MassExp{me},T{tu},TimeExp{te},I{iu},CurrentExp{ce},Teta{thu},TempExp{the},J{ju},LIExp{je}{}
    Unit(const Unit& o) = default;
    Unit(Unit&& o) = default;
    Unit& operator=(const Unit& o) = default;
    Unit& operator=(Unit&& o) = default;
    // comparison
    bool operator==(const Unit& o) const;
    bool operator!=(const Unit& o) const;
    bool samePhysic(const Unit& o) const;
    // operations
    Unit& operator^=(const Unit& o);
    Unit& operator*=(const Unit& o);
    Unit& operator/=(const Unit& o);
private:
    LengthUnit L;
    int8_t LengthExp;
    MassUnit M;
    int8_t MassExp;
    TimeUnit T;
    int8_t TimeExp;
    CurrentUnit I;
    int8_t CurrentExp;
    TemperatureUnit Teta;
    int8_t TempExp;
    LightIntensityUnit J;
    int8_t LIExp;
};

template<class ValueType>
class PhysicValue{
public:
    PhysicValue():data{0},unitSystem{Unit{}}{}
    PhysicValue(ValueType d, Unit u = Unit{}):data{d},unitSystem{u}{}
    PhysicValue(const PhysicValue&) = default;
    PhysicValue(Unit&&) = default;
    PhysicValue& operator=(const PhysicValue&) = default;
    PhysicValue& operator=(PhysicValue&&) = default;
    // comparison
    bool operator==(const Unit& o) const;
    bool operator!=(const Unit& o) const;
    bool sameUnit(const Unit& o) const;
    // operations
    Unit& operator^=(const PhysicValue& o);
    Unit& operator*=(const PhysicValue& o);
    Unit& operator/=(const PhysicValue& o);
    Unit& operator+=(const PhysicValue& o);
    Unit& operator-=(const PhysicValue& o);
    // operations
    Unit operator^(const PhysicValue& o);
    Unit operator*(const PhysicValue& o);
    Unit operator/(const PhysicValue& o);
    Unit operator+(const PhysicValue& o);
    Unit operator-(const PhysicValue& o);
private:
    ValueType data;
    Unit unitSystem;
};
