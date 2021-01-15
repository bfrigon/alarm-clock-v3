//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_batt_status.cpp
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
#include "../config.h"
#include "../drivers/bq27441.h"


uint8_t g_battStatusPage = 0;


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when updating the screen.
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 * @return  TRUE to allow default screen update, FALSE to override.
 * 
 */
bool battStatus_onDrawScreen( Screen* screen ) {

    bool bat_det = g_battery.isBatteryPresent();


    /* Print battery state icon */
    g_lcd.setPosition( 0, DISPLAY_WIDTH - 1 );

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


    char buffer[ DISPLAY_WIDTH + 1 ];

    if( g_battStatusPage == 0 ) {
        uint8_t soc = g_battery.getStateOfCharge( false );
        uint16_t voltage = g_battery.getVoltage();
        uint16_t remain_cap = g_battery.getRemainingCapacity( false );

        if( bat_det == true ) {

            g_lcd.setPosition( 0, 0 );
            snprintf_P( buffer, sizeof( buffer ), S_BATT_VOLTAGE, voltage / 1000, voltage % 1000 );
            g_lcd.print( buffer, 8, TEXT_ALIGN_LEFT );

            g_lcd.setPosition( 1, 0 );
            snprintf_P( buffer, sizeof( buffer ), S_BATT_CAPACITY, remain_cap, soc );
            g_lcd.print( buffer, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );

        } else {
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_BATT_NO_VOLTAGE, 8, TEXT_ALIGN_LEFT );

            g_lcd.setPosition( 1, 0 );
            g_lcd.print_P( S_BATT_NO_CAPACITY, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );
        }
    }

    if( g_battStatusPage == 1 ) {

        int16_t current = g_battery.getAvgCurrent();
        int16_t power = g_battery.getAvgPower();

        if( current != 0 ) {
            g_lcd.setPosition( 0, 0 );
            snprintf_P( buffer, sizeof( buffer ), S_BATT_CURRENT, current );
            g_lcd.print( buffer, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );

            g_lcd.setPosition( 1, 0 );
            snprintf_P( buffer, sizeof( buffer ), S_BATT_POWER, power );
            g_lcd.print( buffer, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );

        } else {
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_BATT_NO_CURRENT, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );

            g_lcd.setPosition( 1, 0 );
            g_lcd.print_P( S_BATT_NO_POWER, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );
        }
    }

    if( g_battStatusPage == 2 ) {
        int8_t soh = g_battery.getStateOfHealth();
        int16_t fcap = g_battery.getFullCapacity( false );

        /* Print Full capacity */
        g_lcd.setPosition( 0, 0 );


        if( fcap > 0 ) {
            snprintf_P( buffer, sizeof( buffer ), S_BATT_FULL_CAP, fcap );
            g_lcd.print( buffer, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );
        } else {
            g_lcd.print_P( S_BATT_NO_FULL_CAP, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );
        }

        /* Print state of health */
        g_lcd.setPosition( 1, 0 );

        if( soh >= 0 ) {
            snprintf_P( buffer, sizeof( buffer ), S_BATT_SOH, soh );
            g_lcd.print( buffer, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );

        } else {
            g_lcd.print_P( S_BATT_NO_SOH, DISPLAY_WIDTH - 1, TEXT_ALIGN_LEFT );
        }

    }

    return false;
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
bool battStatus_onKeypress( Screen* screen, uint8_t key ) {

    if( key == KEY_NEXT ) {
        g_battStatusPage++;

        if( g_battStatusPage > 2 ) {
            g_battStatusPage = 0;
        }

        g_screenUpdate = true;
        g_screenClear = true;

        return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when the exit screen timeout delay has elapsed.
 *
 * @param   screen    Pointer to the screen where the event occured.
 */
void battStatus_onTimeout( Screen* screen ) {

    g_screenUpdate = true;
    screen->resetTimeout();
}


/*! ------------------------------------------------------------------------
 *
 * Event raised when entering the screen
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 * @return  TRUE to continue loading the screenor False otherwise
 * 
 */
bool battStatus_onEnterScreen( Screen* screen ) {
    g_battStatusPage = 0;

    screen->setTimeout( 1000 );
    return true;
}

