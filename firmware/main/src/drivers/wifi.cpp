#include "wifi.h"
#include "../hardware.h"
#include "../config.h"

void updateWifiConfig() {

    IPAddress net_ip( &g_config.net_ip[0] );
    IPAddress net_mask( &g_config.net_mask[0] );
    IPAddress net_gateway( &g_config.net_gateway[0] );
    IPAddress net_dns( &g_config.net_dns[0] );

    g_wifi.config( g_config.net_dhcp, net_ip, net_dns, net_gateway, net_mask );

}

void enableWifi() {

    digitalWrite( PIN_WIFI_RESET, HIGH );

    /* Init wifi module */
    g_wifi.setPins( PIN_WIFI_CS, PIN_WIFI_IRQ, PIN_WIFI_RESET, PIN_WIFI_ENABLE );
    g_wifi.begin();


    updateWifiConfig();

    g_wifi.connect( g_config.ssid, g_config.wkey );
}


void disableWifi() {

    g_wifi.end();

    digitalWrite( PIN_WIFI_RESET, LOW );
}
