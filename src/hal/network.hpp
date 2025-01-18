#pragma once

//
#include <WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>

//
#include <persistent/nv_string.hpp>
#include <std/debug.hpp>

//
static nv::_NvString_<16> wifi_ssid("wifi_ssid");
static nv::_NvString_<16> wifi_pass("wifi_pass");

//
//int status = WL_IDLE_STATUS;
//IPAddress server(74,125,115,105);
//WiFiClient client;

//
wl_status_t connectWifi() {
    const auto start = millis();
          auto status = WiFi.begin(wifi_ssid, wifi_pass);
    while (status != WL_CONNECTED) {
        status = WiFi.begin(wifi_ssid, wifi_pass);
        DebugLine(".");
        if ((millis() - start) > 1000) break;
    }
    DebugLog("");
    DebugLog(status == WL_CONNECTED ? "Connection Success" : "Connection Failed");
    return status;
}

//
int connectToDevice(WiFiClient& client, IPAddress& local, uint16_t port = 6668) {
    return client.connect(local, port);
}

//
void waitAndSend(WiFiClient& client, uint8_t* data, size_t length = 0) {
    const auto start = millis();

    // if not available, try to wait
    while (!client.availableForWrite()) {
        DebugLine(".");
        if ((millis() - start) > 1000 || WiFi.status() != WL_CONNECTED) break;
    }
    DebugLog("");

    // if available, write
    if (client.availableForWrite()) {
        client.write(data, length);
    } else {
        DebugLog("Sending Not Available");
    }
}

//
bool waitForReceive(WiFiClient& client, uint8_t* data, size_t& length) {
    bool done = false;

    // if not available, try to wait
    const auto start = millis();
    while (!client.available()) {
        DebugLine(".");
        if ((millis() - start) > 1000 || WiFi.status() != WL_CONNECTED) break;
    }
    DebugLog("");

    // if is tuya package
    const uint8_t prefix[] = {0x00, 0x00, 0x55, 0xAA};
    const uint8_t suffix[] = {0x00, 0x00, 0xAA, 0x55};
    if (client.available() && client.find(suffix, 4) && client.find(prefix, 4)) {
        length = 0;
        while (client.available()) {
            client.readBytes(data, 1); length++;
        }
        done = true;
    }

    //
    return done;
}
