//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/WiFiManager.h
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
#ifndef WiFiManager_H
#define WiFiManager_H

#include <Arduino.h>
#include <WiFi101.h>
#include "../libs/itask.h"


#define TASK_WIFIMANAGER_CONNECT        1
#define TASK_WIFIMANAGER_RECONNECT      2
#define TASK_WIFIMANAGER_RESOLVE        3
#define TASK_WIFIMANAGER_PING           4
#define TASK_WIFIMANAGER_PING_HOSTNAME  5

#define WIFI_RECONNECT_DELAY            10000   
#define WIFI_RESOLVE_TIMEOUT            5000
#define WIFI_PING_TIMEOUT               10000



static void wifimanager_wifi_cb( uint8_t u8MsgType, void *pvMsg );
static void wifimanager_resolve_cb( uint8 *hostName, uint32 hostIp );
static void wifimanager_socket_cb( SOCKET sock, uint8 u8Msg, void *pvMsg );
static void wifimanager_ping_cb( uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode );

class WiFiManager : public ITask {
  public:
    
    WiFiManager( int8_t pin_cs, int8_t pin_irq, int8_t pin_rst, int8_t pin_en );

    void begin();
    void end();
    wl_status_t reconnect();
    wl_status_t connect();
    void disconnect();
    bool startHostnameResolve( const char *hostname );
    bool getHostnameResolveResults( IPAddress &results );

    uint32_t getLocalIP();
    uint32_t getGateway();
    uint32_t getSubmask();
    uint32_t getDNS();
    bool isConnected();
    wl_status_t status();

    void runTask();
    void handleEvent( uint8_t u8MsgType, void *pvMsg );
    void handleResolve( uint8 *hostName, uint32_t hostIp );
    void handlePingResponse( uint32 ip, uint32 rtt, uint8 error );
    void onPowerStateChange( uint8_t state );

    bool startPing( const char* hostname );
    bool startPing( IPAddress host );
    int32_t getPingResult( IPAddress &dest );

  private:
    int init();
    void getConnectionInfo();

    bool _init = false;
    bool _dhcp = true;

    uint32_t _localip;
	  uint32_t _submask;
	  uint32_t _gateway;
    uint32_t _dns;
	  uint32_t _resolve;
    int32_t _rtt;
    wl_status_t _status;
    wl_status_t _prev_status;
    

    unsigned long _lastConnectAttempt;
    unsigned long _timerTaskStart;
};

extern WiFiManager g_wifimanager;

#endif /* WiFiManager_H */