#include <MotorVibration.h>

motorVibration device(rc_pin);

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println(F("System is starting."));
    device.begin();
    Serial.println(F("System in ready."));
    Serial.println(F("RDY"));
}

void measure(){
    Serial.println(F("MES"));
    device.measure();
    Serial.println(F("RDY"));
}
uint64_t tstart = 0, ts;

void loop() {
    if (Serial.available()) {
        String message = Serial.readStringUntil('\n');
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
            device.setTestDuration(val.toInt());
            Serial.print(F("Measure delay set to: "));
            Serial.println((int)device.getTestDuration());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_mtime"))) {
            Serial.println((int)device.getTestDuration());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("set_mwait"))) {
            int    a   = message.indexOf(' ');
            String val = message.substring(a);
            val.trim();
            device.setInitialWait(val.toInt());
            Serial.print(F("Measure delay set to: "));
            Serial.println((int)device.getInitialWait());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_mwait"))) {
            Serial.println((int)device.getInitialWait());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("set_mthrottle"))) {
            int    a   = message.indexOf(' ');
            String val = message.substring(a);
            val.trim();
            device.setMotorThrottle(val.toInt());
            Serial.print(F("Measure delay set to: "));
            Serial.println((int)device.getMotorThrottle());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_mthrottle"))) {
            Serial.println((int)device.getMotorThrottle());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("set_range"))) {
            int    a   = message.indexOf(' ');
            String val = message.substring(a);
            val.trim();
            device.setAccelRange(val.toInt());
            Serial.print(F("Measure range set to: "));
            Serial.println(device.getAccelRange());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_range"))) {
            Serial.println(device.getAccelRange());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("set_resolution"))) {
            int    a   = message.indexOf(' ');
            String val = message.substring(a);
            val.trim();
            device.setAccelResolution(val.toInt());
            Serial.print(F("Measure resolution set to: "));
            Serial.println(device.getAccelResolution());
            Serial.println(F("RDY"));
            return;
        }
        if (message.startsWith(F("get_resolution"))) {
            Serial.println(device.getAccelResolution());
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
