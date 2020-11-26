#include <Arduino.h>
#include <bme280.h>

bme280::Device sensor(bme280::weatherMonitoring);

void setup() {
    Serial.begin(115200);
    sensor.begin();
}

void loop() {
    auto data = sensor.measure();
    Serial.print(data.Temperature, 2);
    Serial.print("°C ");
    Serial.print(data.Humidity, 2);
    Serial.print("% ");
    Serial.print(data.Pressure, 2);
    Serial.println("hPa");
    delay(60000); // wait one minute
}
