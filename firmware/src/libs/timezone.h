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
#include "tzdata.h"
#include "time.h"


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