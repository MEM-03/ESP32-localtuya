/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#define VERY_LARGE_STRING_LENGTH 8000

//
#include "./drivers/output/tft_display.hpp"
#include "./handler/device.hpp"
#include "./handler/command.hpp"
#include "./handler/fs.hpp"

//
void setup() {
    initState();
    initDisplay();

    //
    Serial.setDebugOutput(true);
    Serial.begin(115200);

    //
    nvs.begin("nvs", false);

    //
    initRTC();
    initInput(COMHandler);

    //
    switchScreen(true, CURRENT_DEVICE);

    //
    if (!loadConfigSD(FSHandler)) {
        if (!loadConfigInternal(FSHandler)) {
            _STOP_EXCEPTION_();
        }
    }

    //
    initWiFi();
    while (!WiFiConnected())
    { handleInput(); delay(1); }

    //
    initServer(device);
    switchScreen(false, CURRENT_DEVICE);
    
    //
    Serial.println("Setup is done...");
}

//
void loop() {

    //
    if (INTERRUPTED.load()) {
        // show RSOD error
        if ((millis() - LAST_TIME.load()) >= STOP_TIMEOUT) {
            #ifdef ESP32
        ESP.restart();
            #else
        ESP.reset();
            #endif
        }
    } else {
        switchScreen((!CONNECTED.load() || LOADING_SD), CURRENT_DEVICE);

        //
        handleInput();
        handleWiFi();

        //
        if (WiFiConnected()) { timeClient.update(); }

        _syncTimeFn_();
        handleDevices();
        delay(1);
    }
}
