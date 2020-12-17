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
#include "../libs/task.h"





#define TASK_WIFIMANAGER_CONNECT        1


#define ATTEMPT_RECONNECT_DELAY         10000


static void wifimanager_wifi_cb( uint8_t u8MsgType, void *pvMsg );
static void wifimanager_resolve_cb( uint8 * hostName, uint32 hostIp );
static void wifimanager_socket_cb( SOCKET sock, uint8 u8Msg, void *pvMsg );

class WiFiManager : public Task {
  public:
    
    WiFiManager( int8_t pin_cs, int8_t pin_irq, int8_t pin_rst, int8_t pin_en );

    void begin();
    void end();
    uint8_t connect();
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
    void handleEvent(uint8_t u8MsgType, void *pvMsg);
    void handleResolve(uint8_t * /*hostName*/, uint32_t hostIp);
    void onPowerStateChange( uint8_t state );

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
    bool _resolveRunning = false;
    wl_status_t _status;
    wl_status_t _prev_status;
    byte *_remoteMacAddress;

    unsigned long _lastConnectAttempt;
};

extern WiFiManager g_wifimanager;

#endif /* WiFiManager_H */