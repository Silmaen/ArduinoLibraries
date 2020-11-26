#include <Arduino.h>
#include <bme280.h>

bme280::Setting myparam{
  bme280::WorkingMode::Normal,
  bme280::Oversampling::O_X8,
  bme280::Oversampling::O_X4,
  bme280::Oversampling::O_X4,
  bme280::StandyByTime::SBT_0_5,
  bme280::FilterCoefficient::F_16
};

bme280::Device sensor(bme280::indoorNavigation);

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
    delay(1000); // wait 1 second
}
