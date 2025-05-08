#include "RV8803.h"
#include <Arduino.h>
#include <Wire.h>

RV8803 rtc;

void setup() {

    Serial.begin(19200);

    Wire.begin(48, 47); // SDA, SCL pins or use Wire.begin() for default pins

    time_t now = 1746709284;
    if (rtc.set_epoch(now))
        Serial.println("RTC time written successfully.");
    else
        Serial.println("Failed to write time to RTC.");
}

void loop() {

    time_t now = rtc.get_epoch();

    struct tm *tm = gmtime(&now);
    Serial.printf(
        "%04d-%02d-%02d %02d:%02d:%02d UTC (%ld)\n",
        tm->tm_year + 1900,
        tm->tm_mon + 1,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec,
        now);

    delay(1000);
}