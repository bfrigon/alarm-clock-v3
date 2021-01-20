//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi/tcpclient.h
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
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <Arduino.h>
#include <Client.h>
#include <IPAddress.h>

extern "C" {
    #include "socket/include/socket.h"
}

class TCPClient : public Client {

public:
    TCPClient();
    TCPClient( SOCKET sock );

    int connectSSL( IPAddress ip, uint16_t port );
    int connectSSL( const char* host, uint16_t port );
    virtual int connect( IPAddress ip, uint16_t port );
    virtual int connect( const char* host, uint16_t port );
    virtual size_t write( uint8_t );
    virtual size_t write( const uint8_t *buf, size_t size );
    virtual int available();
    virtual int read();
    virtual int read( uint8_t *buf, size_t size );
    virtual int peek();
    virtual void flush();
    virtual void stop();
    virtual uint8_t connected();
    virtual operator bool();
    bool operator==( const TCPClient &other ) const;
    bool operator!=( const TCPClient &other ) const;

    using Print::write;

    virtual IPAddress remoteIP();
    virtual uint16_t remotePort();

private:
    SOCKET _socket;

    int connect(const char* host, uint16_t port, uint8_t opt);
    int connect(IPAddress ip, uint16_t port, uint8_t opt, const uint8_t *hostname);
};



#endif /* TCPCLIENT_H */