#include <Tachometer.h>

#define TRIGGER_PIN 7
#define PULSE_LENGTH 0.53407
#define TIMEOUT 400000ull

Tachometer Anemometer(TRIGGER_PIN, PULSE_LENGTH, TIMEOUT);  // Initialize sensor

void setup () {
    Serial.begin(115200);  // We initialize serial connection so that we could print values from sensor.
}

void loop () {
    // Every 500 miliseconds, do a measurement
    Serial.print("Wind Speed: ");
    Serial.print(Anemometer.getVelocity());
    Serial.print(" m/s = ");
    Serial.print(Anemometer.getVelocity(VelocityUnit::KilometerPerHour));
    Serial.println(" km/h");
    delay(500);
}
