#include <Tachometer.h>
#include <MyFilters.h>

#define TRIGGER_PIN 7
#define PULSE_LENGTH 0.816814
#define TIMEOUT 800000

Tachometer Anemometer(TRIGGER_PIN, PULSE_LENGTH, TIMEOUT);  // Initialize sensor

void setup () {
    Serial.begin(115200);  // We initialize serial connection so that we could print values from sensor.
}

MyFilter<float> filterVelMs(30,FilterType::MedianFilter);
MyFilter<float> filterVelKmh(30,FilterType::MedianFilter);
MyFilter<float> filterFreq(30,FilterType::MedianFilter);

void loop () {
    // Every 100 miliseconds, do a measurement
    
    Serial.print("Wind Speed: ");
    Serial.print(filterVelMs.addValue(Anemometer.getVelocity()));
    Serial.print(" m/s = ");
    Serial.print(filterVelKmh.addValue(Anemometer.getVelocity(VelocityUnit::KilometerPerHour)));
    Serial.print(" km/h ");
    Serial.print(filterFreq.addValue(Anemometer.getFrequency()));
    Serial.println(" Hz");
    delay(100);
}