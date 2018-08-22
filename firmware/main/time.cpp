//******************************************************************************
//
// Project : Alarm Clock V3
// File    : time.cpp
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


#include "time.h"
#include "resources.h"


const char* getMonthName(uint8_t month, bool shortName) {

    if ( month < 1 )  month = 1;
    if ( month > 12 ) month = 12;

    if ( shortName ) {

        return &_MONTHS_SHORT[ month - 1 ][0];

    } else {

        return &_MONTHS[ month - 1 ][0];
    }
}

const char* getDayName( uint8_t day, bool shortName ) {

    if ( shortName ) {

        return &_DAYS_SHORT[ day - 1 ][0];

    } else {

        return &_DAYS[ day - 1 ][0];
    }
}


uint8_t getDayOfWeek(uint16_t year, uint8_t month, uint8_t day) {

    /* TODO */

    return 0;
}

int8_t datecmp( DateTime *d1, DateTime *d2 ) {



    return 0;
}


uint8_t timeToBuf( char *buffer, bool fmt_24h, DateTime *date ) {

    Time time;
    time.hour = date->hour();
    time.minute = date->minute();

    return timeToBuf( buffer, fmt_24h, &time );
}

uint8_t timeToBuf( char *buffer, bool fmt_24h, Time *time ) {
    uint8_t length;

    if ( fmt_24h == false ) {
        bool is_pm = ( time->hour >= 12 );
        uint8_t hour = time->hour % 12;

        if ( hour == 0 ) {
            hour = 12;
        }

        length = sprintf_P( buffer, PSTR( "%d:%02d %S" ), hour, time->minute, ( is_pm ? S_PM : S_AM ));

    } else {
        length = sprintf_P( buffer, PSTR( "%d:%02d" ), time->hour, time->minute );
    }

    return length; 
}

uint8_t dateToBuf( char *buffer, uint8_t format, DateTime *date ) {

    uint8_t length;

    switch ( format ) {

        case DATE_FORMAT_MMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%02d/%02d/%d" ),
                                date->month(),
                                date->date(),
                                date->year() );
            break;

        case DATE_FORMAT_YYYYMMDD:
            length = sprintf_P( buffer, PSTR( "%d/%02d/%02d" ),
                                date->year(),
                                date->month(),
                                date->date() );
            break;

        case DATE_FORMAT_DDMMMYYYY:
            length = sprintf_P( buffer, PSTR( "%02d-%S-%d" ),
                                date->date(),
                                getMonthName( date->month(), true ),
                                date->year() );
            break;

        case DATE_FORMAT_MMMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%S-%02d-%d" ),
                                getMonthName( date->month(), true ),
                                date->date(),
                                date->year() );
            break;

        case DATE_FORMAT_YYYYMMMDD:
            length = sprintf_P( buffer, PSTR( "%d-%S-%02d" ),
                                date->year(),
                                getMonthName( date->month(), true ),
                                date->date() );
            break;

        case DATE_FORMAT_WDMMMDD:
            length = sprintf_P( buffer, PSTR( "%S, %S %d"),
                                getDayName( date->dayOfWeek(), true ),
                                getMonthName( date->month(), true ),
                                date->date() );
            break;

        case DATE_FORMAT_WDMMMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%S, %S %d %d"),
                                getDayName( date->dayOfWeek(), true ),
                                getMonthName( date->month(), true ),
                                date->date(),
                                date->year() );
            break;

        /* DATE_FORMAT_DDMMYYYY */
        default:
            length = sprintf_P( buffer, PSTR( "%02d/%02d/%d" ),
                                date->date(),
                                date->month(),
                                date->year() );
            break;
    }

    return length;
}
