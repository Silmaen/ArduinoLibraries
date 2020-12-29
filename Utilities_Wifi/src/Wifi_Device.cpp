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
namespace Wifi {

    network::MacAddress Device::macAddress() {
        byte mac[6];
        WiFi.macAddress(mac);
        return network::MacAddress(mac);
    }

    IPAddress Device::localIP() { return WiFi.localIP(); }

    IPAddress Device::subnetMask() { return WiFi.subnetMask(); }

    IPAddress Device::gatewayIP() { return WiFi.gatewayIP(); }

    wl_status_t Device::begin(const char *ssid) { return static_cast<wl_status_t>(WiFi.begin(ssid)); }

    wl_status_t Device::begin(const char *ssid, const char *passphrase) {
#if defined(ARDUINO_ARCH_ESP8266)
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
#endif
        return static_cast<Wifi::wl_status_t>(WiFi.begin(ssid, passphrase));
    }

    void
    Device::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns_server1, IPAddress dns_server2) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
        WiFi.config(local_ip, dns_server1, gateway, subnet);
        WiFi.setDNS(dns_server1, dns_server2);
#elif defined(ARDUINO_ARCH_ESP8266)
        WiFi.config(local_ip, gateway, subnet, dns_server1, dns_server2);
#endif
    }

    void Device::setHostname(const char *name) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
        WiFi.setHostname(name);
#elif defined(ARDUINO_ARCH_ESP8266)
        WiFi.hostname(name);
#endif
    }
    String Device::SSID() {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
        return String(WiFi.SSID());
#elif defined(ARDUINO_ARCH_ESP8266)
        return WiFi.SSID();
#endif
    }
    network::MacAddress Device::BSSID() {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
        return network::MacAddress(WiFi.BSSID(bssid));
#elif defined(ARDUINO_ARCH_ESP8266)
        return network::MacAddress(WiFi.BSSID());
#endif
    }

    int32_t     Device::RSSI() { return WiFi.RSSI(); }
    int8_t      Device::scanNetworks() { return WiFi.scanNetworks(); }
    String Device::SSID(uint8_t networkItem) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
        return String(WiFi.SSID(networkItem));
#elif defined(ARDUINO_ARCH_ESP8266)
        return WiFi.SSID(networkItem);
#endif
    }
    network::MacAddress Device::BSSID(uint8_t networkItem, uint8_t *bssid) {
#if defined(ARDUINO_SAMD_MKRWIFI1010)
        return network::MacAddress(WiFi.BSSID(networkItem, bssid));
#elif defined(ARDUINO_ARCH_ESP8266)
        return network::MacAddress(WiFi.BSSID(networkItem));
#endif
    }
    uint8_t Device::channel(uint8_t networkItem) { return WiFi.channel(networkItem); }
    int32_t Device::RSSI(uint8_t networkItem) { return WiFi.RSSI(networkItem); }

    uint8_t Device::status() { return WiFi.status(); }

    int Device::hostByName(const char *aHostname, IPAddress *aResult) { return WiFi.hostByName(aHostname, *aResult); }

    wl_status_t Wifi::Device::beginAndConnect(const char *ssid, const char *passphrase) {
        if (status() == Wifi::WL_NO_MODULE) {
            return Wifi::WL_NO_MODULE;
        }
        begin(ssid, passphrase);
        while (status() != Wifi::WL_CONNECTED) {
            delay(500);
        }
        return Wifi::WL_CONNECTED;
    }

}  // namespace Wifi
