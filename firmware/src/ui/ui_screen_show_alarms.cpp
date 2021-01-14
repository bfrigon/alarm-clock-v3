//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_show_alarms.cpp
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
#include "ui.h"


/*! ------------------------------------------------------------------------
 *
 * @brief	Event raised when a key press occurs
 *
 * @param   screen    Pointer to the screen where the event occured.
 * @param   key       Detected key press.
 *
 * @return  TRUE to allow default key press processing, FALSE to override.
 * 
 */
bool showAlarmScreen_onKeypress( Screen *screen, uint8_t key ) {

    screen->exitScreen();
    return false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Event raised when updating the screen.
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 * @return  TRUE to allow default screen updat, FALSE to override.
 * 
 */
bool showAlarmScreen_onDrawScreen( Screen *screen ) {

    DateTime local;
    local = g_rtc.now();
    g_timezone.toLocal( &local );


    /* Get the next alarm id closest from now. */
    int8_t alarm_id = g_alarm.getNextAlarmID( &local, false );

    if( alarm_id == -1 ) {

        g_lcd.print_P( S_ALARMS_OFF );
        return false;
    }

    /* Get alarm next trigger time */
    int16_t alarm_time = g_alarm.getNextAlarmOffset( alarm_id, &local, false );

    g_lcd.printf_P( S_ALARM_IN, alarm_id + 1 );
    g_lcd.setPosition( 1, 0 );


    if( alarm_time > 1440 ) {
        g_lcd.printf_P( S_DATETIME_DHM, alarm_time / 1440, ( alarm_time % 1440 ) / 60, alarm_time % 60 );

    } else if( ( alarm_time == 1440 ) ) {
        g_lcd.print_P( S_DATETIME_24H );

    } else if( ( alarm_time % 1440 ) == 0 ) {
        g_lcd.printf_P( S_DATETIME_DD, alarm_time / 1440 );

    } else if( alarm_time > 60 ) {
        g_lcd.printf_P( S_DATETIME_HM, alarm_time / 60, alarm_time % 60 );

    } else if( alarm_time > 1 ) {
        g_lcd.printf_P( S_DATETIME_MM, alarm_time % 60 );

    } else {
        g_lcd.print_P( S_DATETIME_1MM );
    }

    return false;
}