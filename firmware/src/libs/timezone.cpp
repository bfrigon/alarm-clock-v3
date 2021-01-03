//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/timezone.cpp
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
#include "timezone.h"
#include "../console/console.h"
#include "../config.h"


/*--------------------------------------------------------------------------
 *
 * Initialize class with UTC as the default timezone
 *
 * Arguments
 * ---------
 *  None
 */
TimeZone::TimeZone() {
    _id = 0;

    /* set default timezone to Etc/UTC */
    memcpy_P( &_tz, &TimeZonesTable[ 0 ], sizeof( TimeZoneRules ));
}


/*--------------------------------------------------------------------------
 *
 * Sets the current timezone ID
 *
 * Arguments
 * ---------
 *  - zone_id : Timezone index in the timezone table
 *
 * Returns : Nothing
 */
bool TimeZone::setTimezoneByID( uint16_t id ) {

    if( id > MAX_TIMEZONE_ID - 1 ) {
        return false;
    }

    _id = id;

    memcpy_P( &_tz, &TimeZonesTable[ id ], sizeof( TimeZoneRules ));
    strcpy_P( g_config.clock.timezone, this->getName() );

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Sets the current timezone by name
 *
 * Arguments
 * ---------
 *  - name : Name of the timezone.
 *
 * Returns : TRUE if timezone is found, FALSE otherwise
 */
bool TimeZone::setTimezoneByName( char *name ) {
    int16_t id;
    id = this->findTimezoneByName( name );

    if( id < 0 ) {
        return false;
    }

    this->setTimezoneByID( id );
}


/*--------------------------------------------------------------------------
 *
 * Find a timezone index in the table from it's name.
 *
 * Arguments
 * ---------
 *  - name : Pointer to an array of character containg the name to 
 *           search for
 *
 * Returns : The timezone index in the table
 */
int16_t TimeZone::findTimezoneByName( char* name ) {

    int16_t i;
    for( i = 0; i < MAX_TIMEZONE_ID; i++ ) {
        char *tz_name_ptr;
        tz_name_ptr = (char *)pgm_read_word( &( TimeZonesTable[ i ].name ));

        if ( strcasecmp_P( name, tz_name_ptr ) == 0 ) {
            return i;
        }
    }

    return -1;
}


/*--------------------------------------------------------------------------
 *
 * Get the current timezone name
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Pointer to the array of character in program memory containing
 *           the name of the timezone.
 */
const char* TimeZone::getName() {
    return _tz.name;
}


/*--------------------------------------------------------------------------
 *
 * Get the current timezone abbreviation whether it is currently on standard
 * time or daylight saving time.
 *
 * Arguments
 * ---------
 *  local : Pointer to a DateTime object containing local time.
 *
 * Returns : Pointer to the array of character in program memory containing
 *           the STD or DST abbreviation.
 */
const char* TimeZone::getAbbreviation( DateTime *local ) {

    if( local == 0 ) {
        return 0;
    }

    return ( this->isDST( local ) == true ? _tz.dst_abbvr : _tz.std_abbvr );
}


/*--------------------------------------------------------------------------
 *
 * Convert a DateTime object from UTC to local time for the current timezone
 *
 * Arguments
 * ---------
 *  utc : Pointer to a DateTime object containing UTC time.
 *
 * Returns : Nothing
 */
void TimeZone::toLocal( DateTime *utc ) {

    DateTime local;
    

    /* Convert time to local using standard time */
    local = *utc;
    local.offset( _tz.std_offset * 60L );

    
    if( this->isDST( &local )) {

        /* If local time is inside the daylight period, 
           convert utc to local using daylight time instead */    
        local = *utc;
        local.offset( _tz.dst_offset * 60L );

        /* If convertion occurs during a transition from DST to STD,
           re-convert utc to local using standard time */
        if( this->isDST( &local ) == false ) {
            local = *utc;
            local.offset( _tz.std_offset * 60L );
        }
    }

    *utc = local;
}


/*--------------------------------------------------------------------------
 *
 * Convert a DateTime object from local time to UTC for the current timezone
 *
 * Arguments
 * ---------
 *  utc : Pointer to a DateTime object containing local time time.
 *
 * Returns : Nothing
 */
void TimeZone::toUTC( DateTime *local ) {
    DateTime utc;
    
    utc = *local;
    if( this->isDST( local )) {

        /* If local time is inside the daylight period, 
           convert local to UTC using daylight time instead */    
        
        utc.offset( -( _tz.dst_offset * 60L ));

    } else {

        /* Convert time to UTC using standard time */
        utc.offset( -( _tz.std_offset * 60L ));
    }

    *local = utc;
}


/*--------------------------------------------------------------------------
 *
 * Determine whether or not the local time is currently in a daylight 
 * saving period
 *
 * Arguments
 * ---------
 *  local : Pointer to a DateTime object containing local time.
 *
 * Returns : TRUE if local time is on daylight saving time, FALSE otherwise
 */
bool TimeZone::isDST( DateTime *local ) {

    DateTime dst, std;
    this->getStdTransition( local->year(), &std );
    this->getDstTransition( local->year(), &dst );

    /* DST not observed in this time zone */
    if( dst == std ) {
        return false;

    /* Northen hemisphere DST */
    } else if( std > dst ) {

        return ( *local >= dst && *local < std );

    /* Southern hemisphere DST */
    } else {

        return ( *local < std || *local >= dst );
    }
}


/*--------------------------------------------------------------------------
 *
 * Set the specified DateTime object to the DST->STD transition for this
 * timezone
 *
 * Arguments
 * ---------
 *  year : Current year
 *  std  : DateTime object to write the result to
 *
 * Returns : Nothing
 */
void TimeZone::getStdTransition( int16_t year, DateTime *std ) {
    uint8_t std_day;
    if( _tz.std_week == 0 ) {
        std_day = _tz.dst_dow;
    } else {
        std_day = findDayByDow( year, _tz.std_month, _tz.std_dow, _tz.std_week );
    }

    std->set( year, _tz.std_month, std_day, _tz.std_hour % 24, _tz.std_min, 0 );

    if( _tz.std_hour > 23 ) {
        std->offset( 86400 );
    }
}


/*--------------------------------------------------------------------------
 *
 * Set the specified DateTime object to the STD->DST transition for this
 * timezone
 *
 * Arguments
 * ---------
 *  year : Current year
 *  std  : DateTime object to write the result to
 *
 * Returns : Nothing
 */
void TimeZone::getDstTransition( int16_t year, DateTime *dst ) {
    uint8_t dst_day;
    if( _tz.dst_week == 0 ) {
        dst_day = _tz.dst_dow;
    } else {
        dst_day = findDayByDow( year, _tz.dst_month, _tz.dst_dow, _tz.dst_week );
    }

    dst->set( year, _tz.dst_month, dst_day, _tz.dst_hour % 24, _tz.dst_min, 0 );

    if( _tz.dst_hour > 23 ) {
        dst->offset( 86400 );
    }
}


/*--------------------------------------------------------------------------
 *
 * Get the standard time offset from UTC
 *
 * Arguments
 * ---------
 *  - None
 *
 * Returns : Offset in minutes
 */
int16_t TimeZone::getStdUtcOffset() {
    return _tz.std_offset;
}


/*--------------------------------------------------------------------------
 *
 * Get the daylight saving time offset from UTC
 *
 * Arguments
 * ---------
 *  - None
 *
 * Returns : Offset in minutes
 */
int16_t TimeZone::getDstUtcOffset() {
    return _tz.dst_offset;
}