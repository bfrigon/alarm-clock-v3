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
 * Initialize the DateTime class with default date and time.
 *
 * Arguments
 * ---------
 *  None
 */
DateTime::DateTime() {
    this->set( 2000, 1, 1, 0, 0, 0 );
}


/*--------------------------------------------------------------------------
 *
 * Initialize the DateTime class with date and time from another 
 * DateTime class.
 *
 * Arguments
 * ---------
 *  - src : source  DateTime object
 */
DateTime::DateTime( DateTime *src ) {
    _y = src->year();
    _m = src->month();
    _d = src->day();
    _hh = src->hour();
    _mm = src->minute();
    _ss = src->second();
}


/*--------------------------------------------------------------------------
 *
 * Initialize the DateTime class with specific date/time values
 *
 * Arguments
 * ---------
 *  - year  : Year
 *  - month : Month (1-12)
 *  - day   : day of the month (1-31*). If above the number of days 
 *            of the month, it sets the day to the last.
 *  - hour  : hour (0-23)
 *  - min   : Minutes (0-59)
 *  - sec   : Seconds (0-59)
 */
DateTime::DateTime( uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec ) {
    this->set( year, month, day, hour, min, sec );
}


/*--------------------------------------------------------------------------
 *
 * Set the date and time 
 *
 * Arguments
 * ---------
 *  - year  : Year
 *  - month : Month (1-12)
 *  - day   : day of the month (1-31*). If above the number of days 
 *            of the month, it sets the day to the last.
 *  - hour  : hour (0-23)
 *  - min   : Minutes (0-59)
 *  - sec   : Seconds (0-59)
 * 
 * Returns: Nothing
 */
void DateTime::set( uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec ) {

    if( month == 0 ) {
        month = 1;
    }

    if( month > 12 ) {
        month = 12;
    }

    if( day == 0 ) {
        day = 1;
    }

    if( day > getMonthNumDays( month, year )) {
        day = getMonthNumDays( month, year );
    }

    if( hour > 23 ) {
        hour = 23;
    }

    if( min > 59 ) {
        min = 59;
    }

    if( sec > 59 ) {
        sec = 59;
    }

    this->_y = year;
    this->_m = month;
    this->_d = day;
    this->_hh = hour;
    this->_mm = min;
    this->_ss = sec;
}


/*--------------------------------------------------------------------------
 *
 * Test if two DateTime objects are equal
 *
 * Arguments
 * ---------
 *  - right : DateTime object to compare this instance to.
 * 
 * Returns: TRUE if equal, FALSE otherwise
 */
bool DateTime::operator==(const DateTime &right) const {

    return ( right._y == _y && 
             right._m == _m &&
             right._d == _d && 
             right._hh == _hh && 
             right._mm == _mm &&
             right._ss == _ss);
}


/*--------------------------------------------------------------------------
 *
 * Test if two DateTime objects are not equal
 *
 * Arguments
 * ---------
 *  - right : DateTime object to compare this instance to.
 * 
 * Returns: TRUE if NOT equal, FALSE otherwise
 */
bool DateTime::operator!=( const DateTime &right ) const { 
    return !( *this == right ); 
}


/*--------------------------------------------------------------------------
 *
 * Test if one DateTime is less than the other
 *
 * Arguments
 * ---------
 *  - right : DateTime object to compare this instance to.
 * 
 * Returns: TRUE if less, FALSE otherwise
 */
bool DateTime::operator<( const DateTime &right ) const {

    return ( _y  < right._y ||
            ( _y == right._y &&
             ( _m < right._m ||
              ( _m == right._m &&
               ( _d < right._d ||
                ( _d == right._d &&
                 ( _hh < right._hh ||
                  ( _hh == right._hh &&
                   ( _mm < right._mm ||
                    ( _mm == right._mm && _ss < right._ss))))))))));
}


/*--------------------------------------------------------------------------
 *
 * Test if one DateTime is more than the other
 *
 * Arguments
 * ---------
 *  - right : DateTime object to compare this instance to.
 * 
 * Returns: TRUE if more, FALSE otherwise
 */
bool DateTime::operator>( const DateTime &right ) const { 
    return right < *this; 
}


/*--------------------------------------------------------------------------
 *
 * Test if one DateTime is less or equal than the other
 *
 * Arguments
 * ---------
 *  - right : DateTime object to compare this instance to.
 * 
 * Returns: TRUE if less or equal, FALSE otherwise
 */
bool DateTime::operator<=( const DateTime &right ) const {
    return !(*this > right);
}


/*--------------------------------------------------------------------------
 *
 * Test if one DateTime is more or equal than the other
 *
 * Arguments
 * ---------
 *  - right : DateTime object to compare this instance to.
 * 
 * Returns: TRUE if more or equal, FALSE otherwise
 */
bool DateTime::operator>=( const DateTime &right ) const { 
    return !(*this < right); 
}


/*--------------------------------------------------------------------------
 *
 * Adjust the date and time by the 'x' amount of seconds
 *
 * Arguments
 * ---------
 *  - offset : Number of seconds to adjust forward or backward.
 * 
 * Returns: Nothing
 */
void DateTime::offset( long offset ) {

    int8_t ss = this->_ss;
    int8_t mm = this->_mm;
    int8_t hh = this->_hh;
    long d = this->_d;
    int8_t m = this->_m;
    int16_t y = this->_y;

    ss += ( offset % 60L );
    if( ss > 59 ) {
        mm++;
        ss %= 60;
    }
    if( ss < 0 ) {
        mm--;
        ss += 60;
    }

    mm += (( offset % 3600L ) / 60L );
    if( mm > 59 ) {
        hh++;
        mm %= 60;
    }
    if( mm < 0 ) {
        hh--;
        mm += 60;
    }

    hh +=(( offset % 86400L ) / 3600L );
    if ( hh > 23 ) {
        d++;
        hh %= 24;
    }
    if( hh < 0 ) {
        d--;
        hh += 24;
    }

    d += ( offset / 86400L );
    while(( d > getMonthNumDays( m, y )) || ( d < 1 )) {
        
        if( d < 1 ) {
            m--;
            if( m < 1 ) {
                m = 12;
                y--;
            }

            d += (( long )getMonthNumDays( m, y ));

        } else {
            d -= (( long )getMonthNumDays( m, y ));
            m++;

            if( m > 12 ) {
                m = 1;
                y++;
            }
        }
    }

    this->_ss = ss;
    this->_mm = mm;
    this->_hh = hh;
    this->_d = d;
    this->_m = m;
    this->_y = y;
}



/*--------------------------------------------------------------------------
 *
 * Gets the day of week from the current date.
 *
 * Arguments
 * ---------
 *  None
 * 
 * Returns: Day of week (0=Sunday, 1=Monday ... 6=Saturday)
 */
uint8_t DateTime::dow() {
    return getDayOfWeek( _y, _m, _d);
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

    if( this->_y > 0 ) {
        days += ( this->_y * 365 ) + ( ( this->_y - 1 ) / 4 ) + 1;
    }

    uint8_t i;

    for( i = 1; i <= this->_m - 1; i++ ) {
        days += getMonthNumDays( i, this->_y );
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
 *  - shortName : TRUE to get the abbreviation or False for the full name.
 *
 * Returns : The pointer to the month name string.
 */
const char* getMonthName( uint8_t month, bool shortName ) {

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
 *  - day       : The day of week (0-6)
 *  - shortName : TRUE to get the abbreviation or False for the full name.
 *
 * Returns : The pointer to the day of week name string.
 */
const char* getDayName( uint8_t day, bool shortName ) {

    /* Validate input */
    if( day > 6 ) {
        day = 6;
    }

    if( shortName ) {

        return &_DAYS_SHORT[ day ][0];

    } else {

        return &_DAYS[ day ][0];
    }
}


/*--------------------------------------------------------------------------
 *
 * Get the number of day in a given month
 *
 * Arguments
 * ---------
 *  - month : Month (1-12) 
 *  - year  : year 
 *
 * Returns : The number of days
 */
uint8_t getMonthNumDays( uint8_t month, uint16_t year ) {

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
 *  - year  : year
 *  - month : Month (1-12) 
 *  - day   : Day (1-31)
 *
 * Returns : The day of week index from 1 to 7. (1=Sunday, 7=Saturday)
 */
uint8_t getDayOfWeek( uint16_t year, uint8_t month, uint8_t day ) {

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

    uint16_t days;
    days = ( year / 4 ) + day + months_key[ month - 1 ] + 6 + year;

    /* If leap year and month is january or febuary, subtract 1 day */
    if( ( year % 4 ) == 0 && month <= 2 ) {
        days--;
    }

    /* day 0-6 : 0=Saturday, 6=Friday, etc. */
    days = ( days % 7 );

    
    if( days == 0 ) {
        days = 7;
    }

    /* Return a value from 0 to 6 : 0=Sunday, 6=Saturday */
    return days - 1;
}


/*--------------------------------------------------------------------------
 *
 * Find the day in the month corresponding to the 'x' occurence 
 * of a weekday
 *
 * Arguments
 * ---------
 *  - year  : year
 *  - month : Month (1-12)  
 *  - dow   : Day of week to find (0=Sunday, 6=Saturday)
 *  - order : Which occurence of the day to search for 
 *            ( 1=first, 2=second, ..., 5=last )
 *
 * Returns : The day of the month matching search criteria
 */
uint8_t findDayByDow( uint16_t year, uint8_t month, uint8_t dow, uint8_t order ) {

    if( dow > 6 ) {
        dow = 6;
    }

    if( order < 1 ) {
        order = 1;
    }

    int8_t day;
    day = 1 + (dow - getDayOfWeek( year, month, 1 ));

    if( day < 1 ) {
        day += 7;
    }

    day += ( 7 * ( order - 1 ));
    while( day > getMonthNumDays( month, year )) {
        day -= 7;
    }
    
    return day;
}


/*--------------------------------------------------------------------------
 *
 * Prints the time from a given date/time structure into a string.
 *
 * Arguments
 * ---------
 *  - buffer  : Pointer to the string to write to.
 *  - fmt_24h : TRUE for 24H time format or False for am/pm.
 *  - date    : Structure containing the date and time to print.
 *
 * Returns : The number of characters written
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
 *  - fmt_24h : TRUE for 24H time format or False for am/pm.
 *  - time    : Structure containing the time to print.
 *
 * Returns : The number of characters written
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
 * Returns : The number of characters written
 */
uint8_t dateToBuf( char *buffer, uint8_t format, DateTime *date ) {

    uint8_t length;

    switch( format ) {

        case DATE_FORMAT_MMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%02d/%02d/%d" ),
                                date->month(),
                                date->day(),
                                date->year() );
            break;

        case DATE_FORMAT_YYYYMMDD:
            length = sprintf_P( buffer, PSTR( "%d/%02d/%02d" ),
                                date->year(),
                                date->month(),
                                date->day() );
            break;

        case DATE_FORMAT_DDMMMYYYY:
            length = sprintf_P( buffer, PSTR( "%02d-%S-%d" ),
                                date->day(),
                                getMonthName( date->month(), true ),
                                date->year() );
            break;

        case DATE_FORMAT_MMMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%S-%02d-%d" ),
                                getMonthName( date->month(), true ),
                                date->day(),
                                date->year() );
            break;

        case DATE_FORMAT_YYYYMMMDD:
            length = sprintf_P( buffer, PSTR( "%d-%S-%02d" ),
                                date->year(),
                                getMonthName( date->month(), true ),
                                date->day() );
            break;

        case DATE_FORMAT_WDMMMDD:
            length = sprintf_P( buffer, PSTR( "%S, %S %d" ),
                                getDayName( date->dow(), true ),
                                getMonthName( date->month(), true ),
                                date->day() );
            break;

        case DATE_FORMAT_WDMMMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%S, %S %d %d" ),
                                getDayName( date->dow(), true ),
                                getMonthName( date->month(), true ),
                                date->day(),
                                date->year() );
            break;

        /* DATE_FORMAT_DDMMYYYY */
        default:
            length = sprintf_P( buffer, PSTR( "%02d/%02d/%d" ),
                                date->day(),
                                date->month(),
                                date->year() );
            break;
    }

    return length;
}