//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/WiFiManager.cpp
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
#include "WiFiManager.h"
#include "../hardware.h"
#include "../config.h"
#include "../ui/ui.h"
#include "utility/WiFiSocket.h"

extern "C" {
    #include "bsp/include/nm_bsp_arduino.h"
    #include "driver/include/m2m_ssl.h"
}


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
WiFiManager::WiFiManager( int8_t pin_cs, int8_t pin_irq, int8_t pin_rst, int8_t pin_en ) {

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
void WiFiManager::begin() {
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
void WiFiManager::end() {
    if( _init == false ) {
		return;
	}

    _status = WL_IDLE_STATUS;
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
int WiFiManager::init() {
    
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
	_status = WL_IDLE_STATUS;
	_localip = 0;
	_submask = 0;
	_gateway = 0;
	_dhcp = true;
	_resolve = 0;
	_remoteMacAddress = 0;

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
 * Connect to the WiFi network set in config.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
uint8_t WiFiManager::connect() {

    tstrM2MIPConfig conf;

	if( _init == false ) {
		init();
	}
    
    /* Disconnnect previously established connection */
    if( _status == WL_CONNECTED ) {
        this->disconnect();
    }

    /* Starts a task to monitor connection progress */
    this->startTask( TASK_WIFIMANAGER_CONNECT );

    /* Reset the last connection attempt timer */
    _lastConnectAttempt = millis();

    if( g_config.settings.net_dhcp == false ) {

        _dhcp = false;
        m2m_wifi_enable_dhcp( 0 );    // disable DHCP

        conf.u32DNS = IPAddress( &g_config.settings.net_dns[0] );
        conf.u32Gateway = IPAddress( &g_config.settings.net_gateway[0] );
        conf.u32StaticIP = IPAddress( &g_config.settings.net_ip[0] );
        conf.u32SubnetMask = IPAddress( &g_config.settings.net_mask[0] );
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

    
    const char *ssid = g_config.settings.ssid;
    const void *pvAuthInfo = g_config.settings.wkey;

    /* Start connection to the WiFi network */
    if( m2m_wifi_connect( (char*)ssid, strlen(ssid), M2M_WIFI_SEC_WPA_PSK, (void*)pvAuthInfo, M2M_WIFI_CH_ALL) < 0 ) {
		_status = WL_CONNECT_FAILED;

        this->endTask( _status );
		return _status;
	}

    /* Set the hostname only if DHCP is used */
    if( _dhcp == true ) {
        m2m_wifi_set_device_name( (uint8 *)g_config.settings.hostname, strlen(g_config.settings.hostname ));
    }

    /* Status remains idle until DHCP response has been received or a connection 
       is established if the address is static */
    _status = WL_IDLE_STATUS;

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
void WiFiManager::disconnect() {

    if( _init == false ) {
		return;
	}

    /* Close sockets to clean state */
	for( int i = 0; i < MAX_SOCKET; i++ ) {
		WiFiSocket.close( i );
	}

	m2m_wifi_disconnect();

    if( this->getCurrentTask() != TASK_NONE ) {
        this->endTask( WL_DISCONNECTED );
    }
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
uint32_t WiFiManager::getLocalIP() 
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
uint32_t WiFiManager::getGateway()
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
uint32_t WiFiManager::getSubmask()
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
uint32_t WiFiManager::getDNS()
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
 * Returns : Local IP address 
 */
void WiFiManager::handleEvent(uint8_t u8MsgType, void *pvMsg)
{
    switch( u8MsgType ) {
		case M2M_WIFI_RESP_CON_STATE_CHANGED: 
        {

            tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
			if( pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED ) {

                if( _dhcp == false ) {
                    _status = WL_CONNECTED;
                }


            } else {

                _status = WL_DISCONNECTED;

                if ( _dhcp == false ) {
                    _localip = 0;
                    _gateway = 0;
                    _submask = 0;
                    _dns = 0;
                }
                
                for (int i = 0; i < MAX_SOCKET; i++) {
                    WiFiSocket.close(i);
                }
            }
        }
        break;

        case M2M_WIFI_REQ_DHCP_CONF: 
        {
            tstrM2MIPConfig *pstrIPCfg = (tstrM2MIPConfig *)pvMsg;
            _localip = pstrIPCfg->u32StaticIP;
            _submask = pstrIPCfg->u32SubnetMask;
            _gateway = pstrIPCfg->u32Gateway;
            _dns = pstrIPCfg->u32DNS;
            
            _status = WL_CONNECTED;

            
        }
        break;

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
void WiFiManager::handleResolve( uint8_t * /*hostName*/, uint32_t hostIp )
{
    _resolve = hostIp;  
    _resolveRunning = false; 
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
void WiFiManager::onPowerStateChange( uint8_t state ) {

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
bool WiFiManager::isConnected() {
    return ( _status == WL_CONNECTED );
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
wl_status_t WiFiManager::status() {
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
bool WiFiManager::startHostnameResolve( const char *hostname ) {
    if( _init == false ) {
		return false;
	}

    if( _resolveRunning == true ) {
        return false;
    }

    _resolve = 0;
    _resolveRunning = true;

    if( gethostbyname( (uint8 *)hostname ) < 0 ) {
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
bool WiFiManager::getHostnameResolveResults( IPAddress &result ) {
    if( _init == false ) {
		return false;
	}

    if( _resolveRunning == true ) {
        return false;
    }

    result = _resolve;
    
    _resolveRunning = false;
    _resolve = 0;

    return true;
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
 */
void WiFiManager::runTask() {

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
        case TASK_WIFIMANAGER_CONNECT:
        {

            switch( _status ) {
                case WL_DISCONNECTED:
                case WL_CONNECT_FAILED:
                case WL_NO_SSID_AVAIL:
                    this->endTask( _status );
                    break;

                case WL_CONNECTED:
                    this->endTask( WL_CONNECTED );
                    break;
            }
        }
        break;

        /* Current task : NONE */
        default:
        {

            /* Attempt to reconnect WIFI if connection was lost */
            if( _status != WL_CONNECTED && ( millis() - _lastConnectAttempt > ATTEMPT_RECONNECT_DELAY )) {

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
 * WiFi notification callback
 *
 * Arguments
 * ---------
 *  - u8MsgType : Message type
 *  - pvMsg     : Pointer to message structure
 *
 * Returns : Nothing
 */
static void wifimanager_wifi_cb( uint8_t u8MsgType, void *pvMsg )
{
    g_wifimanager.handleEvent( u8MsgType, pvMsg );
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
 */
static void wifimanager_resolve_cb( uint8 * hostName, uint32 hostIp )
{
	g_wifimanager.handleResolve( hostName, hostIp );
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
 */
static void wifimanager_socket_cb( SOCKET sock, uint8 u8Msg, void *pvMsg )
{
    WiFiSocket.eventCallback( sock, u8Msg, pvMsg );   
}