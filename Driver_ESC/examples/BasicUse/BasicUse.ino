#include <esc.h>
#include <Arduino.h>

Esc myEsc(5);

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("System is starting.");
    myEsc.setMinMax(958,2005);
    Serial.println("System in ready.");
}

float throttle = 0.;

void loop() {
    Serial.println("Test Acceleration.");
    myEsc.arm();
    while(throttle < 0.20) {
        throttle += 0.05;
        myEsc.throttle(throttle);
        Serial.print("Throttle set to: ");
        Serial.print(throttle);
        Serial.println(".");
        delay(500);
    }
    myEsc.stop();
    Serial.println("Test Finished.");
    Serial.println("Waiting 10 seconds.");
    delay(10000);
}
