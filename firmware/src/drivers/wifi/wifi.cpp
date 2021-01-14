//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi.cpp
// Author  : Benoit Frigon <www.bfrigon.com>
//
// Credit  : This file contains large portions of code from the WiFi101
//           arduino library. It has been adapted to make the calls to the
//           WiFi module non-blocking. Original code at: 
//           https://github.com/arduino-libraries/WiFi101/blob/master/src/WiFi.cpp
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
#include "../../hardware.h"
#include "../../config.h"
#include "../../ui/ui.h"


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - pin_cs    : Module select pin
 *  - pin_xdcs  : Interrupt request pin
 *  - pin_dreq  : Module reset pin.
 *  - pin_reset : Module enable pin.
 */
WiFi::WiFi( int8_t pin_cs, int8_t pin_irq, int8_t pin_rst, int8_t pin_en ) {

	gi8Winc1501CsPin = pin_cs;
	gi8Winc1501IntnPin = pin_irq;
	gi8Winc1501ResetPin = pin_rst;
	gi8Winc1501ChipEnPin = pin_en;
}


/*--------------------------------------------------------------------------
 *
 * Initialize the WiFi module and establish connection
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void WiFi::begin() {
    if( _init == false ) {
		init();
	}

    this->connect();
}


/*--------------------------------------------------------------------------
 *
 * De-init the WiFi module
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void WiFi::end() {
    if( _init == false ) {
		return;
	}

    _status = WIFI_STATUS_IDLE;
    _init = false;        

    
    this->disconnect();

    /* Uninitialize socket class */
    socketDeinit();

    /* Uninitialize the WiFi module */
	m2m_wifi_deinit( NULL );

    /* Unitiialize the board support for the WiFi module */
	nm_bsp_deinit();
}


/*--------------------------------------------------------------------------
 *
 * Initialize the WiFi module
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
int WiFi::init() {
    
    tstrWifiInitParam param;
	int8_t ret;

	/* Initialize the board support for the WiFi module */
	nm_bsp_init();

    /* Initialize the WiFi module and register notification callback */
	param.pfAppWifiCb = wifimanager_wifi_cb;
	ret = m2m_wifi_init(&param);   

    if( M2M_SUCCESS != ret && M2M_ERR_FW_VER_MISMATCH != ret ) {
        return ret;
    }

    /* Initialize the socket class */
    socketInit();

    /* Register socket callback routines */
	registerSocketCallback( wifimanager_socket_cb, wifimanager_resolve_cb );
    

	_init = true;
	_status = WIFI_STATUS_IDLE;
	_localip = 0;
	_submask = 0;
	_gateway = 0;
	_dhcp = true;
	_resolve = 0;


	extern uint32 nmdrv_firm_ver;

	if( nmdrv_firm_ver >= M2M_MAKE_VERSION( 19, 5, 0 )) {

		/* enable AES-128 and AES-256 Ciphers, if firmware is 19.5.0 or higher */
		m2m_ssl_set_active_ciphersuites( SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA | 
                                         SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA256 | 
                                         SSL_CIPHER_RSA_WITH_AES_128_GCM_SHA256 | 
                                         SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA | 
                                         SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA256 );
	}


    return ret;
}


/*--------------------------------------------------------------------------
 *
 * Re-establish the connection to the WiFi network. 
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : WL_IDLE_STATUS if successful or error otherwise.
 */
wl_status_t WiFi::reconnect() {

	if( _init == false ) {
		init();
	}

    if( _status == WIFI_STATUS_CONNECTED ) {
        if( this->getCurrentTask() != TASK_NONE ) {
            this->endTask( WIFI_STATUS_DISCONNECTED );
        }
        
        this->startTask( TASK_WIFI_RECONNECT );

        /* Sends disconnect request */
        this->disconnect();

        /* Status remains IDLE until a disconnect event is 
           received. Then, runTask will send a connect request. */
        return WIFI_STATUS_IDLE;

    } else {

        /* Not connected, send a connect request */
        return this->connect();
    } 

}


/*--------------------------------------------------------------------------
 *
 * Sets whether or not the WiFi manager should attempt to reconnect when
 * it loses connection.
 *
 * Arguments
 * ---------
 *  - autoReconnect: TRUE to enable auto-reconnect, FALSE otherwise
 *
 * Returns : Nothing
 */
void WiFi::setAutoReconnect( bool autoReconnect ) {
    _autoReconnect = autoReconnect;
}


/*--------------------------------------------------------------------------
 *
 * Connect to the WiFi network set in config.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : WL_IDLE_STATUS if successful or error otherwise.
 */
wl_status_t WiFi::connect() {

    tstrM2MIPConfig conf;

	if( _init == false ) {
		init();
	}
    
    /* Connection already established */
    if( _status == WIFI_STATUS_CONNECTED ) {
        return WIFI_STATUS_CONNECTED;
    }

    if( this->getCurrentTask() != TASK_NONE ) {
        this->endTask( WIFI_STATUS_DISCONNECTED );
    }

    /* Starts a task to monitor connection progress */
    this->startTask( TASK_WIFI_CONNECT );

    /* Reset the last connection attempt timer */
    _lastConnectAttempt = millis();

    if( g_config.network.dhcp == false ) {

        _dhcp = false;
        m2m_wifi_enable_dhcp( 0 );    // disable DHCP

        conf.u32DNS = IPAddress( &g_config.network.dns[0] );
        conf.u32Gateway = IPAddress( &g_config.network.gateway[0] );
        conf.u32StaticIP = IPAddress( &g_config.network.ip[0] );
        conf.u32SubnetMask = IPAddress( &g_config.network.mask[0] );
        m2m_wifi_set_static_ip( &conf );

        _localip = conf.u32StaticIP;
        _submask = conf.u32SubnetMask;
        _gateway = conf.u32Gateway;
        _dns = conf.u32DNS;

    } else {
        _dhcp = true;
        m2m_wifi_enable_dhcp( 1 );

        _localip = 0;
        _submask = 0;
        _gateway = 0;
        _dns = 0;

    }

    
    const char *ssid = g_config.network.ssid;
    const void *pvAuthInfo = g_config.network.wkey;

    /* Start connection to the WiFi network */
    if( m2m_wifi_connect( (char*)ssid, strlen(ssid), M2M_WIFI_SEC_WPA_PSK, (void*)pvAuthInfo, M2M_WIFI_CH_ALL) < 0 ) {
		_status = WIFI_STATUS_CONNECT_FAILED;

        this->endTask( _status );
		return _status;
	}

    /* Set the hostname only if DHCP is used */
    if( _dhcp == true ) {
        m2m_wifi_set_device_name( (uint8 *)g_config.network.hostname, strlen(g_config.network.hostname ));
    }

    /* Status remains idle until DHCP response has been received or a connection 
       is established if the address is static */
    _status = WIFI_STATUS_IDLE;

    return _status;
}


/*--------------------------------------------------------------------------
 *
 * Disconnects currently established connection.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void WiFi::disconnect() {

    if( _init == false ) {
		return;
	}

    if( _status != WIFI_STATUS_CONNECTED ) {
        return;
    }

    /* Close sockets to clean state */
	for( int i = 0; i < MAX_SOCKET; i++ ) {
		g_wifisocket.close( i );
	}

	m2m_wifi_disconnect();
}


/*--------------------------------------------------------------------------
 *
 * Gets the local IP address assigned by the DHCP or configured ip address if 
 * not using DHCP.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Local IP address 
 */
uint32_t WiFi::getLocalIP() 
{
    return this->_localip;
}


/*--------------------------------------------------------------------------
 *
 * Gets the gateway address set by the DHCP or configured gateway if not
 * using DHCP.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Gateway address 
 */
uint32_t WiFi::getGateway()
{
    return this->_gateway;
}


/*--------------------------------------------------------------------------
 *
 * Gets the subnet mask set by the DHCP or configured mask if not
 * using DHCP.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Subnet mask
 */
uint32_t WiFi::getSubmask()
{
    return this->_submask;
}


/*--------------------------------------------------------------------------
 *
 * Gets the primary DNS assigned by DHCP or configured DNS address if not
 * using DHCP.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : DNS address 
 */
uint32_t WiFi::getDNS()
{
    return this->_dns;
}


/*--------------------------------------------------------------------------
 *
 * WiFi module notification handler
 *
 * Arguments
 * ---------
 *  u8MsgType : Message type
 *  pvMsg     : Pointer to message data
 *
 * Returns : Nothing
 */
void WiFi::handleEvent(uint8_t u8MsgType, void *pvMsg)
{
    switch( u8MsgType ) {
		case M2M_WIFI_RESP_CON_STATE_CHANGED: {

            tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
			if( pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED ) {

                if( _dhcp == false ) {
                    _status = WIFI_STATUS_CONNECTED;
                }


            } else {

                if( this->getCurrentTask() == TASK_WIFI_CONNECT ) {
                    this->endTask( WIFI_STATUS_DISCONNECTED );
                }

                _status = WIFI_STATUS_DISCONNECTED;

                if ( _dhcp == true ) {
                    _localip = 0;
                    _gateway = 0;
                    _submask = 0;
                    _dns = 0;
                }
                
                for (int i = 0; i < MAX_SOCKET; i++) {
                    g_wifisocket.close(i);
                }
            }
        }
        break;

        case M2M_WIFI_REQ_DHCP_CONF: {
            tstrM2MIPConfig *pstrIPCfg = (tstrM2MIPConfig *)pvMsg;
            _localip = pstrIPCfg->u32StaticIP;
            _submask = pstrIPCfg->u32SubnetMask;
            _gateway = pstrIPCfg->u32Gateway;
            _dns = pstrIPCfg->u32DNS;
            
            _status = WIFI_STATUS_CONNECTED;

            
        }
        break;


        // case M2M_WIFI_RESP_GET_SYS_TIME: {

            
        //     tstrSystemTime *dt = (tstrSystemTime *)pvMsg; 

        //     Serial.print( "WiFi module time : ");
        //     Serial.print( dt->u16Year );
        //     Serial.print( "-");
        //     Serial.print( dt->u8Month );
        //     Serial.print( "-");
        //     Serial.print( dt->u8Day );
        //     Serial.print( " ");
        //     Serial.print( dt->u8Hour );
        //     Serial.print( ":");
        //     Serial.print( dt->u8Minute );
        //     Serial.print( ":");
        //     Serial.print( dt->u8Second );
        //     Serial.println();

        // }
        // break;

    	default:
		break;
    }
}


/*--------------------------------------------------------------------------
 *
 * DNS resolution handler
 *
 * Arguments
 * ---------
 *  - hostName : Domain name of the host
 *  - hostIp   : IP address of the host
 *
 * Returns : Nothing
 */
void WiFi::handleResolve( uint8 *hostName, uint32_t hostIp )
{
    _resolve = hostIp;  

    if( this->getCurrentTask() == TASK_WIFI_RESOLVE ) {
        this->endTask( TASK_SUCCESS );
    }

    if( this->getCurrentTask() == TASK_WIFI_PING_HOSTNAME ) {

        if( _resolve == 0 ) {
            _rtt = ERR_WIFI_UNKNOWN_HOSTNAME;
            this->endTask( _rtt );
        } else {

            this->endTask( TASK_SUCCESS );

            this->startPing( hostIp );
        }
    }
}


/*--------------------------------------------------------------------------
 *
 * Ping result callback
 *
 * Arguments
 * ---------
 *  - ip      : Destination IP.
 *  - rtt     : Round trip time.
 *  - error   : Error code or PING_ERR_SUCCESS if successful.
 *
 * Returns : Nothing
 */
void WiFi::handlePingResponse( uint32 ip, uint32 rtt, uint8 error ) {


	if( error == PING_ERR_SUCCESS ) {

		/* Ensure this ICMP reply comes from requested IP address */
		if ( _resolve == ip ) {

			_rtt = (int32_t) rtt;
		} else {

			/* Another network device replied to the our ICMP request */
			_rtt = (int32_t) ERR_WIFI_NETWORK_UNREACHABLE;
		}

	} else if( error == PING_ERR_DEST_UNREACH ) {
		_rtt = (uint32_t) ERR_WIFI_NETWORK_UNREACHABLE;

	} else if( error == PING_ERR_TIMEOUT ) {
		_rtt = (uint32_t) ERR_WIFI_PING_TIMEOUT;

	} else {
		_rtt = (uint32_t) ERR_WIFI_PING_ERROR;
	}

    if( this->getCurrentTask() == TASK_WIFI_PING ) {
        this->endTask(( _rtt > 0 ) ? TASK_SUCCESS : _rtt );
    }
}


/*--------------------------------------------------------------------------
 *
 * Called when power management driver changes the current power mode
 *
 * Arguments
 * ---------
 *  - state : New power state
 *
 * Returns : Nothing
 */
void WiFi::onPowerStateChange( uint8_t state ) {

    if( _init == false ) {
		init();
	}

    if( state == POWER_MODE_NORMAL ) {
        this->connect();
    } else {
        this->disconnect();
    }
}


/*--------------------------------------------------------------------------
 *
 * Check if the connection is established
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if connection is established, FALSE otherwise. 
 *           If using DHCP, returns TRUE only once a valid DHCP response 
 *           has been received.
 *           
 */
bool WiFi::isConnected() {
    return ( _status == WIFI_STATUS_CONNECTED );
}


/*--------------------------------------------------------------------------
 *
 * Returns the connection status or error.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Connection status or error
 *           
 */
wl_status_t WiFi::status() {
    return _status;
}


/*--------------------------------------------------------------------------
 *
 * Starts a hostname resolve request on the WiFi module.
 *
 * Arguments
 * ---------
 *  - hostname : Hostname to resolve
 *
 * Returns : TRUE if successful, FALSE if a resolve request is already running 
 *           or an invalid hostname is provided
 *           
 */
bool WiFi::startHostnameResolve( const char *hostname ) {
    if( _init == false ) {
		return false;
	}

    if( this->startTask( TASK_WIFI_RESOLVE ) != TASK_WIFI_RESOLVE ) {
        
        return false;
    }

    if( this->_status != WIFI_STATUS_CONNECTED ) {

        this->endTask( ERR_WIFI_NOT_CONNECTED );
        return false;
    }

    _resolve = 0;

    if( gethostbyname( (uint8 *)hostname ) != SOCK_ERR_NO_ERROR ) {

        this->endTask( ERR_WIFI_INVALID_HOSTNAME );
        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Gets the result of the last resolve request.
 *
 * Arguments
 * ---------
 *  - result : IPADDRESS structure to write the result to.
 *
 * Returns : TRUE if successful, FALSE if a resolve request is still running
 *           
 */
bool WiFi::getHostnameResolveResults( IPAddress &result ) {
    if( _init == false ) {
		return false;
	}

    if( this->getCurrentTask() == TASK_WIFI_RESOLVE ) {
        return false;
    }

    result = _resolve;
    _resolve = 0;

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Starts a ping request on the WiFi module using a hostname.
 *
 * Arguments
 * ---------
 *  - hostname : Hostname to ping
 *
 * Returns : TRUE if successful, FALSE if another request is already running 
 *           or an invalid hostname is provided
 *           
 */
bool WiFi::startPing( const char* hostname ) {
    if( _init == false ) {
		return false;
	}

    if( this->startTask( TASK_WIFI_PING_HOSTNAME ) != TASK_WIFI_PING_HOSTNAME ) {
        return false;
    }

    if( this->_status != WIFI_STATUS_CONNECTED ) {
        this->endTask( ERR_WIFI_NOT_CONNECTED );
        return false;
    }

    _resolve = 0;
    _rtt = 0;

    if( gethostbyname( (uint8 *)hostname ) != SOCK_ERR_NO_ERROR ) {
        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Starts a ping request on the WiFi module using an IP address.
 *
 * Arguments
 * ---------
 *  - host : IP address to ping
 *
 * Returns : TRUE if successful, FALSE if another request is already running 
 *           or an invalid hostname is provided
 *           
 */
bool WiFi::startPing( IPAddress host ) {
    if( _init == false ) {
		return false;
	}

    if( this->startTask( TASK_WIFI_PING ) != TASK_WIFI_PING ) {
        return false;
    }

    if( this->_status != WIFI_STATUS_CONNECTED ) {
        this->endTask( ERR_WIFI_NOT_CONNECTED );
        return false;
    }

    _resolve = host;
    _rtt = 0;

    if( m2m_ping_req( (uint32_t)host, 128, &wifimanager_ping_cb ) < 0 ) {
        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Gets the result of the last resolve request.
 *
 * Arguments
 * ---------
 *  - result : IPADDRESS structure to write the result to.
 *
 * Returns : 0 if request is still running, < 0 if an error
 *           occured or round trip time is ping was successful.
 *           
 */
int32_t WiFi::getPingResult( IPAddress &dest ) {
    if( _init == false ) {
		return 0;
	}

    /* Request is still running */
    if( this->getCurrentTask() == TASK_WIFI_PING ) {
        return 0;
    }

    int32_t rtt = _rtt;
    dest = _resolve;

    _resolve = 0;
    _rtt = 0;
    
    return rtt;
}


/*--------------------------------------------------------------------------
 *
 * Handle WiFi module events and process running tasks.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 * 
 */
void WiFi::runTask() {

    if ( _init == false ) {
		return;
	}

    /* Handle WIFI module events */
    m2m_wifi_handle_events(NULL);

    /* Update root screen if connection status has changed */
    if( _status != _prev_status ) {
        g_screenUpdate = true;
    }
    _prev_status = _status;

    /* Process running tasks */
    switch( this->getCurrentTask() ) {


        /* Current task : connecting to WIFI network */
        case TASK_WIFI_CONNECT:
        {

            switch( _status ) {
                case WIFI_STATUS_DISCONNECTED:
                case WIFI_STATUS_CONNECT_FAILED:
                case WIFI_STATUS_NO_SSID_AVAIL:
                    this->endTask( _status );
                    break;

                case WIFI_STATUS_CONNECTED:
                    this->endTask( WIFI_STATUS_CONNECTED );
                    break;
            }
        }
        break;

        /* Current task : Reconnect to WiFi */
        case TASK_WIFI_RECONNECT:
        {

            if( _status == WIFI_STATUS_DISCONNECTED ) {
                this->endTask( TASK_SUCCESS );

                /* Got disconnected status, now try to reconnect */
                this->connect();
            }
        }
        break;

        /* Current task : DNS resolve */
        case TASK_WIFI_RESOLVE:
        {

            if( this->getTaskRunningTime() > WIFI_RESOLVE_TIMEOUT ) {

                this->endTask( ERR_TASK_TIMEOUT );
                _resolve = 0;
            }
        }
        break;

        /* Current task : Ping */
        case TASK_WIFI_PING_HOSTNAME:
        case TASK_WIFI_PING:
        {

            if( this->getTaskRunningTime() > WIFI_PING_TIMEOUT ) {

                this->endTask( ERR_TASK_TIMEOUT );
                _resolve = 0;
                _rtt = ERR_WIFI_PING_TIMEOUT;
            }
        }
        break;


        /* Current task : NONE */
        default:
        {

            /* Attempt to reconnect WIFI if connection was lost */
            if( _autoReconnect == true && _status != WIFI_STATUS_CONNECTED && ( millis() - _lastConnectAttempt > WIFI_RECONNECT_DELAY )) {

                /* Do not reconnect WIFI if clock is running on battery */
                if( g_power.getPowerMode() == POWER_MODE_NORMAL ) {
                    this->connect();
                }
            }
        }
        break;
  }
    
}


/*--------------------------------------------------------------------------
 *
 * Set the date/time on the wifi module
 *
 * Arguments
 * ---------
 *  - ntd : DateTime object containing the current UTC time 
 *
 * Returns : TRUE if successful, FALSE otherwise.
 * 
 */
bool WiFi::setSystemTime( DateTime *ndt ) {

    /* Disable SNTP and use system time instead */
    m2m_wifi_enable_sntp( 0 );
    
    uint32_t ts = ndt->getEpoch() + EPOCH_NTP_OFFSET;

    return ( m2m_wifi_set_sytem_time( ts ) == M2M_SUCCESS );
}


/*--------------------------------------------------------------------------
 *
 * WiFi notification callback
 *
 * Arguments
 * ---------
 *  - u8MsgType : Message type
 *  - pvMsg     : Pointer to message structure
 *
 * Returns : Nothing
 * 
 */
static void wifimanager_wifi_cb( uint8_t u8MsgType, void *pvMsg ) {

    g_wifi.handleEvent( u8MsgType, pvMsg );
}


/*--------------------------------------------------------------------------
 *
 * DNS resolution callback
 *
 * Arguments
 * ---------
 *  - hostName : Domain name of the host
 *  - hostIp   : IP address of the host
 *
 * Returns : Nothing
 * 
 */
static void wifimanager_resolve_cb( uint8 *hostName, uint32 hostIp ) 
{
	g_wifi.handleResolve( hostName, hostIp );
}


/*--------------------------------------------------------------------------
 *
 * Socket event callback
 *
 * Arguments
 * ---------
 *  - sock    : Socket ID
 *  - u8Msg   : Event type
 *  - pvMsg   : Pointer to message structure
 *
 * Returns : Nothing
 * 
 */
static void wifimanager_socket_cb( SOCKET sock, uint8 u8Msg, void *pvMsg ) {

    g_wifisocket.handleEvent( sock, u8Msg, pvMsg );   
}


/*--------------------------------------------------------------------------
 *
 * Ping result callback
 *
 * Arguments
 * ---------
 *  - ip      : Destination IP.
 *  - rtt     : Round trip time.
 *  - error   : Error code or PING_ERR_SUCCESS if successful.
 *
 * Returns : Nothing
 * 
 */
static void wifimanager_ping_cb( uint32 ip, uint32 rtt, uint8 error ) {

    g_wifi.handlePingResponse( ip, rtt, error );
}