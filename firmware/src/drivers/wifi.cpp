//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi.cpp
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
#include "wifi.h"
#include "../hardware.h"
#include "../config.h"


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void updateWifiConfig() {

    /*
    IPAddress net_ip( &g_config.settings.net_ip[0] );
    IPAddress net_mask( &g_config.settings.net_mask[0] );
    IPAddress net_gateway( &g_config.settings.net_gateway[0] );
    IPAddress net_dns( &g_config.settings.net_dns[0] );

    g_wifi.config( g_config.settings.net_dhcp, net_ip, net_dns, net_gateway, net_mask );
    */

   
   
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void enableWifi() {

    digitalWrite( PIN_WIFI_RESET, HIGH );

    /* Init wifi module */
    //g_wifi.setPins( PIN_WIFI_CS, PIN_WIFI_IRQ, PIN_WIFI_RESET, PIN_WIFI_ENABLE );
    //g_wifi.begin();


    updateWifiConfig();

    //g_wifi.connect( g_config.settings.ssid, g_config.settings.wkey );
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void disableWifi() {

    //g_wifi.end();

    digitalWrite( PIN_WIFI_RESET, LOW );
}
