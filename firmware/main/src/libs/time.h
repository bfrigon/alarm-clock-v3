//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/time.h
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

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "../resources.h"



#define EPOCH_Y2K_OFFSET            946684800

#define DATE_FORMAT_DDMMYYYY        0
#define DATE_FORMAT_MMDDYYYY        1
#define DATE_FORMAT_YYYYMMDD        2
#define DATE_FORMAT_DDMMMYYYY       3
#define DATE_FORMAT_MMMDDYYYY       4
#define DATE_FORMAT_YYYYMMMDD       5
#define DATE_FORMAT_WDMMMDD         6
#define DATE_FORMAT_WDMMMDDYYYY     7


#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ))


struct Time {
    uint8_t hour;
    uint8_t minute;
};

struct Date {
    uint8_t day;
    uint8_t month;
    uint8_t year;
};

class DateTime {
  public:
    DateTime();
    DateTime( uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec, uint8_t dow );

    uint8_t second() const  {
        return this->_ss;
    }
    uint8_t minute() const  {
        return this->_mm;
    }
    uint8_t hour() const    {
        return this->_hh;
    }

    uint8_t date() const    {
        return this->_d;
    }
    uint8_t month() const   {
        return this->_m;
    }
    uint16_t year() const   {
        return 2000 + this->_year;
    }

    uint8_t dow() const     {
        return this->_dow;    /*Sunday=1 */
    }

    unsigned long getEpoch();

  protected:
    uint8_t _year;
    uint8_t _m;
    uint8_t _d;
    uint8_t _hh;
    uint8_t _mm;
    uint8_t _ss;
    uint8_t _dow;
};


const char *getMonthName( uint8_t month, bool shortName );
const char *getDayName( uint8_t day, bool shortName );
uint8_t getDayOfWeek( uint8_t year, uint8_t month, uint8_t day );
uint8_t getMonthNumDays( uint8_t month, uint8_t year );

uint8_t dateToBuf( char *buffer, uint8_t format, DateTime *date );
uint8_t timeToBuf( char *buffer, bool fmt_24h, DateTime *date );
uint8_t timeToBuf( char *buffer, bool fmt_24h, Time *time );

#endif /* TIME_H */
