//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/time.cpp
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



/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  None
 */
DateTime::DateTime() {

}


/*--------------------------------------------------------------------------
 *
 * Class constructor. Initialize the class with a specific date/time
 *
 * Arguments
 * ---------
 *  - year  : Year (0-99, assumes 2000)
 *  - month : Month (1-12)
 *  - date  : date (1-31)
 *  - hour  : hour (0-23)
 *  - min   : Minutes (0-59)
 *  - sec   : Seconds (0-59)
 *  - dow   : Day of week (1-7 : 1=Sunday, 7=Saturday)
 */
DateTime::DateTime( uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec, uint8_t dow ) {
    if( month == 0 ) {
        month = 1;
    }

    if( month > 12 ) {
        month = 12;
    }

    if( date == 0 ) {
        date = 1;
    }

    if( year < 2000 ) {
        year = 2000;
    }
    if( year > 2199 ) {
        year = 2199;
    }

    this->_year = year - 2000;
    this->_m = month;
    this->_d = date;
    this->_hh = hour;
    this->_mm = min;
    this->_ss = sec;
    this->_dow = dow;
}


/*--------------------------------------------------------------------------
 *
 * This function interpolate the unix time based on the date and time stored 
 * in the DateTime structure. It accounts for leap year but Ignores
 * time zone. It always assume it is UTC.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : The unix time.
 */
unsigned long DateTime::getEpoch() {
    uint16_t days = this->_d - 1;

    if( this->_year > 0 ) {
        days += ( this->_year * 365 ) + ( ( this->_year - 1 ) / 4 ) + 1;
    }

    uint8_t i;

    for( i = 1; i <= this->_m - 1; i++ ) {
        days += getMonthNumDays( i, this->_year );
    }

    return EPOCH_Y2K_OFFSET + ( days * 86400L ) + ( this->_hh * 3600L ) + ( this->_mm * 60L ) + this->_ss;
}


/*--------------------------------------------------------------------------
 *
 * Get the pointer to the string stored in program memory which represents 
 * the given month.
 *
 * Arguments
 * ---------
 *  - month     : The month (1-12)
 *  - shortName : TRUE to get the abreviationor False for the full name.
 *
 * Returns : The pointer to the month name string.
 */
const char *getMonthName( uint8_t month, bool shortName ) {

    /* Validate input */
    if( month < 1 ) {
        month = 1;
    }

    if( month > 12 ) {
        month = 12;
    }

    if( shortName ) {

        return &_MONTHS_SHORT[ month - 1 ][0];

    } else {

        return &_MONTHS[ month - 1 ][0];
    }
}


/*--------------------------------------------------------------------------
 *
 * Get the pointer to the string stored in program memory which represents 
 * the given day of week.
 *
 * Arguments
 * ---------
 *  - day       : The day of week (1-7)
 *  - shortName : TRUE to get the abreviationor False for the full name.
 *
 * Returns : The pointer to the day of week name string.
 */
const char *getDayName( uint8_t day, bool shortName ) {

    /* Validate input */
    if( day < 1 ) {
        day = 1;
    }

    if( day > 7 ) {
        day = 7;
    }

    if( shortName ) {

        return &_DAYS_SHORT[ day - 1 ][0];

    } else {

        return &_DAYS[ day - 1 ][0];
    }
}


/*--------------------------------------------------------------------------
 *
 * Get the number of day in a given month
 *
 * Arguments
 * ---------
 *  - month : Month (1-12) 
 *  - year  : year (0-99, assume year 2000)
 *
 * Returns : The number of days
 */
uint8_t getMonthNumDays( uint8_t month, uint8_t year ) {

    if( month == 0 ) {
        month = 1;
    }

    if( month > 12 ) {
        month = 12;
    }

    const uint8_t month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if( year % 4 == 0 && month == 2 ) {
        return 29;

    } else {
        return month_days[ month - 1 ];
    }
}


/*--------------------------------------------------------------------------
 *
 * Get the day of week from a given date. 
 *
 * Arguments
 * ---------
 *  - year  : year (0-99, assume year 2000)
 *  - month : Month (1-12) 
 *  - day   : Day (1-31)
 *
 * Returns : The day of week index from 1 to 7. (1=Sunday, 7=Saturday)
 */
uint8_t getDayOfWeek( uint8_t year, uint8_t month, uint8_t day ) {

    /*
    Key value method implementation.
    (http://mathforum.org/dr.math/faq/faq.calendar.html)

    Calculation :
    ((year / 4) + day + [month key] + [year key] + year - [ 1 if leap year ] ) % 7

    Assume year is 2000 to 2099. Year key for 2000 is 6
    2000=6, 2100=4, 2200=2, 2300=0, 2400=6...
    */


    /* Validate input */
    if( year > 99 ) {
        year = year % 100;
    }

    if( month == 0 ) {
        month = 1;
    }

    if( month > 12 ) {
        month = 12;
    }

    if( day > 31 ) {
        day = 31;
    }

    if( day == 0 ) {
        day = 1;
    }

    uint8_t months_key[] = {
        1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6
    };

    uint8_t days;
    days = ( year / 4 ) + day + months_key[ month - 1 ] + 6 + year;

    /* If leap year and month is january or febuary, subtract 1 day */
    if( ( year % 4 ) == 0 && month <= 2 ) {
        days--;
    }

    /* day 0-6 : 0=Saturday, 6=Friday, etc. */
    days = ( days % 7 );

    /* Return a value from 1 to 7 : 1=Sunday, 7=Saturday */
    if( days == 0 ) {
        days = 7;
    }

    return days;
}



/*--------------------------------------------------------------------------
 *
 * Prints the time from a given date/time structure into a string.
 *
 * Arguments
 * ---------
 *  - buffer  : Pointer to the string to write to.
 *  - fmt_24h : TRUE for 24H time formator False for am/pm.
 *  - date    : Structure containing the date and time to print.
 *
 * Returns : The day of week index from 1 to 7. (1=Sunday, 7=Saturday)
 */
uint8_t timeToBuf( char *buffer, bool fmt_24h, DateTime *date ) {

    Time time;
    time.hour = date->hour();
    time.minute = date->minute();

    return timeToBuf( buffer, fmt_24h, &time );
}


/*--------------------------------------------------------------------------
 *
 * Prints the given time into a string.
 *
 * Arguments
 * ---------
 *  - buffer  : Pointer to the string to write to.
 *  - fmt_24h : TRUE for 24H time formator False for am/pm.
 *  - time    : Structure containing the time to print.
 *
 * Returns : The day of week index from 1 to 7. (1=Sunday, 7=Saturday)
 */
uint8_t timeToBuf( char *buffer, bool fmt_24h, Time *time ) {
    uint8_t length;

    if( fmt_24h == false ) {
        bool is_pm = ( time->hour >= 12 );
        uint8_t hour = time->hour % 12;

        if( hour == 0 ) {
            hour = 12;
        }

        length = sprintf_P( buffer, PSTR( "%d:%02d %S" ), hour, time->minute, ( is_pm ? S_PM : S_AM ) );

    } else {
        length = sprintf_P( buffer, PSTR( "%d:%02d" ), time->hour, time->minute );
    }

    return length;
}


/*--------------------------------------------------------------------------
 *
 * Prints the date from a given date/time structure into a string unsign
 * a specific format.
 *
 * Arguments
 * ---------
 *  - buffer  : Pointer to the string to write to.
 *  - format  : Date format to use.
 *  - date    : Structure containing the date to print
 *
 * Returns : The day of week index from 1 to 7. (1=Sunday, 7=Saturday)
 */
uint8_t dateToBuf( char *buffer, uint8_t format, DateTime *date ) {

    uint8_t length;

    switch( format ) {

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
            length = sprintf_P( buffer, PSTR( "%S, %S %d" ),
                                getDayName( date->dow(), true ),
                                getMonthName( date->month(), true ),
                                date->date() );
            break;

        case DATE_FORMAT_WDMMMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%S, %S %d %d" ),
                                getDayName( date->dow(), true ),
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