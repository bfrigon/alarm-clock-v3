//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi/wifi.h
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
#ifndef WIFI_H
#define WIFI_H


#include <Arduino.h>
#include <IPAddress.h>
#include <winc1500api.h>
#include <itask.h>
#include <time.h>
#include "wifisocket.h"

extern "C" {
  #include "driver/include/m2m_wifi.h"
  #include "bsp/include/nm_bsp.h"
  #include "bsp/include/nm_bsp_arduino.h"
  #include "driver/include/m2m_periph.h"
  #include "driver/include/m2m_ssl.h"
  #include "driver/include/m2m_wifi.h"
}


/* Class task's */
enum {
    TASK_WIFI_CONNECT = 1,
    TASK_WIFI_DISCONNECT_CLOSE_SOCKET,
    TASK_WIFI_DISCONNECT,
    TASK_WIFI_RESOLVE,
    TASK_WIFI_PING,
    TASK_WIFI_PING_HOSTNAME,
};

/* WiFi module status */
typedef enum {
    WIFI_STATUS_IDLE = 0,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_CONNECT_FAILED,
    WIFI_STATUS_CONNECT_TIMEOUT,
    WIFI_STATUS_CONNECTION_LOST,
    WIFI_STATUS_DISCONNECTING,
    WIFI_STATUS_DISCONNECTED,
} wl_status_t;

/* Delays (ms) */
#define WIFI_RECONNECT_ATTEMPT_DELAY    30000   
#define WIFI_CONNECT_TIMEOUT            10000
#define WIFI_RESOLVE_TIMEOUT            5000
#define WIFI_SOCKET_CLOSE_TIMEOUT       250
#define WIFI_PING_TIMEOUT               5000
#define WIFI_RSSI_REQ_DELAY             5000

void wifimanager_wifi_cb( uint8_t u8MsgType, void *pvMsg );
void wifimanager_resolve_cb( uint8 *hostName, uint32 hostIp );
void wifimanager_socket_cb( SOCKET sock, uint8 u8Msg, void *pvMsg );
void wifimanager_ping_cb( uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode );



/*******************************************************************************
 *
 * @brief   WINC1500 wifi driver
 * 
 *******************************************************************************/
class WiFi : public ITask {

  public:
    
    WiFi( int8_t pin_cs, int8_t pin_irq, int8_t pin_rst, int8_t pin_en );
    void begin();
    void end();
    wl_status_t connect();
    void disconnect();
    bool startHostnameResolve( const char *hostname );
    bool getHostnameResolveResults( IPAddress &results );
    uint32_t getLocalIP();
    uint32_t getGateway();
    uint32_t getSubmask();
    uint32_t getDNS();
    bool connected();
    wl_status_t status();
    void runTasks();
    void handleEvent( uint8_t u8MsgType, void *pvMsg );
    void handleResolve( uint8 *hostName, uint32_t hostIp );
    void handlePingResponse( uint32 ip, uint32 rtt, uint8 error );
    void onPowerStateChange( uint8_t state );
    void setAutoReconnect( bool autoReconnect, bool immediate = false );
    bool startPing( const char* hostname );
    bool startPing( IPAddress host );
    int32_t getPingResult( IPAddress &dest );
    bool setSystemTime( DateTime *ndt );
    bool getMacAddress( uint8_t* buffer );
    int8_t getRSSI();


  private:
    int init();

    bool _init;                           /* Class initialized */
    bool _dhcp;                           /* Automatically get the local IP address from DHCP */
    bool _autoReconnect;                  /* Attempt to reconnect when connection is lost */
    wl_status_t _status;                  /* Current connection status */
    uint32_t _localip;                    /* Local IP address */
    uint32_t _submask;                    /* Local IP address mask */
    uint32_t _gateway;                    /* Gateway IP address */
    uint32_t _dns;                        /* DNS server IP address */
    uint32_t _resolve;                    /* Last hostname resolve result */
    int32_t _rtt;                         /* Last ping round trip time */
    int8_t _rssi;                         /* Previously measured RSSI */
    unsigned long _lastRssiRequest;       /* Timestamp of the last RSSI measurement request */
    unsigned long _lastConnectAttempt;    /* Timestamp of the last connect attemps */
};


/* WiFi driver */
extern WiFi g_wifi;

#endif /* WIFI_H */