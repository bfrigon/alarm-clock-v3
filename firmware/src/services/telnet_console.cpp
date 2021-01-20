//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/telnet_console.cpp
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
#include "telnet_console.h"

#include "../drivers/wifi/wifi.h"
#include "../drivers/wifi/wifisocket.h"
#include "../task_errors.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 */
TelnetConsole::TelnetConsole() {
    _sendBufSize = 0;
    memset( _sendBuffer, 0, sizeof( _sendBuffer ) );
    
    _state = TELNET_STATE_WAIT_WIFI_CONNECTION;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   IPrint interface callback for printing a single character. 
 *          write the character to the send buffer and send the packet once
 *          the buffer is full or and EOL character is written.
 * 
 * @param   c   character to print
 *
 * @return  Number of bytes written
 */
size_t TelnetConsole::_print( char c ) {
    _sendBuffer[ _sendBufSize++ ] = c;

    if( c == '\n' ) {
        this->flushSendBuffer();
    }

    if( _sendBufSize >= TELNET_SEND_BUFFER_SIZE ) {
        this->flushSendBuffer();
    }

    return 1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Send the content of the transmit buffer. 
 * 
 */
void TelnetConsole::flushSendBuffer() {
    if( _sendBufSize == 0 ) {
        return;
    }

    _client.write( _sendBuffer, _sendBufSize );

    _sendBufSize = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads the next character in the receive buffer.
 * 
 * @return  Character or -1 if an error occured.
 * 
 */
int TelnetConsole::_read() {

    /* Reset the session inactivity timer */
    _lastActivity = millis();

    return _client.read();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads the next character in the receive buffer without 
 *          discarding it.
 * 
 * @return  Character read or -1 if no character is available
 * 
 */
int TelnetConsole::_peek() {
    return _client.peek();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the number of bytes (characters) available for reading from 
 *          the buffer.
 * 
 * @return  Number of bytes available
 * 
 */
int TelnetConsole::_available() {
    return _client.available();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Write telnet command sequence to the transmit buffer.
 * 
 * @param   op     Operation type (WILL, WONT, DO, DONT)
 * @param   cmd    Command
 * 
 */
void TelnetConsole::queueTelnetCommand( uint8_t op, uint8_t cmd ) {

    /* Check if there is enough space for the command sequence, if
       not, flush the buffer */
    if( _sendBufSize + 3 >= TELNET_SEND_BUFFER_SIZE ) {
        this->flushSendBuffer();
    }

    _sendBuffer[ _sendBufSize++ ] = TELNET_IAC;
    _sendBuffer[ _sendBufSize++ ] = op;
    _sendBuffer[ _sendBufSize++ ] = cmd;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Return whether or not if a client is currently connected
 * 
 * @return  TRUE if connected or FALSE otherwise.
 * 
 */
bool TelnetConsole::clientConnected() {
    return _client.connected();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Create the server socket 
 * 
 * @return  TRUE if successful or FALSE otherwise.
 * 
 */
bool TelnetConsole::startServer() {
    if( _state != TELNET_STATE_WAIT_WIFI_CONNECTION ) {
        return false;
    }

    if( g_wifi.connected() == false ) {
        return false;
    }

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = _htons( TELNET_PORT );
    addr.sin_addr.s_addr = 0;

    if( _socket != -1 && g_wifisocket.listening( _socket )) {

        g_wifisocket.close( _socket );
    }

    if(( _socket = g_wifisocket.create( AF_INET, SOCK_STREAM, 0 )) < 0 ) {

        //this->endTask( ERR_TELNET_CANT_CREATE_SOCKET );
        return false;
    }

    _state = TELNET_STATE_SOCKET_REQ_BIND;

    if( g_wifisocket.requestBind( _socket, (struct sockaddr *)&addr, sizeof( struct sockaddr_in )) == false ) {
        this->stopServer();

        //this->endTask( ERR_TELNET_CANNOT_BIND );
        return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Disconnect the client and stop accepting connections.
 * 
 * @return  TRUE if successful or FALSE otherwise.
 * 
 */
void TelnetConsole::stopServer() {

    /* Close the client socket */
    _client.stop();

    /* Close the listener socket */
    if( _socket != -1 ) {

        g_wifisocket.close( _socket );
        _socket = -1;
    }

    _state = TELNET_STATE_WAIT_WIFI_CONNECTION;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check for accepted socket connection. Refuses the connection if
 *          a session is already active.
 * 
 * @return  TRUE if a client is available or FALSE otherwise.
 * 
 */
bool TelnetConsole::checkForClients() {

    if( _socket < 0 ) {
        //this->endTask( ERR_TELNET_SOCKET_CLOSED );
        return false;
    }

    SOCKET child;
    child = g_wifisocket.accepted( _socket );

    /* No client available yet */
    if( child < 0 ) {
        return false;
    }

    /* Client already connected, refuse subsequent connections */
    if( _client.connected() ) {

        char buffer[ sizeof( S_CONSOLE_CONNECT_REFUSE ) ];
        strcpy_P( buffer, S_CONSOLE_CONNECT_REFUSE );

        g_wifisocket.write( child, (uint8_t*)buffer, sizeof( buffer ));
        
        g_wifisocket.close( child );
        return false;
    }

    _state = TELNET_STATE_CLIENT_NEGOTIATING;
    _client = child;
    _sendBufSize = 0;
    _lastActivity = millis();
    
    this->queueTelnetCommand( TELNET_OP_WILL, TELNET_CMD_ECHO );
    this->flushSendBuffer();

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Handle telnet negotiation exchange
 * 
 * @return  TRUE if negotiation is done or FALSE otherwise
 * 
 */
bool TelnetConsole::handleNegotiation() {

    if( millis() > _lastActivity + 250 ) {
        /* Negotiation timeout */
        return true;
    }

    if( _client.peek() != TELNET_IAC ) {
        return false;
    }

    if( _client.available() < 3 ) {
        return false;
    }

    _client.read();
    uint8_t op = _client.read();
    uint8_t cmd = _client.read();

    if( op == TELNET_OP_DO && cmd == TELNET_CMD_SUPRESS_GA ) {
        this->queueTelnetCommand( TELNET_OP_WILL, TELNET_CMD_SUPRESS_GA );
    }

    if( op == TELNET_OP_DO && cmd == TELNET_CMD_STATUS ) {
        this->queueTelnetCommand( TELNET_OP_WONT, TELNET_CMD_STATUS );
    }

    if( op == TELNET_OP_WILL && cmd == TELNET_CMD_LINEMODE ) {
        this->queueTelnetCommand( TELNET_OP_WONT, TELNET_CMD_LINEMODE );
    }

    
    this->flushSendBuffer();

    _lastActivity = millis();

    return false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Terminate the console session
 *
 * @param   timeout    TRUE when the session has timed out
 * 
 */
void TelnetConsole::exitConsole( bool timeout = false ) {

    if( _state != TELNET_STATE_CLIENT_CONNECTED ) {
        return;
    }

    /* Stop currently running console commands */
    this->endTask();

    this->println();
    this->println_P( (timeout == true ) ? S_CONSOLE_TIMEOUT : S_CONSOLE_GOODBYE );
    this->println();

    this->flushSendBuffer();

    /* close telnet connection */
    _client.stop();

    _state = TELNET_STATE_SERVER_LISTENING;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Clear the screen and display the welcome message
 * 
 */
void TelnetConsole::resetConsole() {

    /* Stop currently running console commands */
    this->endTask();

    this->println();
    this->println_P( S_CONSOLE_WELCOME );
    this->println();
    this->println();

    this->resetInput();
    
    this->displayPrompt();
    this->flushSendBuffer();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run server tasks
 * 
 */
void TelnetConsole::runTasks() {

    /* Stop server if WiFi connection is lost */
    if( g_wifi.connected() == false && _state != TELNET_STATE_WAIT_WIFI_CONNECTION ) {
        
        this->stopServer();
        return;
    }  


    switch( _state ) {

        /* Waiting for WiFi to be connected before starting the server */
        case TELNET_STATE_WAIT_WIFI_CONNECTION:

            if( g_wifi.connected() == true ) {
                this->startServer();
            }

            break;


        /* Wait for a confirmation that the server socket is bound */
        case TELNET_STATE_SOCKET_REQ_BIND:

            if( _socket < 0 || g_wifisocket.bound( _socket ) == 0 ) {
                return;
            }

            _state = TELNET_STATE_SOCKET_REQ_LISTEN;

            if( g_wifisocket.requestListen( _socket, 0 ) == false ) {
                this->stopServer();

                return;
            }
        
            break;


        /* Wait for a confirmation that the server socket is listening */
        case TELNET_STATE_SOCKET_REQ_LISTEN:

            if( g_wifisocket.listening( _socket )) {

                _state = TELNET_STATE_SERVER_LISTENING;
            }
            break;


        /* Wait for clients to connect */
        case TELNET_STATE_SERVER_LISTENING:

            this->checkForClients();
            break;


        /* Wait for telnet negotiation to complete */
        case TELNET_STATE_CLIENT_NEGOTIATING:
        
            if( this->handleNegotiation() == true ) {
                _state = TELNET_STATE_CLIENT_CONNECTED;

                /* Display the welcome message and prompt */
                this->resetConsole();
            }
            break;


        /* Serve connected client */
        case TELNET_STATE_CLIENT_CONNECTED:

            /* Disconnect client if session timeout timer elapse */
            if( _client.connected() && millis() > _lastActivity + ( TELNET_SESSION_TIMEOUT * 1000UL )) {

                this->exitConsole( true );
                return;
            }
                
            if( _client.connected() == 0 ) {
                _client.stop();

                /* Client disconnected, go back to listening */
                _state = TELNET_STATE_SERVER_LISTENING;
                return;
            }


            /* Check if other clients are trying to connect, 
            if so, refuse connection */
            this->checkForClients();

            /* Process user input and run commands */
            ConsoleBase::runTasks();

            this->flushSendBuffer();
            break;
    }
}