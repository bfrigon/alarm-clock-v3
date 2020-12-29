//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/tzdata.h
// Author  : Benoit Frigon <www.bfrigon.com>
//
// -----------------------------------------------------------------------------
//
// This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
// International License. To view a copy of this license, visit
//
// http://creativecommons.org/licenses/by-sa/4.0/
//
// or send a letter to Creative Commons,
// PO Box 1866, Mountain View, CA 94042, USA.
//
//******************************************************************************
#ifndef TZDATA_H
#define TZDATA_H

#include <Arduino.h>
#include <avr/pgmspace.h>



struct TimeZoneRules {
    const char *name;

    int16_t std_offset;
    uint8_t std_month: 4;
    uint8_t std_week: 3;
    uint8_t std_dow: 6;
    uint8_t std_hour: 5;
    uint8_t std_min: 6;
    const char *std_abbvr;

    int16_t dst_offset;
    uint8_t dst_month: 4;
    uint8_t dst_week: 3;
    uint8_t dst_dow: 6;
    uint8_t dst_hour: 5;
    uint8_t dst_min: 6;
    const char *dst_abbvr;
};

#define MAX_TIMEZONE_ID     3

const char TZ_EST[] PROGMEM = { "EST"};
const char TZ_EDT[] PROGMEM = { "EDT"};
const char TZ_UTC[] PROGMEM = { "UTC" };
const char TZ_AEDT[] PROGMEM = { "AEDT" };
const char TZ_AEST[] PROGMEM = { "AEST" };


const char TZ_AFRICA_JOHANNESBURG[] PROGMEM = { "Africa/Johannesburg" };
const char TZ_AMERICA_TORONTO[] PROGMEM = { "America/Toronto" };
const char TZ_AUSTRALIA_SYDNEY[] PROGMEM = { "Australia/Sydney" };


const TimeZoneRules TimeZonesTable[] PROGMEM = { 
    { TZ_UTC, 0, 0, 0, 0, 0, 0, TZ_UTC, 0, 0, 0, 0, 0, 0, TZ_UTC },
    { TZ_AMERICA_TORONTO, -300, 11, 1, 0, 2, 0, TZ_EST, -240, 3, 2, 0, 2, 0, TZ_EDT },
    { TZ_AUSTRALIA_SYDNEY, 600, 4, 1, 0, 3, 0, TZ_AEST, 660, 10, 1, 0, 2, 0, TZ_AEDT },
};


#endif /* TZ_DATA */