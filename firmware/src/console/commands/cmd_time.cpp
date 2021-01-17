//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/commands/cmd_time.cpp
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
#include "../../task_errors.h"
#include "../../libs/time.h"
#include "../../libs/timezone.h"
#include "../../drivers/neoclock.h"
#include "../../services/ntpclient.h"
#include "../../ui/ui.h"
#include "../../config.h"


static char* param_tz_name;
static DateTime cmd_time_adj;
bool cmd_time_use_ntp;

/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'set date' task
 *
 * @return  TRUE if successful, FALSE otherwise
 *           
 */
bool Console::startTaskSetDate() {
    _taskIndex = 0;

    cmd_time_adj = g_rtc.now();
    g_timezone.toLocal( &cmd_time_adj );

    this->println();
    this->println_P( S_CONSOLE_TIME_SET_INSTR );

    this->startTask( TASK_CONSOLE_SET_DATE );
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run the 'set date' task. Display prompts and validate 
 *          responses required before executing the task.
 *           
 */
void Console::runTaskSetDate() {
    

    /* Even index display the prompt, odd index process user input */
    if( _taskIndex % 2 ) {

        if( this->processInput() == false ) {
            return;
        }

        this->trimInput();
    }
    

    switch( _taskIndex++ ) {

        /* Display 'synchronize with ntp' prompt */
        case 0:
            _inputBufferLimit = 1;

            this->println();
            this->printf_P( S_CONSOLE_TIME_CFG_NTP, ( g_config.clock.use_ntp == true ) ? "Y" : "N" );
            break;

        /* Validate 'synchronize with ntp' response */
        case 1:
            if( tolower( _inputBuffer[ 0 ] ) == 'y' ) {

                cmd_time_use_ntp = true;
                _taskIndex = 6;

            } else if ( tolower( _inputBuffer[ 0 ] ) == 'n' ) {

                cmd_time_use_ntp = false;

            } else if( strlen( _inputBuffer ) == 0 ) {

                /* If using ntp, skip manual time set */
                if( cmd_time_use_ntp == true ) {
                    _taskIndex = 6;
                }

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );

                /* try again */
                _taskIndex = 0;
            }
            break;

        /* Display 'enter date' prompt */
        case 2:
            _inputBufferLimit = INPUT_BUFFER_LENGTH;

            this->printf_P( S_CONSOLE_TIME_CFG_DATE, cmd_time_adj.year(), cmd_time_adj.month(), cmd_time_adj.day() );
            break;

        /* Validate 'enter date' response */
        case 3:
            
            if( strlen( _inputBuffer ) > 0 && ( strptime( _inputBuffer, "%Y-%m-%d", &cmd_time_adj ) == NULL )) {
                this->println_P( S_CONSOLE_INVALID_DATE_FMT );
                this->println();

                /* try again */
                _taskIndex = 2;
            }
            break;

        /* Display 'enter time' prompt */
        case 4:
            _inputBufferLimit = INPUT_BUFFER_LENGTH;

            this->printf_P( S_CONSOLE_TIME_CFG_TIME, cmd_time_adj.hour(), cmd_time_adj.minute() );
            break;

        /* Validate 'enter time' response */
        case 5:
            if( strlen( _inputBuffer ) > 0 && ( strptime( _inputBuffer, "%H:%M", &cmd_time_adj ) == NULL )) {
                this->println_P( S_CONSOLE_INVALID_TIME_FMT );
                this->println();

                /* try again */
                _taskIndex = 4;
            }

            break;

        /* Display 'apply settings' prompt */
        case 6:
            _inputBufferLimit = 1;

            this->println();
            this->print_P( S_CONSOLE_TIME_CFG_APPLY );

            break;

        /* Validate 'apply settings' response */
        case 7:
            if( tolower( _inputBuffer[ 0 ] ) == 'y' ) {

                g_config.clock.use_ntp = cmd_time_use_ntp;

                /* Save configuration */
                g_config.save( EEPROM_SECTION_CLOCK );
                g_screenUpdate = true;

                if( g_config.clock.use_ntp == true ) {

                    /* Enable auto sync and synchronize now */
                    g_ntp.setAutoSync( true, true );

                    _taskIndex = 8;
                    break;

                } else {

                    /* Disable auto sync */
                    g_ntp.setAutoSync( false );

                    /* Convert local time to UTC */
                    g_timezone.toUTC( &cmd_time_adj );

                    /* Update the RTC */
                    g_rtc.writeTime( &cmd_time_adj );

                    /* Update the time on the wifi module */
                    g_wifi.setSystemTime( &cmd_time_adj );

                    /* Request clock display update */
                    g_clock.requestDisplayUpdate();
                }

                this->endTask( TASK_SUCCESS );
                return;

            } else if ( tolower( _inputBuffer[ 0 ] ) == 'n' ) {

                this->endTask( TASK_SUCCESS );
                return;

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );
                

                /* try again */
                _taskIndex = 6;
            }
            break;

        /* Monitor NTP sync completion */
        case 8:

            if( g_ntp.isBusy() == false ) {

                this->endTask( g_ntp.getTaskError() );
                return;
            }

            _taskIndex = 8;
    }

    this->resetInput();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Print the current date and time to the console
 *           
 */
void Console::runTaskPrintDateTime() {

    DateTime now;
    const char *month;
    const char *dow;
    const char *abbvr;

    this->print_P( S_CONSOLE_TIME_CURRENT_TZ );
    this->println_P( g_timezone.getName() );
    this->println();
    
    now = g_rtc.now();
    uint16_t ms = g_rtc.getMillis();

    month = getMonthName( now.month(), true );
    dow = getDayName( now.dow(), true );

    this->printDateTime( &now, TZ_UTC, ms );
    g_console.println();
    
    g_timezone.toLocal( &now );

    abbvr = g_timezone.getAbbreviation( &now );
    month = getMonthName( now.month(), true );
    dow = getDayName( now.dow(), true );

    this->printDateTime( &now, abbvr, ms );
    g_console.println();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'set timezone' task
 *
 * @return  TRUE if successful, FALSE otherwise
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Run the 'set timezone' task. Display prompts and validate 
 *          responses required before executing the task.
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
        if( strlen( _inputBuffer ) == 0 ) {
            this->endTask( TASK_SUCCESS );
            return;
        }

        
        id = g_timezone.findTimezoneByName( _inputBuffer );

    /* Timezone provided as a parameter */
    } else {
        id = g_timezone.findTimezoneByName( param_tz_name );
    }

    if( id < 0 )  {

        this->endTask( ERR_CONSOLE_INVALID_TIMEZONE );
        return;
    }

    /* Set new time zone and save config */
    g_timezone.setTimezoneByID( id );
    
    g_config.save( EEPROM_SECTION_CLOCK );

    this->print_P( S_CONSOLE_TIME_NEW_TZ );
    this->println_P( g_timezone.getName() );

    /* Request clock display update */
    g_clock.requestDisplayUpdate();

    this->endTask( TASK_SUCCESS );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Print current timezone information to the console.
 *           
 */
void Console::showTimezoneInfo() {
    
    this->runTaskPrintDateTime();
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
        this->printTimeInterval( abs( current_offset ) * 60UL , S_DATETIME_SEPARATOR_AND );
        this->print_P( S_CONSOLE_TZ_AHEAD_UTC );
    } else {
        this->print_P( S_CONSOLE_TZ_BEHIND_UTC_PRE );
        this->printTimeInterval( abs( current_offset ) * 60UL, S_DATETIME_SEPARATOR_AND );
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