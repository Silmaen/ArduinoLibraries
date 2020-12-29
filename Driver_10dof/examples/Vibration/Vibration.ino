#include <Arduino.h>
#include <imu10dof.h>

lsm303::Accel myDevice;

static uint64_t    ts, tstart;
static math::vec3f acc;
uint8_t            mode = 0;
String             message;
uint64_t           measureDelay = 5000000;

void setup() {
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println(F("System is starting."));
    if (!myDevice.begin()) {
        Serial.println(F("Problem during initializing"));
        myDevice.print_config();
        while (1)
            ;
    }
    myDevice.setRate(lsm303::Accel::Rate::HZ_ULTRA);
    Serial.println(F("System in ready."));
    Serial.println(F("RDY"));
}

void loop() {
    if (mode == 0) {
        if (Serial.available()) {
            message = Serial.readStringUntil('\n');
            Serial.println(F("ACK"));
            if (message.startsWith(F("quiestu"))) {
                Serial.println(F("DeviceVibration"));
                Serial.println(F("RDY"));
                return;
            }
            if (message.startsWith(F("measure"))) {
                Serial.println(F("Begin Measure"));
                tstart = micros();
                mode   = 1;
                Serial.println(F("MES"));
                return;
            }
            if (message.startsWith(F("setmtime"))) {
                int    a   = message.indexOf(' ');
                String val = message.substring(a);
                val.trim();
                measureDelay = val.toInt();
                Serial.print(F("Measure delay set to: "));
                Serial.println((int)measureDelay);
                Serial.println(F("RDY"));
                return;
            }
            if (message.startsWith(F("getmtime"))) {
                Serial.println((int)measureDelay);
                Serial.println(F("RDY"));
                return;
            }
            Serial.println(F("Unknown Command."));
        }
        ts = micros() - tstart;
        if (ts > 1000000) {
            tstart = micros();
            Serial.println(F("RDY"));
        }

    } else {
        acc = myDevice.measure();
        ts  = micros() - tstart;
        Serial.print((int)ts);
        Serial.print(" ");
        Serial.print(acc.x(), 6);
        Serial.print(" ");
        Serial.print(acc.y(), 6);
        Serial.print(" ");
        Serial.println(acc.z(), 6);
        if (ts > measureDelay) {
            mode = 0;
            Serial.println(F("RDY"));
        }
    }
}