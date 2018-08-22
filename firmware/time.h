//******************************************************************************
//
// Project : Alarm Clock V3
// File    : time.h
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

#ifndef TIME_H
#define TIME_H

#include <Sodaq_DS3231.h>
#include <avr/pgmspace.h>

struct Time {
    uint8_t hour;
    uint8_t minute;
};

struct Date {
    uint8_t day;
    uint8_t month;
    uint8_t year;
};


#define DATE_FORMAT_DDMMYYYY        0
#define DATE_FORMAT_MMDDYYYY        1
#define DATE_FORMAT_YYYYMMDD        2
#define DATE_FORMAT_DDMMMYYYY       3
#define DATE_FORMAT_MMMDDYYYY       4
#define DATE_FORMAT_YYYYMMMDD       5
#define DATE_FORMAT_WDMMMDD        6
#define DATE_FORMAT_WDMMMDDYYYY    7






const char* getMonthName(uint8_t month, bool shortName);
const char* getDayName(uint8_t day, bool shortName);
uint8_t getDayOfWeek(uint16_t year, uint8_t month, uint8_t day);
int8_t datecmp( DateTime *d1, DateTime *d2 );

uint8_t dateToBuf( char *buffer, uint8_t format, DateTime *date );
uint8_t timeToBuf( char *buffer, bool fmt_24h, DateTime *date );
uint8_t timeToBuf( char *buffer, bool fmt_24h, Time *time );

#endif /* TIME_H */
