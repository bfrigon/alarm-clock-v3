//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/commands/net.cpp
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
#include "../console.h"
#include "../../libs/time.h"
#include "../../libs/timezone.h"
#include "../../drivers/neoclock.h"
#include "../../ui/ui.h"
#include "../../config.h"


static char* param_tz_name;

bool Console::startTaskDateSet() {

}

void Console::runTaskDateSet() {

}


/*--------------------------------------------------------------------------
 *
 * Print the date and time to the console
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 *           
 */
void Console::printDateTime() {

    DateTime now;
    const char *month;
    const char *dow;
    const char *abbvr;
    
    now = g_rtc.now();
    month = getMonthName( now.month(), true );
    dow = getDayName( now.dow(), true );

    this->printf_P( S_CONSOLE_DATE_FMT_UTC, dow, month, now.day(), now.hour(), now.minute(), now.second(), now.year() );

    
    g_timezone.toLocal( &now );

    abbvr = g_timezone.getCurrentZoneAbbreviation( &now );
    month = getMonthName( now.month(), true );
    dow = getDayName( now.dow(), true );

    this->printf_P( S_CONSOLE_DATE_FMT_LOCAL, dow, month, now.day(), now.hour(), now.minute(), now.second(), abbvr, now.year() );

    this->println();
    this->print_P( S_CONSOLE_TIME_CURRENT_TZ );
    this->println_P( g_timezone.getCurrentZoneName() );
    

}


/*--------------------------------------------------------------------------
 *
 * Starts a set timezone task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful, FALSE otherwise
 *           
 */
bool Console::startTaskSetTimeZone() {

    
    param_tz_name = this->getInputParameter();

    if( param_tz_name == 0 ) {

        this->print_P( S_CONSOLE_TIME_CURRENT_TZ );
        this->println_P( g_timezone.getCurrentZoneName() );
        this->println();

        this->print_P( S_CONSOLE_TIME_ENTER_TZ );
    }

    this->startTask( TASK_CONSOLE_SET_TZ );
    return true;
}


/*--------------------------------------------------------------------------
 *
 * Run the set timezone task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 *           
 */
void Console::runTaskSetTimeZone() {
    int16_t id;

    
    if( param_tz_name == 0 ) {
        if( this->processInput() == false ) {
            return;
        }

        this->trimInput();

        /* If empty, keep existing time zone */
        if( _inputlength == 0 ) {
            this->endTask( TASK_SUCCESS );
            return;
        }

        
        id = g_timezone.findTimeZoneByName( _inputbuffer );

    /* Timezone provided as a parameter */
    } else {
        id = g_timezone.findTimeZoneByName( param_tz_name );
    }

    if( id < 0 )  {
        this->println_P( S_CONSOLE_TIME_INVALID_TZ );

        this->endTask( TASK_FAIL );
        return;
    }

    /* Set new time zone and save config */
    g_timezone.setTimeZone( id );
    g_config.clock.tz = id;
    g_config.save( EEPROM_SECTION_CLOCK );

    this->print_P( S_CONSOLE_TIME_NEW_TZ );
    this->println_P( g_timezone.getCurrentZoneName() );

    /* Refresh clock display */
    g_clock.restoreClockDisplay();
    g_screenUpdate = true;

    this->endTask( TASK_SUCCESS );
}