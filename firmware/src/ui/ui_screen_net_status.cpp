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

#include <config.h>
#include <IPAddress.h>
#include "ui.h"



uint8_t g_netStatusPage = 0;



/*******************************************************************************
 *
 * @brief   Event raised when updating the screen.
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 * @return  TRUE to allow default screen update, FALSE to override.
 * 
 */
bool netStatus_onDrawScreen( Screen* screen ) {

    IPAddress addr;

    switch( g_netStatusPage ) {
        case 0:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_IP );

            addr = g_wifi.getLocalIP();

            break;

        case 1:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_GATEWAY );

            addr = g_wifi.getGateway();

            break;

        case 2:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_MASK );

            addr = g_wifi.getSubmask();

            break;

        case 3:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_MENU_NETWORK_DNS );

            addr = g_wifi.getDNS();

            break;

        case 4:
            g_lcd.setPosition( 0, 0 );
            g_lcd.print_P( S_SSID );

            g_lcd.setPosition( 1, 0 );
            g_lcd.print( g_config.network.ssid, 16, TEXT_ALIGN_LEFT);
            break;
    }

    if( g_netStatusPage < 4 ) {
        g_lcd.setPosition( 1, 0 );
        g_lcd.printf_P( S_NETINFO_IP, addr[0], addr[1], addr[2], addr[3] );
    }

    return false;
}


/*******************************************************************************
 *
 * @brief   Event raised when a key press occurs.
 *
 * @param   screen    Pointer to the screen where the event occured.
 * @param   key       Detected key press.
 *
 * @return  TRUE to allow default key press processing, FALSE to override.
 * 
 */
bool netStatus_onKeypress( Screen* screen, uint8_t key ) {

    if( key == KEY_NEXT ) {
        g_netStatusPage++;

        if( g_netStatusPage > 4 ) {
            g_netStatusPage = 0;
        }

        g_screen.requestScreenUpdate( true );

        return false;
    }

    return true;
}


/*******************************************************************************
 *
 * @brief   Event raised when entering the screen.
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 * 
 */
void netStatus_onEnterScreen( Screen* screen, uint8_t prevScreenID ) {

    if( prevScreenID != SCREEN_ID_SUSPEND ) {
        g_netStatusPage = 0;
    }

}

