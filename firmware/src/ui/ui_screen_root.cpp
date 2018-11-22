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


/*--------------------------------------------------------------------------
 *
 * Event raised when a key press occurs
 *
 * Arguments
 * ---------
 *  - screen : Pointer to the screen where the event occured.
 *  - key    : Detected key press.
 *
 * Returns : TRUE to allow default key press processingor False to override.
 */
bool rootScreen_onKeypress( Screen* screen, uint8_t key ) {

    switch( key ) {
        case KEY_MENU:
            screen_main_menu.activate( true, &screen_root );
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

        case KEY_SWIPE | KEY_RIGHT:
            g_config.settings.lamp.mode = LAMP_MODE_ON;
            g_lamp.activate( &g_config.settings.lamp );
            break;

        case KEY_SWIPE | KEY_LEFT:
            g_lamp.deactivate();
            break;
    }

    return false;
}


/*--------------------------------------------------------------------------
 *
 * Event raised when updating the screen.
 *
 * Arguments
 * ---------
 *  - screen : Pointer to the screen where the event occured.
 *
 * Returns : TRUE to allow default screen updateor False to override.
 */
bool rootScreen_onDrawScreen( Screen* screen ) {

    char buffer[16];
    DateTime now = g_rtc.now();


    /* Print status icons */
    g_lcd.setPosition( 0, 12 );
    g_lcd.print( ( g_wifi.connected() == true ) ? CHAR_WIFI_ON : CHAR_SPACE );
    g_lcd.print( ( g_alarm.isSDCardPresent() == false ) ? CHAR_NO_SD : CHAR_SPACE );

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        g_lcd.print( ( g_alarm.isAlarmEnabled() == true ) ? CHAR_ALARM : CHAR_SPACE );

    } else {
        g_lcd.print( CHAR_SPACE );
    }

    g_lcd.print( ( g_power.getPowerMode() == POWER_MODE_NORMAL ) ? CHAR_SPACE : CHAR_BATTERY );

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        timeToBuf( buffer, g_config.settings.clock_24h, &now );

    } else {
        dateToBuf( buffer, g_config.settings.date_format, &now );
    }

    g_lcd.setPosition( 1, 0 );
    g_lcd.print( buffer, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );

    return false;
}