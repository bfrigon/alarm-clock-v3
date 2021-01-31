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
#include "../console_base.h"
#include "../../task_errors.h"
#include "../../libs/time.h"
#include "../../libs/timezone.h"
#include "../../libs/tzdata.h"
#include "../../drivers/neoclock.h"
#include "../../services/ntpclient.h"
#include "../../ui/ui.h"
#include "../../config.h"


static char* param_tz_name;
static DateTime cmd_time_adj;
bool cmd_time_use_ntp;
uint8_t selected_region;

/* Set timezone command task steps */
enum {
    STEP_PRINT_REGION_LIST = 0,
    STEP_PROMPT_SELECT_REGION = MAX_TZ_REGION_NAMES,
    STEP_VALIDATE_REGION,
    STEP_PRINT_TZ_LIST,
    STEP_PROMPT_SELECT_TZ  = STEP_PRINT_TZ_LIST + MAX_TIMEZONE_ID,
    STEP_VALIDATE_TZ_INDEX,
    STEP_VALIDATE_TZ_NAME,
    STEP_SET_TZ,
};

/* Set date command task steps */
enum {
    STEP_DISPLAY_SYNC_NTP_PROMPT,
    STEP_VALIDATE_SYNC_NTP_PROMPT,
    STEP_DISPLAY_DATE_PROMPT,
    STEP_VALIDATE_DATE_PROMPT,
    STEP_DISPLAY_TIME_PROMPT,
    STEP_VALIDATE_TIME_PROMPT,
    STEP_DISPLAY_APPLY_SETTINGS_PROMPT,
    STEP_VALIDATE_APPLY_SETTINGS_PROMPT,
    STEP_MONITOR_NTP_SYNC_COMPLETION,
    

};



/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'set date' task
 *
 * @return  TRUE if successful, FALSE otherwise
 *           
 */
bool ConsoleBase::openCommandSetDate() {
    _taskIndex = STEP_DISPLAY_SYNC_NTP_PROMPT;

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
void ConsoleBase::runCommandSetDate() {
    

    /* Even index display the prompt, odd index process user input */
    if( _taskIndex % 2 ) {

        if( this->processInput() == false ) {
            return;
        }
    }
    

    switch( _taskIndex++ ) {

        /* Display 'synchronize with ntp' prompt */
        case STEP_DISPLAY_SYNC_NTP_PROMPT:
            _inputBufferLimit = 1;

            this->println();
            this->printf_P( S_CONSOLE_TIME_CFG_NTP, ( g_config.clock.use_ntp == true ) ? "Y" : "N" );
            break;

        /* Validate 'synchronize with ntp' response */
        case STEP_VALIDATE_SYNC_NTP_PROMPT:
            if( tolower( _inputBuffer[ 0 ] ) == 'y' ) {

                cmd_time_use_ntp = true;
                _taskIndex = STEP_DISPLAY_APPLY_SETTINGS_PROMPT;

            } else if ( tolower( _inputBuffer[ 0 ] ) == 'n' ) {

                cmd_time_use_ntp = false;

            } else if( strlen( _inputBuffer ) == 0 ) {

                /* If using ntp, skip manual time set */
                if( cmd_time_use_ntp == true ) {
                    _taskIndex = STEP_DISPLAY_APPLY_SETTINGS_PROMPT;
                }

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );

                /* try again */
                _taskIndex = STEP_DISPLAY_SYNC_NTP_PROMPT;
            }
            break;

        /* Display 'enter date' prompt */
        case STEP_DISPLAY_DATE_PROMPT:
            _inputBufferLimit = INPUT_BUFFER_LENGTH;

            this->printf_P( S_CONSOLE_TIME_CFG_DATE, cmd_time_adj.year(), cmd_time_adj.month(), cmd_time_adj.day() );
            break;

        /* Validate 'enter date' response */
        case STEP_VALIDATE_DATE_PROMPT:
            
            if( strlen( _inputBuffer ) > 0 && ( strptime( _inputBuffer, "%Y-%m-%d", &cmd_time_adj ) == NULL )) {
                this->println_P( S_CONSOLE_INVALID_DATE_FMT );
                this->println();

                /* try again */
                _taskIndex = STEP_DISPLAY_DATE_PROMPT;
            }
            break;

        /* Display 'enter time' prompt */
        case STEP_DISPLAY_TIME_PROMPT:
            _inputBufferLimit = INPUT_BUFFER_LENGTH;

            this->printf_P( S_CONSOLE_TIME_CFG_TIME, cmd_time_adj.hour(), cmd_time_adj.minute() );
            break;

        /* Validate 'enter time' response */
        case STEP_VALIDATE_TIME_PROMPT:
            if( strlen( _inputBuffer ) > 0 && ( strptime( _inputBuffer, "%H:%M", &cmd_time_adj ) == NULL )) {
                this->println_P( S_CONSOLE_INVALID_TIME_FMT );
                this->println();

                /* try again */
                _taskIndex = STEP_DISPLAY_TIME_PROMPT;
            }

            break;

        /* Display 'apply settings' prompt */
        case STEP_DISPLAY_APPLY_SETTINGS_PROMPT:
            _inputBufferLimit = 1;

            this->println();
            this->print_P( S_CONSOLE_TIME_CFG_APPLY );

            break;

        /* Validate 'apply settings' response */
        case STEP_VALIDATE_APPLY_SETTINGS_PROMPT:
            if( tolower( _inputBuffer[ 0 ] ) == 'y' ) {

                g_config.clock.use_ntp = cmd_time_use_ntp;

                /* Save configuration */
                g_config.save( EEPROM_SECTION_CLOCK );

                if( g_config.clock.use_ntp == true ) {

                    /* Enable auto sync and synchronize now */
                    g_ntp.setAutoSync( true, this );

                    _taskIndex = STEP_MONITOR_NTP_SYNC_COMPLETION;
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
                    g_clock.requestClockUpdate();
                    g_screen.requestScreenUpdate( false );
                }

                this->endTask( TASK_SUCCESS );
                return;

            } else if ( tolower( _inputBuffer[ 0 ] ) == 'n' ) {

                this->endTask( TASK_SUCCESS );
                return;

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );
                

                /* try again */
                _taskIndex = STEP_DISPLAY_APPLY_SETTINGS_PROMPT;
            }
            break;

        /* Monitor NTP sync completion */
        case STEP_MONITOR_NTP_SYNC_COMPLETION:

            if( g_ntp.isBusy() == false ) {

                this->endTask( g_ntp.getTaskError() );
                return;
            }

            _taskIndex = STEP_MONITOR_NTP_SYNC_COMPLETION;
    }

    this->resetInput();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Print the current date and time to the console
 *           
 */
void ConsoleBase::runCommandPrintDateTime() {

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
    this->println();
    
    g_timezone.toLocal( &now );

    abbvr = g_timezone.getAbbreviation( &now );
    month = getMonthName( now.month(), true );
    dow = getDayName( now.dow(), true );

    this->printDateTime( &now, abbvr, ms );
    this->println();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'set timezone' task
 *
 * @return  TRUE if successful, FALSE otherwise
 *           
 */
bool ConsoleBase::openCommandSetTimeZone() {
    
    param_tz_name = this->getInputParameter();

    if( param_tz_name == 0 ) {

        this->print_P( S_CONSOLE_TIME_CURRENT_TZ );
        this->println_P( g_timezone.getName() );
        this->println();
    }

    _taskIndex = STEP_PRINT_REGION_LIST;
    selected_region = 0;

    this->startTask( TASK_CONSOLE_SET_TZ );
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run the 'set timezone' task. Display prompts and validate 
 *          responses required before executing the task.
 *           
 */
void ConsoleBase::runCommandSetTimeZone() {
    int16_t zone_id;
    
    if( param_tz_name > 0 ) {
        
        zone_id = findTimezoneByName( param_tz_name );

        if( zone_id < 0 ) {
            this->endTask( ERR_CONSOLE_INVALID_TIMEZONE );
            return;
        }

        _taskIndex = STEP_SET_TZ;
    }


    /* Print time zone region list */
    if( _taskIndex < STEP_PROMPT_SELECT_REGION ) {
        this->printf_P( PSTR( "%2d) %-20S " ), _taskIndex + 1, _TZ_REGION_NAMES[ _taskIndex ]);

        if( _taskIndex % 2 || _taskIndex == STEP_PROMPT_SELECT_REGION - 1 ) {
            this->println();
        }

        if( _taskIndex == STEP_PROMPT_SELECT_REGION - 1 ) {
            this->println();

            this->println_P( S_CONSOLE_TIME_EXIT_MENU );
            this->println();
        }

        _taskIndex++;
        return;
    }


    /* Print select region prompt */
    if( _taskIndex == STEP_PROMPT_SELECT_REGION ) {
        this->print_P( S_CONSOLE_TIME_ENTER_TZ_NAME );

        _taskIndex++;
        return;
    }


    /* Validate select region prompt */
    if( _taskIndex == STEP_VALIDATE_REGION ) {

        if( this->processInput() == false ) {
            return;
        }

        if( atoi( _inputBuffer ) > 0 ) {
            selected_region = atoi( _inputBuffer ) - 1;

            if( selected_region >= MAX_TZ_REGION_NAMES ) {
                this->println_P( S_CONSOLE_TIME_INVALID_SEL );
                this->println();

                _taskIndex = STEP_PROMPT_SELECT_REGION;

                this->resetInput();
                return;
            }

            this->println();
            _taskIndex = STEP_PRINT_TZ_LIST + getTzRegionStartIndex( selected_region );

            this->resetInput();
            return;
            
        } else {

            /* If empty, keep existing time zone */
            if( strlen( _inputBuffer ) == 0 ) {

                this->endTask( TASK_SUCCESS );
                return;
            }

            zone_id = findTimezoneByName( _inputBuffer );

            if( zone_id < 0 )  {

                this->println_P( S_CONSOLE_TIME_INVALID_TZ );
                this->println();

                _taskIndex = STEP_PROMPT_SELECT_REGION;

                this->resetInput();
                return;
            }

            this->resetInput();

            _taskIndex = STEP_SET_TZ;
        }
    }


    /* Print time zones list within selected region */
    if( _taskIndex <= getTzRegionEndIndex( selected_region ) + STEP_PRINT_TZ_LIST ) {

        TimeZone tz;
        tz.setTimezoneByID( _taskIndex - STEP_PRINT_TZ_LIST );
        
        this->printf_P( PSTR( "%2d) %-36S " ), 
                        _taskIndex - STEP_PRINT_TZ_LIST - getTzRegionStartIndex( selected_region ) + 1, 
                        tz.getName() );

        if(( _taskIndex - STEP_PRINT_TZ_LIST ) % 2 || _taskIndex == getTzRegionEndIndex( selected_region ) + STEP_PRINT_TZ_LIST ) {
            this->println();
        }

        if( _taskIndex == getTzRegionEndIndex( selected_region ) + STEP_PRINT_TZ_LIST ) {

            this->println();
            this->println_P( S_CONSOLE_TIME_INSTR_RETURN );
            this->println();

            _taskIndex = STEP_PROMPT_SELECT_TZ;

        } else {
            _taskIndex++;
        }
        
        return;
    }


    /* Print select timezone prompt */
    if( _taskIndex == STEP_PROMPT_SELECT_TZ ) {
        this->print_P( S_CONSOLE_TIME_SELECT_TZ );

        _taskIndex++;
        return;
    }


    /* Validate select timezone prompt */
    if( _taskIndex == STEP_VALIDATE_TZ_INDEX ) {

        if( this->processInput() == false ) {
            return;
        }
        
        if( strlen( _inputBuffer ) == 0 || strcasecmp_P( _inputBuffer, PSTR( "0")) == 0 ) { 
            _taskIndex = STEP_PRINT_REGION_LIST;

            this->resetInput();
            return;
        }

        zone_id = ( atoi( _inputBuffer ) - 1 ) + getTzRegionStartIndex( selected_region );
        Serial.println( zone_id );

        /* Checks if selected zone is within region boundaries */
        if( zone_id < getTzRegionStartIndex( selected_region ) || 
            zone_id > getTzRegionEndIndex( selected_region )) {

            this->println_P( S_CONSOLE_TIME_INVALID_SEL );
            this->println();

            _taskIndex = STEP_PROMPT_SELECT_TZ;

            this->resetInput();
            return;
        }
        
        _taskIndex = STEP_SET_TZ;
    }


    /* Set new timezone */
    if( _taskIndex == STEP_SET_TZ ) {

        g_timezone.setTimezoneByID( zone_id );
        strcpy_P( g_config.clock.timezone, g_timezone.getName() );
        
        g_config.save( EEPROM_SECTION_CLOCK );

        this->println();
        this->print_P( S_CONSOLE_TIME_NEW_TZ );
        this->println_P( g_timezone.getName() );

        /* Request clock display update */
        g_clock.requestClockUpdate();
    }

    this->endTask( TASK_SUCCESS );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Print current timezone information to the console.
 *           
 */
void ConsoleBase::showTimezoneInfo() {
    
    this->runCommandPrintDateTime();
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
        this->println_P( S_CONSOLE_TZ_EQUAL_UTC );
    } else if( current_offset > 0 ) {
        this->print_P( S_CONSOLE_TZ_AHEAD_UTC_PRE );
        this->printTimeInterval( abs( current_offset ) * 60UL , S_DATETIME_SEPARATOR_AND );
        this->println_P( S_CONSOLE_TZ_AHEAD_UTC );
    } else {
        this->print_P( S_CONSOLE_TZ_BEHIND_UTC_PRE );
        this->printTimeInterval( abs( current_offset ) * 60UL, S_DATETIME_SEPARATOR_AND );
        this->println_P( S_CONSOLE_TZ_BEHIND_UTC );
    }

    
    if( dst == std ) {

        /* DST not observed in this time zone */
        this->println_P( S_CONSOLE_TZ_NO_DST );

    } else {

        this->printfln_P( S_CONSOLE_TZ_IS_DST, (isDst == true ) ? S_YES : S_NO );


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