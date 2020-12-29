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
    myDevice.setResolution(lsm303::Accel::Resolution::HIGH_RES);
    myDevice.setRate(lsm303::Accel::Rate::HZ_ULTRA);
    myDevice.setRange(lsm303::Accel::Range::R_16G);
    Serial.println(F("System in ready."));
    Serial.println(F("RDY"));
}

void measure(){
    Serial.println(F("MES"));
    do {
        acc = myDevice.measure_fast();
        ts  = micros() - tstart;
        Serial.print((int)ts);
        Serial.print(" ");
        Serial.print(acc.x(), 6);
        Serial.print(" ");
        Serial.print(acc.y(), 6);
        Serial.print(" ");
        Serial.println(acc.z(), 6);
    } while(ts < measureDelay);
    Serial.println(F("RDY"));
}

void loop() {
    if (Serial.available()) {
        message = Serial.readStringUntil('\n');
        Serial.println(F("ACK"));
        if (message.startsWith(F("qui_es_tu"))) {
            Serial.println(F("DeviceVibration"));
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("measure"))) {
            Serial.println(F("Begin Measure"));
            tstart = micros();
            measure();
            return;
        }
        if (message.startsWith(F("set_mtime"))) {
            int    a   = message.indexOf(' ');
            String val = message.substring(a);
            val.trim();
            measureDelay = val.toInt();
            Serial.print(F("Measure delay set to: "));
            Serial.println((int)measureDelay);
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_mtime"))) {
            Serial.println((int)measureDelay);
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("set_range"))) {
            int    a   = message.indexOf(' ');
            String val = message.substring(a);
            val.trim();
            myDevice.setRange(lsm303::Accel::Range(val.toInt()));
            Serial.print(F("Measure range set to: "));
            Serial.println((int)myDevice.getRange());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_range"))) {
            Serial.println((int)myDevice.getRange());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("set_resolution"))) {
            int    a   = message.indexOf(' ');
            String val = message.substring(a);
            val.trim();
            myDevice.setResolution(lsm303::Accel::Resolution(val.toInt()));
            Serial.print(F("Measure resolution set to: "));
            Serial.println((int)myDevice.getResolution());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_resolution"))) {
            Serial.println((int)myDevice.getResolution());
            Serial.println(F("RDY"));
            return;
        }
        Serial.println(F("Unknown Command."));
    }
    // periodically send a RDY message
    ts = micros() - tstart;
    if (ts > 1000000) {
        tstart = micros();
        Serial.println(F("RDY"));
    }
}