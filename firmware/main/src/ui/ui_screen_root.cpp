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


bool rootScreen_onKeypress( Screen *screen, uint8_t key ) {

    switch( key ) {
        case KEY_MENU:
            gotoScreen( &screen_main_menu, true, &screen_root );
            break;

        case KEY_SET | KEY_SHIFT:
            gotoScreen( &screen_set_time, true, &screen_root );
            break;

        case KEY_ALARM | KEY_SHIFT:
            gotoScreen( &screen_set_alarms, true, &screen_root );
            break;

        case KEY_ALARM:
            gotoScreen( &screen_show_alarms, true, &screen_root );
            break;

        case KEY_SWIPE | KEY_RIGHT:
            g_config.lamp.mode = LAMP_MODE_ON;
            g_lamp.activate( &g_config.lamp );
            break;

        case KEY_SWIPE | KEY_LEFT:
            g_lamp.deactivate();
            break;

        case KEY_SET:
            /* temporary */
            g_alarm.loadProfile( 0 );
            g_alarm.play( ALARM_MODE_NORMAL );
            break;
    }

    return false;
}

bool rootScreen_onDrawScreen( Screen *screen ) {

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
        timeToBuf( buffer, g_config.clock_24h, &now );

    } else {
        dateToBuf( buffer, g_config.date_format, &now );
    }

    g_lcd.setPosition( 1, 0 );
    g_lcd.print( buffer, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );

    return false;
}