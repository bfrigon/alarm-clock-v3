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

    this->print_P( S_CONSOLE_TIME_CURRENT_TZ );
    this->println_P( g_timezone.getName() );
    this->println();
    
    now = g_rtc.now();
    month = getMonthName( now.month(), true );
    dow = getDayName( now.dow(), true );

    this->printf_P( S_CONSOLE_DATE_FMT_UTC, dow, month, now.day(), now.hour(), now.minute(), now.second(), now.year() );

    
    g_timezone.toLocal( &now );

    abbvr = g_timezone.getAbbreviation( &now );
    month = getMonthName( now.month(), true );
    dow = getDayName( now.dow(), true );

    this->printf_P( S_CONSOLE_DATE_FMT_LOCAL, dow, month, now.day(), now.hour(), now.minute(), now.second(), abbvr, now.year() );
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
        this->println_P( g_timezone.getName() );
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

        
        id = g_timezone.findTimezoneByName( _inputbuffer );

    /* Timezone provided as a parameter */
    } else {
        id = g_timezone.findTimezoneByName( param_tz_name );
    }

    if( id < 0 )  {
        this->println_P( S_CONSOLE_TIME_INVALID_TZ );

        this->endTask( TASK_FAIL );
        return;
    }

    /* Set new time zone and save config */
    g_timezone.setTimezoneByID( id );
    
    g_config.save( EEPROM_SECTION_CLOCK );

    this->print_P( S_CONSOLE_TIME_NEW_TZ );
    this->println_P( g_timezone.getName() );

    /* Refresh clock display */
    g_clock.restoreClockDisplay();

    this->endTask( TASK_SUCCESS );
}



void Console::showTimezoneInfo() {
    this->printDateTime();
    this->println();

    DateTime now = g_rtc.now();
    g_timezone.toLocal( &now );

    bool isDst = g_timezone.isDST( &now );
    
    int16_t std_offset, dst_offset, current_offset;
    std_offset = g_timezone.getStdUtcOffset();
    dst_offset = g_timezone.getDstUtcOffset();
    current_offset = ( isDst == true ) ? dst_offset : std_offset;

    DateTime std, dst;
    g_timezone.getStdTransition( g_rtc.now()->year(), &std );
    g_timezone.getDstTransition( g_rtc.now()->year(), &dst );



    /* Print whether the local time is equal, ahead or behind UTC */
    if( current_offset == 0 ) {
        this->print_P( S_CONSOLE_TZ_EQUAL_UTC );
    } else if( current_offset > 0 ) {
        this->print_P( S_CONSOLE_TZ_AHEAD_UTC_PRE );
        this->printTimeInterval( abs( current_offset * 60L ), S_DATETIME_SEPARATOR_AND );
        this->print_P( S_CONSOLE_TZ_AHEAD_UTC );
    } else {
        this->print_P( S_CONSOLE_TZ_BEHIND_UTC_PRE );
        this->printTimeInterval( abs( current_offset * 60L ), S_DATETIME_SEPARATOR_AND );
        this->print_P( S_CONSOLE_TZ_BEHIND_UTC );
    }

    
    if( dst == std ) {

        /* DST not observed in this time zone */
        this->print_P( S_CONSOLE_TZ_NO_DST );

    } else {

        this->printf_P( S_CONSOLE_TZ_IS_DST, (isDst == true ) ? S_YES : S_NO );


        bool show_dst_first = ( std > dst );

        for( uint8_t i = 0; i < 2; i++ ) {

            if( show_dst_first == true ) {
                this->println();
                this->printf_P( S_CONSOLE_TZ_DST_TRANS, 
                                getDayName( dst.dow(), false ),
                                getMonthName( dst.month(), false ),
                                dst.day(),
                                dst.year(),
                                dst.hour(),
                                dst.minute() );

                this->printTimeInterval( abs( dst_offset - std_offset ) * 60, S_DATETIME_SEPARATOR_AND );
                this->println();
                
                show_dst_first = false;
                continue;
            }

            if( show_dst_first == false ) {

                this->println();
                this->printf_P( S_CONSOLE_TZ_STD_TRANS, 
                                getDayName( std.dow(), false ),
                                getMonthName( std.month(), false ),
                                std.day(),
                                std.year(),
                                std.hour(),
                                std.minute() );

                this->printTimeInterval( abs( std_offset - dst_offset ) * 60, S_DATETIME_SEPARATOR_AND );
                this->println();
                
                show_dst_first = true;
                continue;
            }
        }

        
    }
}