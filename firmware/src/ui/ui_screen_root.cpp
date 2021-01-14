//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_root.cpp
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



uint8_t g_prevBattState = 0;


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
bool rootScreen_onKeypress( Screen* screen, uint8_t key ) {

    switch( key ) {
        case KEY_MENU:
            screen_main_menu.activate( true, &screen_root );
            break;

        case KEY_SET:
            if( g_lamp.isActive() == false ) {
                g_lamp.activate( &g_config.clock.lamp, false, true, LAMP_MODE_NIGHTLIGHT );
            } else {
                g_lamp.deactivate( true );
            }
            break;

        case KEY_SET | KEY_SHIFT:
            screen_set_time.activate( true, &screen_root );
            break;

        case KEY_ALARM | KEY_SHIFT:
            screen_set_alarms.activate( true, &screen_root );
            break;

        case KEY_ALARM:
            screen_show_alarms.activate( true, &screen_root );
            break;

        default:
            return true;
    }

    return false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Event raised when updating the screen.
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 * @return  TRUE to allow default screen updateor False to override.
 * 
 */
bool rootScreen_onDrawScreen( Screen* screen ) {

    char buffer[16];

    g_lcd.setPosition( 0, 0 );
    g_lcd.print( ( g_wifi.isConnected() == true ) ? CHAR_WIFI_ON : CHAR_SPACE );

    /* Print status icons */
    g_lcd.setPosition( 0, 13 );
    g_lcd.print( ( g_alarm.isSDCardPresent() == false ) ? CHAR_NO_SD : CHAR_SPACE );

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        g_lcd.print( ( g_alarm.isAlarmEnabled() == true ) ? CHAR_ALARM : CHAR_SPACE );

    } else {
        g_lcd.print( CHAR_SPACE );
    }

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

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        timeToBuf( buffer, g_config.clock.display_24h, &now );

    } else {
        dateToBuf( buffer, g_config.clock.date_format, &now );
    }

    g_lcd.setPosition( 1, 0 );
    g_lcd.print( buffer, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );

    return false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Event raised when the exit screen timeout delay has elapsed.
 *
 * @param   screen    Pointer to the screen where the event occured.
 */
void rootScreen_onTimeout( Screen *screen ) {

    screen->resetTimeout();

    uint8_t battState = g_battery.getBatteryState();

    if( battState != g_prevBattState )    {
        g_screenUpdate = true;
    }
}