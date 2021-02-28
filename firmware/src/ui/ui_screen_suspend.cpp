//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_suspend.cpp
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
 * @brief   Event raised when a key press occurs
 *
 * @param   screen    Pointer to the screen where the event occured.
 * @param   key       Detected key press.
 *
 * @return  TRUE to allow default key press processing, FALSE to override.
 * 
 */
bool suspendScreen_onKeypress( Screen* screen, uint8_t key ) {

    g_power.setPowerMode( POWER_MODE_LOW_POWER );
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
bool suspendScreen_onDrawScreen( Screen* screen ) {

    /* Print status icons */
    g_lcd.setPosition( 0, 14 );
    g_lcd.print( ( g_alarm.isAlarmEnabled() == true ) ? CHAR_ALARM : CHAR_SPACE );

    switch( g_battery.getBatteryState() ) {
        case BATTERY_STATE_NOT_PRESENT:
            g_lcd.print( CHAR_NO_BATTERY );
            break;

        case BATTERY_STATE_CHARGING:
            g_lcd.print( CHAR_BATTERY_CHARGING );
            break;

        case BATTERY_STATE_DISCHARGE_FULL:
            g_lcd.print( CHAR_BATTERY_FULL );
            break;

        case BATTERY_STATE_DISCHARGE_HALF:
            g_lcd.print( CHAR_BATTERY_HALF );
            break;
        
        case BATTERY_STATE_DISCHARGE_LOW:
            g_lcd.print( CHAR_BATTERY_LOW );
            break;

        default:
            g_lcd.print( CHAR_SPACE );
    }


    DateTime now = g_rtc.now();
    g_timezone.toLocal( &now );


    char buffer[16];
 
    g_lcd.setPosition( 1, 0 );

    timeToBuf( buffer, g_config.clock.display_24h, &now );
    g_lcd.print( buffer, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
    

    return false;
}