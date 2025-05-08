#pragma once
#include <Wire.h>
#include <time.h>

// Fixed I2C address
#define RV8803_ADDR 0x32

// Only required registers
#define RV8803_SECONDS 0x11
#define RV8803_MINUTES 0x12
#define RV8803_HOURS 0x13
#define RV8803_WEEKDAYS 0x14
#define RV8803_DATE 0x15
#define RV8803_MONTHS 0x16
#define RV8803_YEARS 0x17

// Weekday bit flags
#define SUNDAY 0x01
#define MONDAY 0x02
#define TUESDAY 0x04
#define WEDNESDAY 0x08
#define THURSDAY 0x10
#define FRIDAY 0x20
#define SATURDAY 0x40

class RV8803 {
  public:
    RV8803(TwoWire &wirePort = Wire) : _wire(wirePort) {
    }

    time_t get_epoch() {
        _wire.beginTransmission(RV8803_ADDR);
        _wire.write(RV8803_SECONDS);
        _wire.endTransmission(false);
        _wire.requestFrom(RV8803_ADDR, (uint8_t)7);

        uint8_t sec = bcdToDec(_wire.read() & 0x7F);
        uint8_t min = bcdToDec(_wire.read() & 0x7F);
        uint8_t hr = bcdToDec(_wire.read() & 0x3F);
        _wire.read(); // weekday — ignore
        uint8_t day = bcdToDec(_wire.read() & 0x3F);
        uint8_t mon = bcdToDec(_wire.read() & 0x1F);
        uint8_t yr = bcdToDec(_wire.read());

        struct tm tm;
        tm.tm_sec = sec;
        tm.tm_min = min;
        tm.tm_hour = hr;
        tm.tm_mday = day;
        tm.tm_mon = mon - 1;
        tm.tm_year = yr + 100;
        tm.tm_isdst = 0;

        return mktime(&tm);
    }

    bool set_epoch(time_t epoch) {
        struct tm *tm = gmtime(&epoch);

        _wire.beginTransmission(RV8803_ADDR);
        _wire.write(RV8803_SECONDS);
        _wire.write(decToBcd(tm->tm_sec));
        _wire.write(decToBcd(tm->tm_min));
        _wire.write(decToBcd(tm->tm_hour));
        _wire.write(weekdayToBit(tm->tm_wday));
        _wire.write(decToBcd(tm->tm_mday));
        _wire.write(decToBcd(tm->tm_mon + 1));
        _wire.write(decToBcd(tm->tm_year % 100));
        return _wire.endTransmission() == 0;
    }

  private:
    TwoWire &_wire;

    uint8_t decToBcd(uint8_t val) {
        return (val / 10 * 16) + (val % 10);
    }

    uint8_t bcdToDec(uint8_t val) {
        return (val / 16 * 10) + (val % 16);
    }

    uint8_t weekdayToBit(uint8_t tm_wday) {
        switch (tm_wday) {
            case 0:
                return SUNDAY;
            case 1:
                return MONDAY;
            case 2:
                return TUESDAY;
            case 3:
                return WEDNESDAY;
            case 4:
                return THURSDAY;
            case 5:
                return FRIDAY;
            case 6:
                return SATURDAY;
            default:
                return 0;
        }
    }
};
