//
// Created by damien.lachouette on 08/12/2020.
//

#pragma once
#include <Arduino.h>
#include <IPAddress.h>
#include "MacAddress.h"

namespace Wifi {

    enum wl_status_t{
        WL_NO_SHIELD = 255,
        WL_NO_MODULE = WL_NO_SHIELD,
        WL_IDLE_STATUS = 0,
        WL_NO_SSID_AVAIL = 1,
        WL_SCAN_COMPLETED = 2,
        WL_CONNECTED = 3,
        WL_CONNECT_FAILED = 4,
        WL_CONNECTION_LOST = 5,
        WL_DISCONNECTED = 6,
        WL_AP_LISTENING = 7,
        WL_AP_CONNECTED = 8,
        WL_AP_FAILED = 9
    };
/**
 * \brief Wifi Device management
 */
class Device {
public:

    /**
     * \brief Start Wifi connection for OPEN networks
     *
     * \param ssid Pointer to the SSID string.
     */
    wl_status_t begin(const char* ssid);

    /**
     * \brief Start Wifi connection with passphrase
     *
     * the most secure supported mode will be automatically selected
     *
     * \param ssid Pointer to the SSID string.
     * \param passphrase Passphrase. Valid characters in a passphrase
     *        must be between ASCII 32-126 (decimal).
     */
    wl_status_t begin(const char* ssid, const char *passphrase);


    wl_status_t beginAndConnect(const char* ssid, const char *passphrase);

    /**
     * \brief Change Ip configuration settings disabling the dhcp client
     * \param local_ip        Static ip configuration
     * \param gateway         Static gateway configuration
     * \param subnet          Static Subnet mask
     * \param dns_server2     IP configuration for DNS server 1
     * \param dns_server1     IP configuration for DNS server 2
     */
    void config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns_server1= IPAddress(), IPAddress dns_server2= IPAddress());

    /**
     * \brief Set the hostname used for DHCP requests
     *
     * \param name hostname to set
     *
     */
    void setHostname(const char* name);

    /**
     * \brief Get the interface MAC address.
     *
     * \return pointer to uint8_t array with length wlMacAddrLength
     */
    network::MacAddress macAddress();

    /**
     * \brief Get the interface IP address.
     *
     * \return Ip address value
     */
    IPAddress localIP();

    /**
     * \brief Get the interface subnet mask address.
     *
     * \return subnet mask address value
     */
    IPAddress subnetMask();

    /**
     * \brief Get the gateway ip address.
     *
     * \return gateway ip address value
     */
    IPAddress gatewayIP();

    /**
     * \brief Return the current SSID associated with the network
     * \return ssid string
     */
    String SSID();

    /**
     * \brief Return the current BSSID associated with the network.
     *
     * It is the MAC address of the Access Point
     * \return pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
     */
    network::MacAddress BSSID();

    /**
     * \brief Return the current RSSI /Received Signal Strength in dBm) associated with the network
     * \return signed value
     */
    int32_t RSSI();

    /**
     * \brief Start scan WiFi networks available
     *
     * \return Number of discovered networks
     */
    int8_t scanNetworks();

    /**
     * \brief Return the SSID discovered during the network scan.
     * \param networkItem specify from which network item want to get the information
     * \return ssid string of the specified item on the networks scanned list
     */
    String	SSID(uint8_t networkItem);

    network::MacAddress BSSID(uint8_t networkItem, uint8_t* bssid=nullptr);
    uint8_t channel(uint8_t networkItem);

    /**
     * \brief Return the RSSI of the networks discovered during the scanNetworks
     * \param networkItem specify from which network item want to get the information
     * \return signed value of RSSI of the specified item on the networks scanned list
     */
    int32_t RSSI(uint8_t networkItem);

    /**
     * \brief Return Connection status.
     * \return one of the value defined in wl_status_t
     */
    uint8_t status();


    /**
     * \brief Resolve the given hostname to an IP address.
     * \param aHostname Name to be resolved
     * \param aResult IPAddress structure to store the returned IP address
     * \result 1 if aIPAddrString was successfully converted to an IP address,
     *          else error code
     */
    int hostByName(const char* aHostname, IPAddress* aResult);

};

}  // namespace Wifi
