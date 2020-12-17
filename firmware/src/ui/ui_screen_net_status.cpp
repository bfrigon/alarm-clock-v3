//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_net_status.cpp
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
#include <IPAddress.h>

uint8_t g_netStatusPage = 0;


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
bool netStatus_onDrawScreen( Screen* screen ) {

    char buffer[ DISPLAY_WIDTH + 1 ];

    IPAddress addr;

    switch( g_netStatusPage ) {
        case 0:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_IP );

            addr = g_wifimanager.getLocalIP();

            break;

        case 1:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_GATEWAY );

            addr = g_wifimanager.getGateway();

            break;

        case 2:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_MASK );

            addr = g_wifimanager.getSubmask();

            break;

        case 3:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_DNS );

            addr = g_wifimanager.getDNS();

            break;

        case 4:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_SSID );

            g_lcd.setPosition( 1, 0 );
            g_lcd.print( g_config.settings.ssid, 16, TEXT_ALIGN_LEFT, false );
            break;
    }

    if( g_netStatusPage < 4 ) {
        g_lcd.setPosition( 1, 0 );
        g_lcd.printf_P( S_NETINFO_IP, addr[0], addr[1], addr[2], addr[3] );
    }

    return false;
}


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
bool netStatus_onKeypress( Screen* screen, uint8_t key ) {

    if( key == KEY_NEXT ) {
        g_netStatusPage++;

        if( g_netStatusPage > 4 ) {
            g_netStatusPage = 0;
        }

        g_screenUpdate = true;
        g_screenClear = true;

        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Event raised when entering the screen
 *
 * Arguments
 * ---------
 *  - screen : Pointer to the screen where the event occured.
 *
 * Returns : TRUE to continue loading the screenor False otherwise
 */
bool netStatus_onEnterScreen( Screen* screen ) {
    g_netStatusPage = 0;

    return true;
}

