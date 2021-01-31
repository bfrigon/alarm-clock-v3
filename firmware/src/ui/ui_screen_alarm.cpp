//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_alarm.cpp
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

bool showHelpMsg = false;



/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when entering the screen
 *
 * @param   screen    Pointer to the screen where the event occured.
 * 
 */
void alarmScreen_onEnterScreen( Screen *screen, uint8_t prevScreenID ) {

    screen->setTimeout( 3000 );

    showHelpMsg = false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when a key press occurs
 *
 * @param   screen    Pointer to the screen where the event occured.
 * @param   key       Detected key press.
 *
 * @return  TRUE to allow default key press processing, FALSE to override.
 * 
 */
bool alarmScreen_onKeypress( Screen *screen, uint8_t key ) {
    uint8_t mode = g_alarm.getPlayMode();

    if( mode & ALARM_MODE_TEST ) {
        g_alarm.stop();
        return false;
    }

    if( g_alarm.isSnoozing() ) {

        if( key == ( KEY_ALARM | KEY_SHIFT ) ) {
            g_alarm.stop();
            return false;

        } else {
            showHelpMsg = true;

            screen->resetTimeout();
            screen->setTimeout( 3000 );

            g_screen.requestScreenUpdate( true );
        }

    } else {

        if( g_alarm.profile.snoozeDelay != 0 ) {
            g_alarm.snooze();

        } else {

            g_alarm.stop();
        }
    }

    return false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when updating the screen.
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 * @return  TRUE to allow default screen updateor False to override.
 * 
 */
bool alarmScreen_onDrawScreen( Screen *screen ) {
    uint8_t mode = g_alarm.getPlayMode();



    if( g_alarm.isSnoozing() && ( ( mode & ALARM_MODE_TEST ) == 0 ) ) {
        if( showHelpMsg == true ) {

            g_lcd.print_P( S_INSTR_CANCEL_ALARM_1 );
            g_lcd.setPosition( 1, 0 );
            g_lcd.print_P( S_INSTR_CANCEL_ALARM_2 );

            return true;
        }

        g_lcd.setPosition( 0, 0 );
        g_lcd.print_P( S_SNOOZE, 16, TEXT_ALIGN_CENTER );

        uint16_t remaining;
        remaining = g_alarm.getSnoozeTimeRemaining();

        char buffer[ DISPLAY_WIDTH ];

        if( remaining <= 30 ) {
            sprintf_P( buffer, S_DATETIME_S, 30 );

        } else if( remaining <= 60 ) {
            strcpy_P( buffer, S_DATETIME_1M );

        } else if( remaining % 60 == 0 ) {
            sprintf_P( buffer, S_DATETIME_M, ( remaining / 60 ) );

        } else {
            sprintf_P( buffer, S_DATETIME_M, ( remaining / 60 ) + 1 );
        }

        g_lcd.setPosition( 1, 0 );
        g_lcd.print( buffer, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );

        return true;
    }


    g_lcd.setPosition( 1, 0 );

    if( strlen( g_alarm.profile.message ) > 0 ) {
        g_lcd.print( g_alarm.profile.message, 16, TEXT_ALIGN_CENTER );

    } else {

        if( g_alarm.profile.time.hour < 12 ) {
            g_lcd.print_P( S_ALARM_MSG_MORNING, 16, TEXT_ALIGN_CENTER );

        } else if( g_alarm.profile.time.hour < 18 ) {
            g_lcd.print_P( S_ALARM_MSG_AFTERNOON, 16, TEXT_ALIGN_CENTER );

        } else {
            g_lcd.print_P( S_ALARM_MSG_EVENING, 16, TEXT_ALIGN_CENTER );
        }
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when the exit screen timeout delay has elapsed.
 *
 * @param   screen    Pointer to the screen where the event occured.
 */
void alarmScreen_onTimeout( Screen *screen ) {
    if( showHelpMsg == true ) {
        showHelpMsg = false;

        screen->resetTimeout();
        screen->setTimeout( 1000 );

        g_screen.requestScreenUpdate( true );
    } else {
        g_screen.requestScreenUpdate( false );
    }
}