//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi/udpclient.h
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
#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <Arduino.h>
#include <Udp.h>
#include <winc1500api.h>

extern "C" {
  #include "socket/include/socket.h"
}

#define SOCKET_BUFFER_UDP_SIZE      (128u)


class UDPClient : public UDP {
  public:

    UDPClient();
    
    virtual uint8_t begin( uint16_t );
    virtual uint8_t beginMulticast( IPAddress, uint16_t );
    uint8_t bound();
    
    virtual void stop();

    virtual int beginPacket( IPAddress ip, uint16_t port );
    virtual int beginPacket( const char *host, uint16_t port );
    virtual int endPacket();
    virtual size_t write( uint8_t);
    virtual size_t write( const uint8_t *buffer, size_t size );

    using Print::write;

    virtual int parsePacket();
    virtual int available();
    virtual int read();
    virtual int read( unsigned char* buffer, size_t len );
    virtual int read( char* buffer, size_t len ) { return read((unsigned char*)buffer, len); };
    virtual int peek();
    virtual void flush();

    virtual IPAddress remoteIP();
    virtual uint16_t remotePort();


  private:

    SOCKET _socket;
    int _parsedPacketSize;
    uint8_t _sndBuffer[SOCKET_BUFFER_UDP_SIZE];
    uint16_t _sndSize;
    uint16_t _sndPort;
    uint32_t _sndIP;
};

#endif /* UDPCLIENT_H */