//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/commands/net.cpp
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
#include "../console.h"
#include "../../drivers/wifimanager.h"
#include "../../config.h"


/*--------------------------------------------------------------------------
 *
 * Starts a network reconnect task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful, FALSE if another task is already running.
 *           
 */
bool Console::startTaskNetRestart() {

     if( g_wifimanager.isBusy() ) {
        this->println_P( S_CONSOLE_WIFI_BUSY );
        this->println();
        return false;
    }

    _taskIndex = 0;
    this->startTask( TASK_CONSOLE_NET_RESTART );

    this->printf_P( S_CONSOLE_NET_RECONNECTING, g_config.settings.ssid );
    g_wifimanager.reconnect();

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Monitor the network reconnect task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 *           
 */
void Console::runTaskNetRestart() {

    wl_status_t status;
    status = g_wifimanager.status();

    switch( status ) {

        /* Connection successful */
        case WL_CONNECTED:
            this->println_P( S_CONSOLE_NET_CONNECTED );
            this->endTask( TASK_SUCCESS );
            break;

        /* Connection failure */
        case WL_DISCONNECTED:
        case WL_CONNECT_FAILED:
        case WL_NO_SSID_AVAIL:
            this->printf_P( S_CONSOLE_NET_CONNECT_FAIL, status );
            this->endTask( status );
            break;

        /* Request is still running */
        default:
            break;
    }
}


/*--------------------------------------------------------------------------
 *
 * Starts a network status print task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful, FALSE if another task is already running.
 *           
 */
bool Console::startTaskNetStatus() {
    this->startTask( TASK_CONSOLE_NET_STATUS );

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Run the network status print task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 *           
 */
void Console::runTaskNetStatus() {
    IPAddress addr;

    /* Print connection status */
    this->print_P( S_CONSOLE_NET_STATUS );

    if( g_wifimanager.isConnected() == true ) {
        this->println_P( S_CONSOLE_NET_CONNECTED );
    } else {
        this->println_P( S_CONSOLE_NET_DISCONNECTED );
    }
    
    /* Print SSID */
    this->printf_P( S_CONSOLE_NET_SSID, g_config.settings.ssid );

    /* Print if DHCP is ON or OFF */
    this->print_P( S_CONSOLE_NET_DHCP );
    this->println_P( g_config.settings.net_dhcp == true ? S_YES : S_NO );

    /* Print local IP address */
    addr = g_wifimanager.getLocalIP();
    this->printf_P( S_CONSOLE_NET_IP, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );

    /* Print subnet mask */
    addr = g_wifimanager.getSubmask();
    this->printf_P( S_CONSOLE_NET_MASK, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );

    /* Print gateway address */
    addr = g_wifimanager.getGateway();
    this->printf_P( S_CONSOLE_NET_GATEWAY, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );

    /* Print DNS server address */
    addr = g_wifimanager.getDNS();
    this->printf_P( S_CONSOLE_NET_DNS, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );


    this->endTask( TASK_SUCCESS );

}


/*--------------------------------------------------------------------------
 *
 * Starts a DNS resolve task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful, FALSE if another task is already running or
 *           an invalid parameter is provided.
 *           
 */
bool Console::startTaskNslookup() {

    char* hostname;
    hostname = this->getInputParameter();

    if( hostname == 0 ) {

        this->println_P( S_CONSOLE_MISSING_PARAMATER );
        this->print_P( S_CONSOLE_USAGE );
        this->println_P( S_USAGE_NSLOOKUP );
        this->println();

        return false;
    }

    if( g_wifimanager.isConnected() == false ) {
        this->println_P( S_CONSOLE_NET_NOT_CONNECTED );
        this->println();
        return false;
    }

    if( g_wifimanager.isBusy() ) {
        this->println_P( S_CONSOLE_WIFI_BUSY );
        this->println();
        return false;
    }

    if( g_wifimanager.startHostnameResolve( hostname ) == false ) {
        this->println_P( S_CONSOLE_NET_INVALID_HOST );
        this->println();
        return false;
    }

    this->printf_P( S_CONSOLE_NET_DNS_QUERY, hostname );
    this->println();

    this->startTask( TASK_CONSOLE_NET_NSLOOKUP );

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Monitor the DNS resolve task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 *           
 */
void Console::runTaskNsLookup() {
    
    IPAddress result;
    if( g_wifimanager.getHostnameResolveResults( result )) {

        if( result != 0 ) {
            result.printTo( Serial );
            this->println();
        } else {

            this->println_P( S_CONSOLE_NET_PING_UNKNOWN );
        }

        this->endTask( TASK_SUCCESS );
    }
}


/*--------------------------------------------------------------------------
 *
 * Starts a host ping task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful, FALSE if another task is already running or
 *           an invalid parameter is provided.
 *           
 */
bool Console::startTaskPing() {

    char* host;
    host = this->getInputParameter();

    if( host == 0 ) {

        this->println_P( S_CONSOLE_MISSING_PARAMATER );
        this->print_P( S_CONSOLE_USAGE );
        this->println_P( S_USAGE_PING );
        this->println();

        return false;
    }

    if( g_wifimanager.isConnected() == false ) {
        this->println_P( S_CONSOLE_NET_NOT_CONNECTED );
        this->println();
        return false;
    }

    if( g_wifimanager.isBusy() ) {
        this->println_P( S_CONSOLE_WIFI_BUSY );
        this->println();
        return false;
    }


    IPAddress addr;
    addr.fromString( host );

    bool ret;
    if( addr != 0 ) {
        ret = g_wifimanager.startPing( addr );
        this->printf_P( S_CONSOLE_NET_PING_IP, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
        this->println();

    } else {
        ret = g_wifimanager.startPing( host );
        this->printf_P( S_CONSOLE_NET_PING_HOSTNAME, host );
        this->println();
    }

    if( ret == false ) {
        this->println_P( S_CONSOLE_NET_PING_ERROR );
        this->println();
        return false;
    }

    this->startTask( TASK_CONSOLE_NET_PING );

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Monitor the host ping task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 *           
 */
void Console::runTaskPing() {

    IPAddress ip;
    int32_t rtt;

    rtt = g_wifimanager.getPingResult( ip );

    /* Ping is still running */
    if( rtt == 0 ) {
        return;
    }

    /* Ping successful */
    if( rtt > 0 ) {
        this->printf_P( S_CONSOLE_NET_PING_RESULT, ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ], rtt );

        this->endTask( TASK_SUCCESS );

    /* Ping error */
    } else {

        switch( rtt ) {
            case WL_PING_DEST_UNREACHABLE:
                this->println_P( S_CONSOLE_NET_PING_UNREACH );
                break;

            case WL_PING_ERROR:
                this->println_P( S_CONSOLE_NET_PING_ERROR );
                break;

            case WL_PING_TIMEOUT:
                this->println_P( S_CONSOLE_NET_PING_TIMEOUT );
                break;

            case WL_PING_UNKNOWN_HOST:
                this->println_P( S_CONSOLE_NET_PING_UNKNOWN );
                break;
        }

        this->endTask( rtt );          
    }    
    
}