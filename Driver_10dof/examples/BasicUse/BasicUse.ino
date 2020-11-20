#include <imu10dof.h>
#include <Arduino.h>

imu10dof myDevice;
imu10dof::measure_data data;

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("System is starting.");
    if (!myDevice.begin()){
        Serial.println("Problem during initializing");
        myDevice.checks();
        while(1);
    }
    Serial.println("System in ready.");
}

void loop() {
    data = myDevice.measureAll();
    Serial.print("ax: ");
    Serial.print(data.accel.x());
    Serial.print(" ay: ");
    Serial.print(data.accel.y());
    Serial.print(" az: ");
    Serial.print(data.accel.z());
    Serial.print(" gx: ");
    Serial.print(data.gyro.x());
    Serial.print(" gy: ");
    Serial.print(data.gyro.y());
    Serial.print(" gz: ");
    Serial.print(data.gyro.z());
    Serial.print(" mx: ");
    Serial.print(data.magento.x());
    Serial.print(" my: ");
    Serial.print(data.magento.y());
    Serial.print(" mz: ");
    Serial.print(data.magento.z());
    Serial.print(" P: ");
    Serial.print(data.pressure);
    Serial.print(" T: ");
    Serial.println(data.temperature);
    delay(500);
}
