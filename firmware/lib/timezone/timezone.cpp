//******************************************************************************
//
// Project : Alarm Clock V3
// File    : lib/timezone/timezone.cpp
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
#include <tzdata.h>


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize class with UTC as the default timezone
 *
 */
TimeZone::TimeZone() {

    /* set default timezone to Etc/UTC */
    this->setTimezoneByID( 0 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the current timezone ID
 *
 * @param   zone_id    Timezone index in the timezone table
 * 
 */
bool TimeZone::setTimezoneByID( uint16_t id ) {

    if( id > MAX_TIMEZONE_ID - 1 ) {
        return false;
    }

    _id = id;

    memcpy_P( &_tz, &TimeZonesTable[ id ], sizeof( TimeZoneRules ));

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the current timezone by name
 *
 * @param   name    Name of the timezone.
 *
 * @return  TRUE if timezone is found, FALSE otherwise
 * 
 */
bool TimeZone::setTimezoneByName( char *name ) {
    int16_t id;
    id = findTimezoneByName( name );

    if( id < 0 ) {
        return false;
    }

    this->setTimezoneByID( id );

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the current timezone name
 *
 * @return  Pointer to the array of character in program memory containing
 *          the name of the timezone.
 * 
 */
const char* TimeZone::getName() {
    return _tz.name;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the current timezone abbreviation whether it is currently 
 *          on standard time or daylight saving time.
 *
 * @param   local    Pointer to a DateTime object containing local time.
 *
 * @return  Pointer to the array of character in program memory containing
 *           the STD or DST abbreviation.
 * 
 */
const char* TimeZone::getAbbreviation( DateTime *local ) {

    if( local == 0 ) {
        return 0;
    }

    return ( this->isDST( local ) == true ? _tz.dst_abbvr : _tz.std_abbvr );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Convert a DateTime object from UTC to local time for the 
 *          current timezone
 *
 * @param   utc    Pointer to a DateTime object containing UTC time.
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Convert a DateTime object from local time to UTC for the 
 *          current timezone
 *
 * @param   utc    Pointer to a DateTime object containing local time time.
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Determine whether or not the local time is currently 
 *          in a daylight saving period
 *
 * @param   local    Pointer to a DateTime object containing local time.
 *
 * @return  TRUE if local time is on daylight saving time, FALSE otherwise
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the specified DateTime object to the DST->STD transition 
 *          for this timezone.
 *
 * @param   year    Current year
 * @param   std     DateTime object to write the result to
 * 
 */
void TimeZone::getStdTransition( int16_t year, DateTime *std ) {
    uint8_t std_day;
    if( _tz.std_week == 0 ) {
        std_day = _tz.dst_dow;
    } else {
        std_day = findDayByDow( year, _tz.std_month, _tz.std_dow, _tz.std_week );
    }

    uint8_t hour;
    if( _tz.std_hour < 0 ) {
        hour = 24 - abs( _tz.std_hour );
    } else {
        hour = _tz.std_hour % 24;
    }

    std->set( year, _tz.std_month, std_day, hour, _tz.std_min, 0 );

    if( _tz.std_hour > 23 ) {
        std->offset( 86400 );
    }
    if( _tz.std_hour < 0 ) {
        std->offset( -86400 );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the specified DateTime object to the STD->DST transition 
 *          for this timezone.
 *
 * @param   year    Current year
 * @param   std     DateTime object to write the result to
 * 
 */
void TimeZone::getDstTransition( int16_t year, DateTime *dst ) {
    uint8_t dst_day;
    if( _tz.dst_week == 0 ) {
        dst_day = _tz.dst_dow;
    } else {
        dst_day = findDayByDow( year, _tz.dst_month, _tz.dst_dow, _tz.dst_week );
    }

    uint8_t hour;
    if( _tz.dst_hour < 0 ) {
        hour = 24 - abs( _tz.dst_hour );
    } else {
        hour = _tz.dst_hour % 24;
    }

    dst->set( year, _tz.dst_month, dst_day, hour, _tz.dst_min, 0 );

    if( _tz.dst_hour > 23 ) {
        dst->offset( 86400 );
    }

    if( _tz.dst_hour < 0 ) {
        dst->offset( -86400 );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the standard time offset from UTC
 *
 * @return  Offset in minutes
 * 
 */
int16_t TimeZone::getStdUtcOffset() {
    return _tz.std_offset;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the daylight saving time offset from UTC
 *
 * @return  Offset in minutes
 * 
 */
int16_t TimeZone::getDstUtcOffset() {
    return _tz.dst_offset;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Find a timezone index in the table from it's name.
 *
 * @param   name   Pointer to an array of character containg the name 
 *                 to search for
 *
 * @return  The timezone index in the table
 * 
 */
int16_t findTimezoneByName( char* name ) {

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


/*! ------------------------------------------------------------------------
 *
 * @brief   Gets a region starting index in the timezone table
 *
 * @param   region    Region ID
 *
 * @return  The starting index of the region block OR 0 if invalid region
 *          is specified.
 * 
 */
int16_t getTzRegionStartIndex( uint8_t region ) {
    switch( region ) {
        case TZ_REGION_AFRICA:          return TZ_REGION_AFRICA_INDEX;
        case TZ_REGION_ANTARCTICA:      return TZ_REGION_ANTARCTICA_INDEX;
        case TZ_REGION_ARCTIC_OCEAN:    return TZ_REGION_ARCTIC_OCEAN_INDEX;
        case TZ_REGION_ASIA:            return TZ_REGION_ASIA_INDEX;
        case TZ_REGION_ATLANTIC_OCEAN:  return TZ_REGION_ATLANTIC_OCEAN_INDEX;
        case TZ_REGION_AUSTRALIA:       return TZ_REGION_AUSTRALIA_INDEX;
        case TZ_REGION_CARIBBEAN:       return TZ_REGION_CARIBBEAN_INDEX;
        case TZ_REGION_CENTRAL_AMERICA: return TZ_REGION_CENTRAL_AMERICA_INDEX;
        case TZ_REGION_ETCETERA:        return TZ_REGION_ETCETERA_INDEX;
        case TZ_REGION_EUROPE:          return TZ_REGION_EUROPE_INDEX;
        case TZ_REGION_INDIAN_OCEAN:    return TZ_REGION_INDIAN_OCEAN_INDEX;
        case TZ_REGION_MIDDLE_EAST:     return TZ_REGION_MIDDLE_EAST_INDEX;
        case TZ_REGION_NORTH_AMERICA:   return TZ_REGION_NORTH_AMERICA_INDEX;
        case TZ_REGION_PACIFIC_OCEAN:   return TZ_REGION_PACIFIC_OCEAN_INDEX;
        case TZ_REGION_SOUTH_AMERICA:   return TZ_REGION_SOUTH_AMERICA_INDEX;
        default:                        return 0;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Gets a region block size in the timezone table
 *
 * @param   region    Region ID
 *
 * @return  The size of the region block OR 0 if invalid region
 *          is specified.
 * 
 */
uint16_t getTzRegionSize( uint8_t region ) {
    switch( region ) {
        case TZ_REGION_AFRICA:          return TZ_REGION_AFRICA_SIZE;
        case TZ_REGION_ANTARCTICA:      return TZ_REGION_ANTARCTICA_SIZE;
        case TZ_REGION_ARCTIC_OCEAN:    return TZ_REGION_ARCTIC_OCEAN_SIZE;
        case TZ_REGION_ASIA:            return TZ_REGION_ASIA_SIZE;
        case TZ_REGION_ATLANTIC_OCEAN:  return TZ_REGION_ATLANTIC_OCEAN_SIZE;
        case TZ_REGION_AUSTRALIA:       return TZ_REGION_AUSTRALIA_SIZE;
        case TZ_REGION_CARIBBEAN:       return TZ_REGION_CARIBBEAN_SIZE;
        case TZ_REGION_CENTRAL_AMERICA: return TZ_REGION_CENTRAL_AMERICA_SIZE;
        case TZ_REGION_ETCETERA:        return TZ_REGION_ETCETERA_SIZE;
        case TZ_REGION_EUROPE:          return TZ_REGION_EUROPE_SIZE;
        case TZ_REGION_INDIAN_OCEAN:    return TZ_REGION_INDIAN_OCEAN_SIZE;
        case TZ_REGION_MIDDLE_EAST:     return TZ_REGION_MIDDLE_EAST_SIZE;
        case TZ_REGION_NORTH_AMERICA:   return TZ_REGION_NORTH_AMERICA_SIZE;
        case TZ_REGION_PACIFIC_OCEAN:   return TZ_REGION_PACIFIC_OCEAN_SIZE;
        case TZ_REGION_SOUTH_AMERICA:   return TZ_REGION_SOUTH_AMERICA_SIZE;
        default:                        return 0;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Gets a region ending index in the timezone table
 *
 * @param   region    Region ID
 *
 * @return  The ending index of the region block OR 0 if invalid region
 *          is specified.
 * 
 */
int16_t getTzRegionEndIndex( uint8_t region ) {
    return getTzRegionStartIndex( region ) + getTzRegionSize( region ) - 1;
}