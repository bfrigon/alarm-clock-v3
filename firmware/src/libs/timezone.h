//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/timezone.h
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
#ifndef TIMEZONE_H
#define TIMEZONE_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "time.h"
#include "iprint.h"


enum {
    TZ_REGION_AFRICA,
    TZ_REGION_ANTARCTICA,
    TZ_REGION_ARCTIC_OCEAN,
    TZ_REGION_ASIA,
    TZ_REGION_ATLANTIC_OCEAN,
    TZ_REGION_AUSTRALIA,
    TZ_REGION_CARIBBEAN,
    TZ_REGION_CENTRAL_AMERICA,
    TZ_REGION_ETCETERA,
    TZ_REGION_EUROPE,
    TZ_REGION_INDIAN_OCEAN,
    TZ_REGION_MIDDLE_EAST,
    TZ_REGION_NORTH_AMERICA,
    TZ_REGION_PACIFIC_OCEAN,
    TZ_REGION_SOUTH_AMERICA,
    TZ_REGION_UNKNOWN,
};


PROG_STR( S_TZ_REGION_AFRICA,           "Africa" );
PROG_STR( S_TZ_REGION_NORTH_AMERICA,    "North america" );
PROG_STR( S_TZ_REGION_SOUTH_AMERICA,    "South america" );
PROG_STR( S_TZ_REGION_EUROPE,           "Europe" );
PROG_STR( S_TZ_REGION_CENTRAL_AMERICA,  "Central amer." );
PROG_STR( S_TZ_REGION_CARIBBEAN,        "Caribbean" );
PROG_STR( S_TZ_REGION_PACIFIC_OCEAN,    "Pacific ocean" );
PROG_STR( S_TZ_REGION_ATLANTIC_OCEAN,   "Atlantic ocean" );
PROG_STR( S_TZ_REGION_ARCTIC_OCEAN,     "Arctic ocean" );
PROG_STR( S_TZ_REGION_INDIAN_OCEAN,     "Indian ocean" );
PROG_STR( S_TZ_REGION_MIDDLE_EAST,      "Middle east" );
PROG_STR( S_TZ_REGION_AUSTRALIA,        "Australia" );
PROG_STR( S_TZ_REGION_ASIA,             "Asia" );
PROG_STR( S_TZ_REGION_ANTARCTICA,       "Antarctica" );
PROG_STR( S_TZ_REGION_ETCETERA,         "Etcetera" );



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


class TimeZone {
  public: 
    TimeZone();

    bool setTimezoneByID( uint16_t id );
    bool setTimezoneByName( char *name );
    int16_t findTimezoneByName( char* name );
    const char* getName();
    const char* getAbbreviation( DateTime *local );

    void toLocal( DateTime *utc );
    void toUTC( DateTime *local );
    int16_t getStdUtcOffset();
    int16_t getDstUtcOffset();

    bool isDST( DateTime *local );
    void getStdTransition( int16_t year, DateTime *std );
    void getDstTransition( int16_t year, DateTime *dst );


  private:
    uint16_t _id = 0;
    TimeZoneRules _tz;

};

extern TimeZone g_timezone;

#endif /* TIMEZONE_H */