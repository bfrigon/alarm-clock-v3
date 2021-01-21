//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/telnet_console.h
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
#ifndef TELNET_CONSOLE_H
#define TELNET_CONSOLE_H

#include <Arduino.h>
#include "../console/console_base.h"
#include "../drivers/wifi/wifisocket.h"
#include "../drivers/wifi/tcpclient.h"
#include "../libs/itask.h"


#define TELNET_PORT                 23
#define TELNET_SEND_BUFFER_SIZE     64

#define TELNET_SESSION_TIMEOUT      300


/* Server states */
enum {
    TELNET_STATE_WAIT_WIFI_CONNECTION,
    TELNET_STATE_SOCKET_REQ_BIND,
    TELNET_STATE_SOCKET_REQ_LISTEN,
    TELNET_STATE_SERVER_LISTENING,
    TELNET_STATE_CLIENT_NEGOTIATING,
    TELNET_STATE_CLIENT_CONNECTED
};


/* Telnet commands */
#define TELNET_CMD_ECHO         1
#define TELNET_CMD_SUPRESS_GA   3
#define TELNET_CMD_STATUS       5
#define TELNET_CMD_TERM_TYPE    24
#define TELNET_CMD_LINEMODE     34


/* Telnet operation types */
#define TELNET_OP_WILL          251
#define TELNET_OP_WONT          252
#define TELNET_OP_DO            253
#define TELNET_OP_DONT          254
#define TELNET_IAC              255



class TelnetConsole : public ConsoleBase {
  public:

    TelnetConsole();

    void runTasks();

    bool startServer();
    void stopServer();
    bool clientConnected();

    void enableServer( bool enabled );
    void printConsoleStatus( ConsoleBase *console );


  private:
    SOCKET _socket;
    TCPClient _client;
    uint8_t _state;
    unsigned long _lastActivity;
    bool _serverEnabled;

    char _sendBuffer[ TELNET_SEND_BUFFER_SIZE + 1 ];
    size_t _sendBufSize;


    size_t _print( char c );
    int _read();
    int _peek();
    int _available();
    void flushSendBuffer();

    void exitConsole( bool timeout );
    void resetConsole();
    bool requestListen();
    bool checkForClients();
    bool handleNegotiation();
    void queueTelnetCommand( uint8_t op, uint8_t cmd );
    void processIncommingTelnetCommands( uint8_t op, uint8_t cmd );

};

extern TelnetConsole g_telnetConsole;


#endif /* TELNET_CONSOLE */