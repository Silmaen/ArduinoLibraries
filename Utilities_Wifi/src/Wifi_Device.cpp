//
// Created by damien.lachouette on 08/12/2020.
//
#include "Wifi_Device.h"
#if defined(ARDUINO_SAMD_MKRWIFI1010)
#include <WiFiNINA.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#else
#error Unsupported Board.
#endif

uint8_t *Wifi::Device::macAddress(uint8_t *mac) {
    return WiFi.macAddress(mac);
}

IPAddress Wifi::Device::localIP() {
    return WiFi.localIP();
}

IPAddress Wifi::Device::subnetMask() {
    return WiFi.subnetMask();
}

IPAddress Wifi::Device::gatewayIP() {
    return WiFi.gatewayIP();
}

int Wifi::Device::begin(const char *ssid) {
    return WiFi.begin(ssid);
}

int Wifi::Device::begin(const char *ssid, const char *passphrase) {
    return WiFi.begin(ssid, passphrase);
}

void Wifi::Device::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns_server1, IPAddress dns_server2) {
    WiFi.config(local_ip,gateway,subnet,dns_server1,dns_server2);
}

void Wifi::Device::setHostname(const char *name) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
    WiFi.setHostname(name);
#elif defined(ARDUINO_ARCH_ESP8266)
    WiFi.hostname(name);
#endif
}
const char *Wifi::Device::SSID() {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
    return WiFi.SSID();
#elif defined(ARDUINO_ARCH_ESP8266)
    return WiFi.SSID().c_str();
#endif

}
uint8_t *Wifi::Device::BSSID(uint8_t *bssid) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
    return WiFi.BSSID(bssid);
#elif defined(ARDUINO_ARCH_ESP8266)
    return WiFi.BSSID();
#endif
}

int32_t  Wifi::Device::RSSI() {
    return WiFi.RSSI();
}
int8_t  Wifi::Device::scanNetworks() {
    return WiFi.scanNetworks();
}
const char *Wifi::Device::SSID(uint8_t networkItem) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
    return WiFi.SSID(networkItem);
#elif defined(ARDUINO_ARCH_ESP8266)
    return WiFi.SSID(networkItem).c_str();
#endif
}
uint8_t *Wifi::Device::BSSID(uint8_t networkItem, uint8_t *bssid) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
    return WiFi.BSSID(networkItem, bssid);
#elif defined(ARDUINO_ARCH_ESP8266)
    return WiFi.BSSID(networkItem);
#endif

}
uint8_t  Wifi::Device::channel(uint8_t networkItem) { return WiFi.channel(networkItem);}
int32_t  Wifi::Device::RSSI(uint8_t networkItem) {
    return WiFi.RSSI(networkItem);
}

uint8_t  Wifi::Device::status() {
    return WiFi.status();
}

int     Wifi::Device::hostByName(const char *aHostname, IPAddress *aResult) {
    return WiFi.hostByName(aHostname, *aResult);
}