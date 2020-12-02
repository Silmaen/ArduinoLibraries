#include <Arduino.h>
#include <imu10dof.h>

lsm303::Accel myDevice;

static uint64_t last_ts, ts;
static math::vec3f last_acc, acc, vibe;

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("System is starting.");
    if (!myDevice.begin()){
        Serial.println("Problem during initializing");
        myDevice.print_config();
        while(1);
    }
    myDevice.setRate(lsm303::Accel::Rate::HZ_ULTRA);
    Serial.println("System in ready.");
    last_acc = myDevice.measure();
    last_ts = micros();
}


void loop() {
    acc = myDevice.measure();
    ts = micros();
    vibe = (acc-last_acc) / static_cast<float>(ts - last_ts);
    last_acc=acc;
    last_ts=ts;
    Serial.print("acc.x: ");
    Serial.print(acc.x(),6);
    Serial.print(" acc.y: ");
    Serial.print(acc.y(),6);
    Serial.print(" acc.z: ");
    Serial.print(acc.z()-9.81,6);
    Serial.print(" vibe.x: ");
    Serial.print(vibe.x(),6);
    Serial.print(" vibe.y: ");
    Serial.print(vibe.y(),6);
    Serial.print(" vibe.z: ");
    Serial.println(vibe.z(),6);
}
