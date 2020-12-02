#include <Arduino.h>
#include <bme280.h>

bme280::Setting myparam{
  bme280::WorkingMode::Normal,
  bme280::Oversampling::O_X16,
  bme280::Oversampling::O_X16,
  bme280::Oversampling::O_X16,
  bme280::StandyByTime::SBT_0_5,
  bme280::FilterCoefficient::F_16
};

bme280::Device sensor(myparam);

void setup() {
    Serial.begin(115200);
    while(!Serial);
    if (!sensor.begin()) {
        Serial.println("Error: no device");
    }
}

void loop() {
    auto data = sensor.measure();
    Serial.print(data.Temperature, 2);
    Serial.print("°C ");
    Serial.print(data.Humidity, 2);
    Serial.print("% ");
    Serial.print(data.Pressure, 2);
    Serial.println("hPa");
    delay(60); // wait 1 second
}
