#include <pm06.h>
#include <Arduino.h>

pm06 myPm;

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("System is starting.");
    pm06Measure res =  myPm.getCoef();
    Serial.print("coefs 5V: ");
    Serial.print(res.V5);
    Serial.print(" V: ");
    Serial.print(res.Vbatt);
    Serial.print(" I: ");
    Serial.println(res.Ibatt);
}

pm06Measure res;

void loop() {
    res = myPm.measure();
    Serial.print("5V: ");
    Serial.print(res.V5);
    Serial.print(" V: ");
    Serial.print(res.Vbatt);
    Serial.print(" I: ");
    Serial.println(res.Ibatt);
    delay(500);
}
