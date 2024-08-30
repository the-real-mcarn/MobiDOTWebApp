#include <iostream>
#include <string>
#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <base64.hpp>

#include "mobidot/mobidot.hpp"

AsyncWebServer server(80);

MobiDOT MobiDOT(/* rx */ D6, /* tx */ D5, /* ctrl */ D4, /* light */ D7);

// Compile time
const char compile_date[] = __DATE__ " " __TIME__;
#define DEBUG_BAUDRATE 115200

// Wifi init
ESP8266WiFiMulti WiFiMulti;

// Display buffer for online update
const uint16_t bufferLength = MOBIDOT_HEIGHT_REAR * ceil(MOBIDOT_WIDTH_REAR / 8.0);
unsigned char buffer[bufferLength] = {0};

// SERIAL RECEIVE VARS
#define LF 0x0A

char angle_str[10];
int idx = 0;
bool state = false;

void dumpBuffer()
{
    for (size_t i = 0; i < bufferLength; i++)
    {
        if ((i % (int)ceil(MOBIDOT_WIDTH_REAR / 8.0)) == 0)
        {
            Serial.println();
        }

        for (size_t bit = 0; bit < 8; bit++)
        {
            Serial.print(buffer[i] & (1 << bit) ? "1" : "0");
        }
    }
}

uint8_t h2d(char hex) {
    if(hex > 0x39) hex -= 7; // adjust for hex letters upper or lower case
    return(hex & 0xf);
}

void setup()
{
    // Serial setup
    Serial.begin(DEBUG_BAUDRATE);
    delay(10);

    // Print firmware version
    Serial.println(compile_date);

    // Begin LittleFS
    if (!LittleFS.begin())
    {
        Serial.println(F("An Error has occurred while mounting LittleFS"));
        return;
    }

    // Wifi setup
    WiFi.mode(WIFI_STA);
    WiFi.persistent(true);
    WiFiMulti.addAP("Langeboomgaard", "ACvI4152EK");

    Serial.print(F("\nWifi connecting"));
    while (WiFiMulti.run() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.println(bufferLength);

    server.on(
        "/",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(LittleFS, "/index.html", "text/html");
        });

    server.on(
        "/index.css",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(LittleFS, "/index.css", "text/css");
        });

    server.on(
        "/index.js",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(LittleFS, "/index.js", "application/javascript");
        });

    server.on(
        "/command/toggleLight",
        HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            MobiDOT.toggleLight();
            request->send(200, "text/json", "{}");
        });

    server.on(
        "/command/base64",
        HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            // Serial.println("base64 upload");
            // Serial.println(request->getParam(0)->value());

            unsigned char data[bufferLength * 2] = {0}; // (*2) Every character represents a nibble so we need twice as many
            decode_base64((unsigned char*)request->getParam(0)->value().c_str(), data);
            // Serial.println((char*)data);

            for (size_t i = 0; i < bufferLength * 2; i = i + 2) // (*2) See above
            {
                buffer[i / 2] = (h2d(data[i]) << 4) + h2d(data[i + 1]);
            }

            request->send(200, "text/json", "{}");
        });

    server.on(
        "/command/update",
        HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            Serial.println("display update");
            // dumpBuffer();

            MobiDOT.drawBitmap(buffer, MOBIDOT_WIDTH_REAR, MOBIDOT_HEIGHT_REAR, true);
            MobiDOT.update();
            request->send(200, "text/json", "{}");

            memset(&buffer, 0, bufferLength);
        });

    // server.on(
    //     "/command/bitmap",
    //     HTTP_POST,
    //     [](AsyncWebServerRequest *request)
    //     {
    //         Serial.println("bitmap upload");
    //         for (size_t i = 0; i < request->params() / 2; i = i + 2)
    //         {
    //             const uint16_t index = request->getParam(i)->value().toInt();
    //             const unsigned char value = request->getParam(i + 1)->value().toInt();
    //             request->send(200, "text/json", "{}");

    //             buffer[index] = value;
    //         }
    //     });

    server.begin();

    // Display setup
    MobiDOT.selectDisplay(MobiDOT::Display::REAR);
    MobiDOT.toggleLight();
    MobiDOT.update();
}

void loop()
{
}