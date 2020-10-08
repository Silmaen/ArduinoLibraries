#include <AJ-SR04M.h>

#define TRIGGER_PIN 12
#define ECHO_PIN    11

AJSR04M distanceSensor(TRIGGER_PIN, ECHO_PIN);  // Initialize sensor

void setup () {
    Serial.begin(115200);  // We initialize serial connection so that we could print values from sensor.
    // setup temperature
    distanceSensor.setTemperature(22.1);
}

void loop () {

    // Every 500 miliseconds, do a measurement
    Serial.print("Distance: ");
    Serial.print(distanceSensor.measureDistance());
    Serial.println(" m");
    delay(500);
}
