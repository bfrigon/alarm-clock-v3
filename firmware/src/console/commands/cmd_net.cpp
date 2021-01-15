//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/commands/cmd_net.cpp
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
#include "../../drivers/wifi/wifi.h"
#include "../../config.h"


/*! ------------------------------------------------------------------------
 * 
 * @brief   Starts the 'net restart' command task
 * 
 * @return  TRUE if successful, FALSE if another task is already running.
 * 
 */
bool Console::startTaskNetRestart() {

     if( g_wifi.isBusy() && g_wifi.isConnected() ) {
        this->println_P( S_CONSOLE_WIFI_BUSY );
        this->println();
        return false;
    }

    _taskIndex = 0;
    this->startTask( TASK_CONSOLE_NET_RESTART, true );

    this->printf_P( S_CONSOLE_NET_RECONNECTING, g_config.network.ssid );
    g_wifi.reconnect();
    g_wifi.setAutoReconnect( true );

    return true;
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Monitor the 'net restart' command task
 * 
 * @return  TRUE if successful, FALSE if another task is already running.
 * 
 */
void Console::runTaskNetRestart() {

    if( g_wifi.isBusy() == true ) {
        return;
    }

    wl_status_t status;
    status = g_wifi.status();

    switch( status ) {

        /* Connection successful */
        case WIFI_STATUS_CONNECTED:
            this->println_P( S_CONSOLE_NET_CONNECTED );
            this->endTask( TASK_SUCCESS );
            break;

        /* Connection failure */
        case WIFI_STATUS_DISCONNECTED:
        case WIFI_STATUS_CONNECT_FAILED:
        case WIFI_STATUS_NO_SSID_AVAIL:
            this->printf_P( S_CONSOLE_NET_CONNECT_FAIL, status );
            this->endTask( status );
            break;

        /* Request is still running */
        default:
            break;
    }
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Starts the 'net start' command task
 * 
 * @return  TRUE if successful, FALSE if another task is already running.
 * 
 */
bool Console::startTaskNetStart() {
    if( g_wifi.isConnected() == true ) {
        this->println_P( S_CONSOLE_NET_ALREADY_CONN );
        this->println();
        return false;
    }

    this->startTask( TASK_CONSOLE_NET_START, true );

    this->printf_P( S_CONSOLE_NET_CONNECTING, g_config.network.ssid );
    g_wifi.connect();
    g_wifi.setAutoReconnect( true );

    return true;
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Starts the 'net stop' command task
 * 
 * @return  TRUE if successful, FALSE if another task is already running.
 * 
 */
bool Console::startTaskNetStop() {
    if( g_wifi.isConnected() == false ) {
        this->println_P( S_CONSOLE_NET_NOT_CONNECTED );
        this->println();
        return false;
    }

    if( g_wifi.isBusy() && g_wifi.isConnected() ) {
        this->println_P( S_CONSOLE_WIFI_BUSY );
        this->println();
        return false;
    }
    
    this->startTask( TASK_CONSOLE_NET_STOP, true );

    g_wifi.setAutoReconnect( false );
    g_wifi.disconnect();

    return true;
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Monitor the 'net stop' command task
 * 
 */
void Console::runTaskNetStop() {
    
    if( g_wifi.isConnected() == false ) {
        this->println_P( S_CONSOLE_NET_DISCONNECTED );

        this->endTask( TASK_SUCCESS );
    }
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Print the status of the WiFi connection to the console
 * 
 */
void Console::printNetStatus() {

    IPAddress addr;

    /* Print connection status */
    this->print_P( S_CONSOLE_NET_STATUS );

    if( g_wifi.isConnected() == true ) {
        this->println_P( S_CONSOLE_NET_CONNECTED );
    } else {
        this->println_P( S_CONSOLE_NET_DISCONNECTED );
    }
    
    /* Print SSID */
    this->printf_P( S_CONSOLE_NET_SSID, g_config.network.ssid );

    /* Print if DHCP is ON or OFF */
    this->print_P( S_CONSOLE_NET_DHCP );
    this->println_P( g_config.network.dhcp == true ? S_YES : S_NO );

    /* Print local IP address */
    addr = g_wifi.getLocalIP();
    this->printf_P( S_CONSOLE_NET_IP, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );

    /* Print subnet mask */
    addr = g_wifi.getSubmask();
    this->printf_P( S_CONSOLE_NET_MASK, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );

    /* Print gateway address */
    addr = g_wifi.getGateway();
    this->printf_P( S_CONSOLE_NET_GATEWAY, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );

    /* Print DNS server address */
    addr = g_wifi.getDNS();
    this->printf_P( S_CONSOLE_NET_DNS, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Starts the 'nslookup' command task
 * 
 * @return  TRUE if successful, FALSE if another task is already running or
 *          an invalid parameter is provided.
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

    if( g_wifi.isBusy() ) {
        this->println_P( S_CONSOLE_WIFI_BUSY );
        this->println();

        return false;
    }

    if( g_wifi.startHostnameResolve( hostname ) == false ) {
        switch( g_wifi.getTaskError() ) {
            case ERR_WIFI_NOT_CONNECTED:
                this->println_P( S_CONSOLE_NET_NOT_CONNECTED );
                this->println();
                break;

            case ERR_WIFI_INVALID_HOSTNAME:
                this->println_P( S_CONSOLE_NET_INVALID_HOST );
                this->println();
                break;
        }

        return false;
    }

    this->printf_P( S_CONSOLE_NET_DNS_QUERY, hostname );
    this->println();

    this->startTask( TASK_CONSOLE_NET_NSLOOKUP );

    return true;
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Monitor the 'nslookup' command task. Display prompts and validate 
 *          input required before executing the task. 
 * 
 */
void Console::runTaskNsLookup() {
    
    IPAddress result;
    if( g_wifi.getHostnameResolveResults( result )) {

        if( result != 0 ) {
            result.printTo( Serial );
            this->println();
        } else {

            this->println_P( S_CONSOLE_NET_PING_UNKNOWN );
        }

        this->endTask( TASK_SUCCESS );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'ping' command task
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

    if( g_wifi.isConnected() == false ) {
        this->println_P( S_CONSOLE_NET_NOT_CONNECTED );
        this->println();
        return false;
    }

    if( g_wifi.isBusy() ) {
        this->println_P( S_CONSOLE_WIFI_BUSY );
        this->println();
        return false;
    }


    IPAddress addr;
    addr.fromString( host );

    bool ret;
    if( addr != 0 ) {
        ret = g_wifi.startPing( addr );
        this->printf_P( S_CONSOLE_NET_PING_IP, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
        this->println();

    } else {
        ret = g_wifi.startPing( host );
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Monitor the 'ping' command task. 
 *           
 */
void Console::runTaskPing() {

    IPAddress ip;
    int32_t rtt;

    rtt = g_wifi.getPingResult( ip );

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
            case ERR_WIFI_NETWORK_UNREACHABLE:
                this->println_P( S_CONSOLE_NET_PING_UNREACH );
                break;

            case ERR_WIFI_PING_ERROR:
                this->println_P( S_CONSOLE_NET_PING_ERROR );
                break;

            case ERR_WIFI_PING_TIMEOUT:
                this->println_P( S_CONSOLE_NET_PING_TIMEOUT );
                break;

            case ERR_WIFI_UNKNOWN_HOSTNAME:
                this->println_P( S_CONSOLE_NET_PING_UNKNOWN );
                break;
        }

        this->endTask( rtt );          
    }    
    
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'net config' command task
 *           
 */
bool Console::startTaskNetworkConfig() {

    _taskIndex = 0;

    this->println();
    this->println_P( S_CONSOLE_NET_CFG_INSTR );
    this->println();

    this->startTask( TASK_CONSOLE_NET_CONFIG );

    /* Disable auto-reconnect while entering settings */
    g_wifi.setAutoReconnect( false );

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run the 'net config' command task. Display prompts and validate 
 *          responses required before executing the task.
 * 
 */
void Console::runTaskNetworkConfig() {
    
    IPAddress addr;

    /* Even index display the prompt, odd index process user input */
    if( _taskIndex % 2 ) {

        if( this->processInput() == false ) {
            return;
        }

        this->trimInput();
    }

    switch( _taskIndex++ ) {

        /* Display network SSID prompt */
        case 0:
            this->printf_P( S_CONSOLE_NET_CFG_SSID, g_config.network.ssid );

            _inputBufferLimit = MAX_SSID_LENGTH;
            break;

        /* Validate network SSID input */
        case 1:
            if( _inputlength > 0 ) {
                strncpy( g_config.network.ssid, _inputbuffer, MAX_SSID_LENGTH );
                g_config.network.ssid[ MAX_SSID_LENGTH ] = '\0';
            }
            break;

        /* Display WiFi password prompt */
        case 2:
            this->print_P( S_CONSOLE_NET_CFG_KEY );

            _inputBufferLimit = MAX_WKEY_LENGTH;
            _inputHidden = true;
            break;

        /* Validate WiFi password prompt */
        case 3:
            if( _inputlength > 0 ) {
                strncpy( g_config.network.wkey, _inputbuffer, MAX_WKEY_LENGTH );
                g_config.network.wkey[ MAX_WKEY_LENGTH ] = '\0';
            }
            break;

        /* Display use DHCP prompt */
        case 4:
            _inputBufferLimit = 1;
            _inputHidden = false;

            this->printf_P( S_CONSOLE_NET_CFG_DHCP, (g_config.network.dhcp == true ? "Y" : "N" ));
            break;

        /* Validate use DHCP answer */
        case 5:
            if( tolower( _inputbuffer[ 0 ] ) == 'y' ) {
                g_config.network.dhcp = true;

                /* Skips static ip settings */
                _taskIndex = 14;

            } else if ( tolower( _inputbuffer[ 0 ] ) == 'n' ) {
                g_config.network.dhcp = false;

            } else if ( _inputlength > 0 ) {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );
                this->println();

                /* try again */
                _taskIndex = 4;
            } else {

                if( g_config.network.dhcp == true ) {

                    /* Skips static ip settings */
                    _taskIndex = 14;
                }
            }
            break;

        /* Display local ip address prompt */
        case 6:
            _inputBufferLimit = INPUT_BUFFER_LENGTH;

            addr = g_config.network.ip;
            this->printf_P( S_CONSOLE_NET_CFG_IP, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
            break;

        /* Validate local ip address prompt */
        case 7:
            if( _inputlength > 0 && addr.fromString( _inputbuffer ) == true ) {
                IPADDRESS_TO_ARRAY( addr, g_config.network.ip );
                
            } else if( _inputlength > 0 ) {
                this->println_P( S_CONSOLE_INVALID_INPUT_IP );
                this->println();

                /* try again */
                _taskIndex = 6;

            }
            break;

        /* Display subnet mask prompt */
        case 8:
            addr = g_config.network.mask;
            this->printf_P( S_CONSOLE_NET_CFG_SUBNET, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
            break;

        /* Validate local ip address prompt */
        case 9:
            if( _inputlength > 0 && addr.fromString( _inputbuffer ) == true ) {
                IPADDRESS_TO_ARRAY( addr, g_config.network.mask );
                
            } else if( _inputlength > 0 ) {
                this->println_P( S_CONSOLE_INVALID_INPUT_SUB );
                this->println();

                /* try again */
                _taskIndex = 8;

            }
            break;  

        /* Display gateway address prompt */
        case 10:
            addr = g_config.network.gateway;
            this->printf_P( S_CONSOLE_NET_CFG_GATEWAY, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
            break;

        /* Validate gateway address prompt */
        case 11:
            if( _inputlength > 0 && addr.fromString( _inputbuffer ) == true ) {
                IPADDRESS_TO_ARRAY( addr, g_config.network.gateway );
                
            } else if( _inputlength > 0 ) {
                this->println_P( S_CONSOLE_INVALID_INPUT_IP );
                this->println();

                /* try again */
                _taskIndex = 10;

            }
            break;       

        /* Display DNS address prompt */
        case 12:
            addr = g_config.network.dns;
            this->printf_P( S_CONSOLE_NET_CFG_DNS, addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
            break;

        /* Validate DNS address prompt */
        case 13:
            if( _inputlength > 0 && addr.fromString( _inputbuffer ) == true ) {
                IPADDRESS_TO_ARRAY( addr, g_config.network.dns );

                /* Skip hostname since dhcp is not used */
                _taskIndex = 16;

            } else if( _inputlength == 0 ) {

                /* Skip hostname since dhcp is not used */
                _taskIndex = 16;
                
            } else if( _inputlength > 0 ) {
                this->println_P( S_CONSOLE_INVALID_INPUT_IP );
                this->println();

                /* try again */
                _taskIndex = 12;

            }
            break;

        /* Display hostname prompt */
        case 14:
            _inputBufferLimit = MAX_HOSTNAME_LENGTH;

            addr = g_config.network.gateway;
            this->printf_P( S_CONSOLE_NET_CFG_HOSTNAME, g_config.network.hostname );
            break;

        /* Validate hostname prompt */
        case 15:
            if( _inputlength > 0 ) {
                strncpy( g_config.network.hostname, _inputbuffer, MAX_HOSTNAME_LENGTH );
                g_config.network.hostname[ MAX_HOSTNAME_LENGTH ] = '\0';
            }
            break;     

        /* Display ntp server prompt */
        case 16:
            _inputBufferLimit = MAX_NTPSERVER_LENGTH;

            addr = g_config.network.gateway;
            this->printf_P( S_CONSOLE_NET_CFG_NTPSERVER, g_config.network.ntpserver );
            break;

        /* Validate hostname prompt */
        case 17:
            if( _inputlength > 0 ) {
                strncpy( g_config.network.ntpserver, _inputbuffer, MAX_NTPSERVER_LENGTH );
                g_config.network.ntpserver[ MAX_NTPSERVER_LENGTH ] = '\0';
            }
            break;  


        /* Display apply settings prompt */
        case 18:
            _inputBufferLimit = 1;

            this->println();
            this->print_P( S_CONSOLE_NET_CFG_APPLY );
            break;

        /* Validate apply settings answer */
        case 19:

            if( tolower( _inputbuffer[ 0 ] ) == 'y' ) {

                /* Save and apply the new network configuration */
                g_config.save( EEPROM_SECTION_NETWORK );
                g_config.apply( EEPROM_SECTION_NETWORK );

            } else if ( tolower( _inputbuffer[ 0 ] ) == 'n' ) {
                g_config.load( EEPROM_SECTION_NETWORK );

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );

                /* try again */
                _taskIndex = 18;
            }
            break;
            
    }

    this->resetInput();    

    
    if (_taskIndex > 19 ) {
        this->endTask( TASK_SUCCESS );

        if( g_wifi.isConnected() == true ) {
            g_wifi.setAutoReconnect( true );
        }
    }
}