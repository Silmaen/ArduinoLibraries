#include <imu10dof.h>
#include <Arduino.h>

constexpr uint16_t bufsize = 1000;

imu10dof myDevice;

struct accInfo {
    uint64_t timestamp;
    vec3f accel;
};

accInfo  buffer[bufsize];
uint16_t iterator = 0;

void setup() {
    Serial.begin(115200);
    while(!Serial.available());
    Serial.println("System in ready.");
}

void loop() {
    iterator = 0;
    Serial.println("Measuring...");
    uint64_t starttime = micros();
    while(iterator<bufsize) {
        buffer[iterator].timestamp = micros();
        buffer[iterator].accel     = myDevice.measureAccel();
        ++iterator;
    }
    uint64_t stroptime = micros();
    Serial.print("End measure: ");
    Serial.print((stroptime-starttime)/1e6);
    Serial.println("s of measure.");
    Serial.println("Waiting 10seconds.");
    delay(10000); //
}
