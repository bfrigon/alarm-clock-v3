//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi/wifisocket.h
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
#ifndef WIFISOCKET_H
#define WIFISOCKET_H


#include <Arduino.h>
#include <IPAddress.h>
#include <winc1500api.h>

extern "C" {
    #include "socket/include/socket.h"
    #include "socket/include/m2m_socket_host_if.h"
    #include "driver/include/m2m_wifi.h"
    #include "driver/source/m2m_hif.h"
    #include "driver/include/m2m_periph.h"
}

#define SOCKET_BUFFER_SIZE    128


struct SocketInfo {
    uint8_t state;
    SOCKET parent;
    tstrSocketRecvMsg recvMsg;
    struct {
        uint8_t* data;
        uint8_t* head;
        int length;
    } buffer;
    struct sockaddr _lastSendtoAddr;
};


/* Socket states */
enum {
  SOCKET_STATE_INVALID,
  SOCKET_STATE_IDLE,
  SOCKET_STATE_CONNECTING,
  SOCKET_STATE_CONNECTED,
  SOCKET_STATE_BINDING,
  SOCKET_STATE_BOUND,
  SOCKET_STATE_LISTEN,
  SOCKET_STATE_LISTENING,
  SOCKET_STATE_ACCEPTED
};


//**************************************************************************
//
// WiFi socket
//
//**************************************************************************
class WiFiSocket {
  public:
    WiFiSocket();

    SOCKET create( uint16 u16Domain, uint8 u8Type, uint8 u8Flags );
    bool requestBind( SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen );
    uint8_t bound( SOCKET sock );
    bool requestListen( SOCKET sock, uint8 backlog );
    uint8_t listening( SOCKET sock );
    bool requestConnect( SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen );
    uint8_t connected( SOCKET sock );
        
    sint8 setopt( SOCKET socket, uint8 u8Level, uint8 option_name, const void *option_value, uint16 u16OptionLen );
        
    int available( SOCKET sock );
    int peek( SOCKET sock );
    int read( SOCKET sock, uint8_t* buf, size_t size );
    
    size_t write( SOCKET sock, const uint8_t *buf, size_t size );
    sint16 sendto( SOCKET sock, void *pvSendBuffer, uint16 u16SendLength, uint16 flags, struct sockaddr *pstrDestAddr, uint8 u8AddrLen );
    IPAddress remoteIP( SOCKET sock );
    uint16_t remotePort( SOCKET sock );
    sint8 close( SOCKET sock );
    SOCKET accepted( SOCKET sock );
    int hasParent( SOCKET sock, SOCKET child );

    void handleEvent( SOCKET sock, uint8 u8Msg, void *pvMsg );


  private:
    int fillRecvBuffer(SOCKET sock);

    SocketInfo _info[ MAX_SOCKET ];
};


extern WiFiSocket g_wifisocket;



#endif /* WIFISOCKET_H */