#include <esc.h>
#include <Arduino.h>

esc myEsc(5, 982,2006,982);

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("System is starting.");
}

float throttle = 0.;

void loop() {
    Serial.println("Test Acceleration.");
    myEsc.arm();
    throttle = 0.;
    while(throttle < 20) {
        throttle += 5;
        myEsc.throttle(throttle);
        Serial.print("Throttle set to: ");
        Serial.print(throttle);
        Serial.print(" -- ");
        Serial.print(myEsc.getOutSignal());
        Serial.println(".");
        delay(500);
    }
    myEsc.stop();
    Serial.println("Test Finished.");
    Serial.println("Waiting 10 seconds.");
    delay(10000);
}
