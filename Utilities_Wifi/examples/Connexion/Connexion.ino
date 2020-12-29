/*
 *
 */
#include <Wifi_Device.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = Wifi::WL_IDLE_STATUS;     // the Wifi radio's status

Wifi::Device WiFi_d;
void printWifiData() {
    // print your board's IP address:
    IPAddress ip = WiFi_d.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println(ip);

    // print your MAC address:
    network::MacAddress mac =WiFi_d.macAddress();
    Serial.print("MAC address: ");
    Serial.println(mac.toString());
}

void printCurrentNet() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi_d.SSID());

    // print the MAC address of the router you're attached to:
    network::MacAddress bssid=WiFi_d.BSSID();
    Serial.print("BSSID: ");
    Serial.println(bssid.toString());

    // print the received signal strength:
    long rssi = WiFi_d.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial)  ; // wait for serial port to connect. Needed for native USB port only

  // check for the WiFi module:
  if (WiFi_d.beginAndConnect(ssid, pass) == Wifi::WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  printWifiData();
  printCurrentNet();
}

void loop() {
  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();
}

