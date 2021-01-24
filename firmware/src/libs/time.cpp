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


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the DateTime class with default date and time.
 *
 */
DateTime::DateTime() {
    this->set( 2000, 1, 1, 0, 0, 0 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the DateTime class with date and time from another 
 *          DateTime class.
 *
 * @param   src    source  DateTime object
 * 
 */
DateTime::DateTime( DateTime *src ) {
    _y = src->year();
    _m = src->month();
    _d = src->day();
    _hh = src->hour();
    _mm = src->minute();
    _ss = src->second();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the DateTime class with specific date/time values
 *
 * @param   year     Year
 * @param   month    Month (1-12)
 * @param   day      day of the month (1-31*). If above the number of days 
 *                   of the month, it sets the day to the last.
 * @param   hour     hour (0-23)
 * @param   min      Minutes (0-59)
 * @param   sec      Seconds (0-59)
 * 
 */
DateTime::DateTime( uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec ) {
    this->set( year, month, day, hour, min, sec );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the date and time 
 *
 * @param   year     Year
 * @param   month    Month (1-12)
 * @param   day      day of the month (1-31*). If above the number of days 
 *                   of the month, it sets the day to the last.
 * @param   hour     hour (0-23)
 * @param   min      Minutes (0-59)
 * @param   sec      Seconds (0-59)
 * 
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

    _y = year;
    _m = month;
    _d = day;
    _hh = hour;
    _mm = min;
    _ss = sec;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Test if two DateTime objects are equal
 *
 * @param   right    DateTime object to compare this instance to.
 * 
 * @return  TRUE if equal, FALSE otherwise
 * 
 */
bool DateTime::operator==(const DateTime &right) const {

    return ( right._y == _y && 
             right._m == _m &&
             right._d == _d && 
             right._hh == _hh && 
             right._mm == _mm &&
             right._ss == _ss);
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Test if two DateTime objects are not equal
 *
 * @param   right    DateTime object to compare this instance to.
 * 
 * @return  TRUE if NOT equal, FALSE otherwise
 * 
 */
bool DateTime::operator!=( const DateTime &right ) const { 
    return !( *this == right ); 
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Test if one DateTime is less than the other
 *
 * @param   right    DateTime object to compare this instance to.
 * 
 * @return  TRUE if less, FALSE otherwise
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Test if one DateTime is more than the other
 *
 * @param   right    DateTime object to compare this instance to.
 * 
 * @return  TRUE if more, FALSE otherwise
 * 
 */
bool DateTime::operator>( const DateTime &right ) const { 
    return right < *this; 
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Test if one DateTime is less or equal than the other
 *
 * @param   right    DateTime object to compare this instance to.
 * 
 * @return  TRUE if less or equal, FALSE otherwise
 * 
 */
bool DateTime::operator<=( const DateTime &right ) const {
    return !(*this > right);
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Test if one DateTime is more or equal than the other
 *
 * @param   right    DateTime object to compare this instance to.
 * 
 * @return  TRUE if more or equal, FALSE otherwise
 * 
 */
bool DateTime::operator>=( const DateTime &right ) const { 
    return !(*this < right); 
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Adjust the date and time by the 'x' amount of seconds
 *
 * @param   offset    Number of seconds to adjust forward or backward.
 * 
 */
void DateTime::offset( long offset ) {

    if( offset == 0 ) {
        return;
    }

    int8_t ss = _ss;
    int8_t mm = _mm;
    int8_t hh = _hh;
    long d = _d;
    int8_t m = _m;
    int16_t y = _y;

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

    _ss = ss;
    _mm = mm;
    _hh = hh;
    _d = d;
    _m = m;
    _y = y;
}



/*! ------------------------------------------------------------------------
 *
 * @brief   Gets the day of week from the current date.
 *
 * @return  Day of week (0=Sunday, 1=Monday ... 6=Saturday)
 * 
 */
uint8_t DateTime::dow() {
    return getDayOfWeek( _y, _m, _d);
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Calculate the unix epoch
 * 
 * @details This function interpolate the unix time based on the date and 
 *          time stored in the DateTime structure. It accounts for leap year 
 *          but Ignores time zone. It always assume it is UTC.
 *
 * @return  The unix epoch.
 * 
 */
unsigned long DateTime::getEpoch() {

    unsigned long days;
    
    /* Number of days since 1970 */
    days = (( _y - 1970 ) * 365 );        

    /* Add number of leap years since 1972 */
    days += (( _y - 1972 ) / 4 );         
    days += ( _y % 4 ) ? 1 : 0;           

    /* Add number of days since the begining of the current year 
       until the start of the current month */
    uint8_t i;
    for( i = 1; i <= _m - 1; i++ ) {
        days += getMonthNumDays( i, _y );
    }

    /* Add number of days in the current month */
    days += ( _d - 1 );                   

    return ( days * 86400L ) + ( _hh * 3600L ) + ( _mm * 60L ) + _ss;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the pointer to the string stored in program memory which 
 *          represents the given month.
 *
 * @param   month        The month (1-12)
 * @param   shortName    TRUE to get the abbreviation or False for the 
 *                       full name.
 *
 * @return  The pointer to the month name string.
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the pointer to the string stored in program memory which 
 *          represents the given day of week.
 *
 * @param   day          The day of week (0-6)
 * @param   shortName    TRUE to get the abbreviation or False for the 
 *                       full name.
 *
 * @return  The pointer to the day of week name string.
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the number of day in a given month
 *
 * @param   month    Month (1-12) 
 * @param   year     year 
 *
 * @return  The number of days
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the day of week from a given date. 
 *
 * @param   year     year
 * @param   month    Month (1-12) 
 * @param   day      Day (1-31)
 *
 * @return  The day of week index from 1 to 7. (1=Sunday, 7=Saturday)
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Find the day in the month corresponding to the 'x' occurence 
 *          of a weekday
 *
 * @param   year     year
 * @param   month    Month (1-12)  
 * @param   dow      Day of week to find (0=Sunday, 6=Saturday)
 * @param   order    Which occurence of the day to search for 
 *                   ( 1=first, 2=second, ..., 5=last )
 *
 * @return  The day of the month matching search criteria
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints the time from a given date/time structure into a string.
 *
 * @param   buffer     Pointer to the string to write to.
 * @param   fmt_24h    TRUE for 24H time format or False for am/pm.
 * @param   date       Structure containing the date and time to print.
 *
 * @return  The number of characters written
 * 
 */
uint8_t timeToBuf( char *buffer, bool fmt_24h, DateTime *date ) {

    Time time;
    time.hour = date->hour();
    time.minute = date->minute();

    return timeToBuf( buffer, fmt_24h, &time );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints the given time into a string.
 *
 * @param   buffer     Pointer to the string to write to.
 * @param   fmt_24h    TRUE for 24H time format or False for am/pm.
 * @param   time       Structure containing the time to print.
 *
 * @return  The number of characters written
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints the date from a given date/time structure into a string 
 *          unsign a specific format.
 *
 * @param   buffer     Pointer to the string to write to.
 * @param   format     Date format to use.
 * @param   date       Structure containing the date to print
 *
 * @return  The number of characters written
 * 
 */
uint8_t dateToBuf( char *buffer, uint8_t format, DateTime *date ) {

    uint8_t length;

    switch( format ) {

        case DATETIME_FORMAT_MMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%02d/%02d/%d" ),
                                date->month(),
                                date->day(),
                                date->year() );
            break;

        case DATETIME_FORMAT_YYYYMMDD:
            length = sprintf_P( buffer, PSTR( "%d/%02d/%02d" ),
                                date->year(),
                                date->month(),
                                date->day() );
            break;

        case DATETIME_FORMAT_DDMMMYYYY:
            length = sprintf_P( buffer, PSTR( "%02d-%S-%d" ),
                                date->day(),
                                getMonthName( date->month(), true ),
                                date->year() );
            break;

        case DATETIME_FORMAT_MMMDDYYYY:
            length = sprintf_P( buffer, PSTR( "%S-%02d-%d" ),
                                getMonthName( date->month(), true ),
                                date->day(),
                                date->year() );
            break;

        case DATETIME_FORMAT_YYYYMMMDD:
            length = sprintf_P( buffer, PSTR( "%d-%S-%02d" ),
                                date->year(),
                                getMonthName( date->month(), true ),
                                date->day() );
            break;

        case DATETIME_FORMAT_WDMMMDD:
            length = sprintf_P( buffer, PSTR( "%S, %S %d" ),
                                getDayName( date->dow(), true ),
                                getMonthName( date->month(), true ),
                                date->day() );
            break;

        case DATETIME_FORMAT_WDMMMDDYYYY:
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Parse an integer value from a given array of character
 *
 * @param   buf       Pointer to the string to parse.
 * @param   dest      Pointer to an integer which hold the result.
 * @param   min       lower acceptable limit of the integer to parse.
 * @param   max       upper acceptable limit of the integer to parse.
 * @param   digits    Maximum number of digits the integer must have.
 *
 * @return  The pointer on the character array at the position where the number
 *          ends if successful. Returns NULL when the buffer does not contain a 
 *          valid number or if the parsed number is outside the defined range.
 * 
 */
const char* parse_int( const char *buf, int *dest, int min, int max, uint8_t digits )  {

    int result = 0; 
    unsigned char ch; 


  
    ch = *buf; 
    if (ch < '0' || ch > '9') {
        return NULL; 
    }
 
    do { 
        result *= 10; 
        result += ch - '0'; 
        digits--;
        ch = *++buf; 
    } while (( result * 10 <= max ) && digits && ch >= '0' && ch <= '9'); 
 
    if (result < min || result > max) {
        return NULL; 
    }
 
    *dest = result; 
    return buf; 
} 


/*! ------------------------------------------------------------------------
 *
 * @brief   convert a character array representation of time to a DateTime 
 *          structure
 *
 * @param   buf     Pointer to the character array to convert.
 * @param   fmt     Pointer to a character array containing the format used to 
 *                  parse the date/time string
 * @param   dest    DateTime object to store the results to
 *
 * @return  Pointer to the first character not processed by this function 
 *          or NULL if it fails to match all of the format string.
 * 
 */
const char* strptime( const char *buf, const char *fmt, DateTime* dest ) {
    uint16_t year = dest->year();
    uint8_t month = dest->month();
    uint8_t day = dest->day();
    uint8_t hour = dest->hour();
    uint8_t min = dest->minute();
    uint8_t sec = dest->second();
    int res;

    
    
    while(( *fmt != '\0' ) && ( buf != NULL )) {

        if( isspace( *fmt )) {
            while( isspace( *buf++ ));
            continue;
        }

        if( *fmt != '%' ) {
            if( *fmt != *buf++ ) {
                return NULL;
            }
            
            fmt++;
            continue;
        } 

        switch( *++fmt ) {

            /* Match the '%' character */
            case '%':
                if( *fmt != *buf++ ) {
                    return NULL;
                }

                fmt++;
                continue;

            /* Year */
            case 'Y':
                buf = parse_int( buf, &res, 0, 9999, 4 );
                year = res;
                break;

            /* Month (1-12) */
            case 'm':
                buf = parse_int( buf, &res, 1, 12, 2);
                month = res;
                break;

            /* Day (1-31) */
            case 'd':
                buf = parse_int( buf, &res, 1, 31, 2);
                day = res;
                break;

            /* Hour (0-23) */
            case 'H':
                buf = parse_int( buf, &res, 0, 23, 2);
                hour = res;
                break;

            /* Minute (0-59) */
            case 'M':
                buf = parse_int( buf, &res, 0, 59, 2);
                min = res;
                break;

            /* Seconds (0-59) */
            case 'S':
                buf = parse_int( buf, &res, 0, 59, 2);
                sec = res;
                break;

            /* Unsuppoted conversion */
            default: 
                return NULL;

        }

        fmt++;
    }

    if( buf == NULL ) {
        return NULL;
    }

    dest->set( year, month, day, hour, min, sec );

    return buf;
}