//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi/tcpclient.cpp
// Author  : Benoit Frigon <www.bfrigon.com>
//
// Credit  : This file contains large portions of code from the WiFi101
//           arduino library. It has been adapted to make the calls to the
//           WiFi module non-blocking. Original code at: 
//           https://github.com/arduino-libraries/WiFi101
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

#include "tcpclient.h"
#include "wifi.h"
#include "wifisocket.h"



/*******************************************************************************
 *
 * @brief   Default class constructor
 * 
 */
TCPClient::TCPClient() {
    _socket = -1;
}


/*******************************************************************************
 *
 * @brief   Create a class instance and assign a specific socket ID.
 * 
 * @param   sock    Socket ID to assign.
 * 
 */
TCPClient::TCPClient( SOCKET sock ) {
    
    _socket = sock;
}


/*******************************************************************************
 *
 * @brief   Establish a secure connection to the given host name and port.
 * 
 * @param   host    Hostname to connect to
 * @param   port    Port number to connect to 
 * 
 * @return  1 if successful, 0 otherwise.
 * 
 */
int TCPClient::connectSSL( const char* host, uint16_t port ) {

    return connect( host, port, SOCKET_FLAGS_SSL );
}


/*******************************************************************************
 *
 * @brief   Establish a secure connection to the given host IP address 
 *          and port.
 * 
 * @param   ip      IP address of the remote host.
 * @param   port    Port number to connect to.
 * 
 * @return  1 if successful, 0 otherwise.
 * 
 */
int TCPClient::connectSSL( IPAddress ip, uint16_t port ) {
    return connect( ip, port, SOCKET_FLAGS_SSL, 0 );
}


/*******************************************************************************
 *
 * @brief   Establish a connection to the given host name and port.
 * 
 * @param   host    Hostname to connect to.
 * @param   port    Port number to connect to .
 * 
 * @return  1 if successful, 0 otherwise.
 * 
 */
int TCPClient::connect( const char* host, uint16_t port ) {
    return connect( host, port, 0 );
}


/*******************************************************************************
 *
 * @brief   Establish a connection to the given host IP address and port.
 * 
 * @param   ip      IP address of the remote host.
 * @param   port    Port number to connect to.
 * 
 * @return  1 if successful, 0 otherwise.
 * 
 */
int TCPClient::connect( IPAddress ip, uint16_t port ) {
    return connect( ip, port, 0, 0 );	
}


/*******************************************************************************
 *
 * @brief   Establish a connection to the given host name and port and set
 *          socket options.
 * 
 * @param   host    Hostname to connect to.
 * @param   port    Port number to connect to.
 * @param   opt     Socket options.
 * 
 * @return  1 if successful, 0 otherwise.
 * 
 */
int TCPClient::connect( const char* host, uint16_t port, uint8_t opt ) {

    if( g_wifi.startHostnameResolve( host ) == false ) {
        return 0;
    }

    IPAddress remote_addr;

    unsigned long start = millis();
    while( g_wifi.getHostnameResolveResults( remote_addr ) == false && millis() - start < WIFI_PING_TIMEOUT ) {

        m2m_wifi_handle_events( NULL );
    }

    return connect( remote_addr, port, opt, (const uint8_t *)host );
}


/*******************************************************************************
 *
 * @brief   Establish a connection to the given host name and port and set
 *          socket options.
 * 
 * @param   ip          IP address of the remote host.
 * @param   port        Port number to connect to.
 * @param   opt         Socket options.
 * @param   hostname    Server name indicator, used by SSL socket.
 * 
 * @return  1 if successful, 0 otherwise.
 * 
 */
int TCPClient::connect( IPAddress ip, uint16_t port, uint8_t opt, const uint8_t *hostname ) {

    struct sockaddr_in addr;

    /* Initialize socket address structure */
    addr.sin_family = AF_INET;
    addr.sin_port = _htons(port);
    addr.sin_addr.s_addr = ip;

    if( this->connected() ) {
        this->stop();
    }

    /* Create TCP socket */
    if(( _socket = g_wifisocket.create( AF_INET, SOCK_STREAM, opt )) < 0 ) {
        return 0;
    }

    if( opt & SOCKET_FLAGS_SSL && hostname ) {
        g_wifisocket.setopt( _socket, SOL_SSL_SOCKET, SO_SSL_SNI, hostname, m2m_strlen( (uint8_t *)hostname ));
    }

    /* Connect to remote host */
    if( g_wifisocket.requestConnect( _socket, (struct sockaddr *)&addr, sizeof( struct sockaddr_in )) == false ) {

        g_wifisocket.close( _socket );
        _socket = -1;
        return 0;
    }

    return 1;
}


/*******************************************************************************
 *
 * @brief   Write a single byte into the packet buffer.
 * 
 * @param   byte    Byte to write.
 * 
 * @return  Number of bytes written.
 * 
 */
size_t TCPClient::write( uint8_t byte ) {
    return this->write( &byte, 1 );
}


/*******************************************************************************
 *
 * @brief   Copy from the given buffer into the packet buffer.
 * 
 * @param   buffer    Pointer to the buffer containing the data to write.
 * @param   size      Size of the buffer.
 * 
 * @return  Number of bytes written.
 * 
 */
size_t TCPClient::write( const uint8_t *buffer, size_t size ) {

    if( _socket < 0 || size == 0 || this->connected() == 0 ) {
        this->setWriteError();
        return 0;
    }

    int result = g_wifisocket.write( _socket, buffer, size );

    if( result <= 0 ) {
        
        this->setWriteError();
        return 0;
    }

    return size;
}


/*******************************************************************************
 *
 * @brief   Get the number of bytes (characters) available for reading from 
 *          the buffer.
 * 
 * @return  Number of bytes available.
 * 
 */
int TCPClient::available() {

    if( _socket == -1 ) {
        return 0;
    }

    return g_wifisocket.available( _socket );
}


/*******************************************************************************
 *
 * @brief   Reads the next character from the packet buffer.
 * 
 * @return  Character or -1 if an error occured.
 * 
 */
int TCPClient::read() {

    uint8_t b;

    if( read( &b, sizeof( b )) != 1 ) {
        return -1;
    }

    return b;
}


/*******************************************************************************
 *
 * @brief   Reads from the packet buffer and copy the data to
 *          the specified buffer.
 * 
 * @param   buf     Pointer to a buffer to hold incoming packets.
 * @param   size    Maximum size of the buffer.
 * 
 * @return  The number of bytes read.
 * 
 */
int TCPClient::read( uint8_t* buf, size_t size ) {

    /* sizeof(size_t) is architecture dependent
       but we need a 16 bit data type here. */
    uint16_t size_tmp = available();
    
    if( size_tmp == 0 ) {
        return -1;
    }
    
    if( size < size_tmp ) {
        size_tmp = size;
    }

    int result = g_wifisocket.read( _socket, buf, size );

    return result;
}


/*******************************************************************************
 *
 * @brief   Reads the next character in the buffer without discarding it.
 * 
 * @return  Character read or -1 if no character is available.
 * 
 */
int TCPClient::peek() {

    if( !this->available() ) {
        return -1;
    }

    return g_wifisocket.peek( _socket );
}


/*******************************************************************************
 *
 * @brief   Unimplemented method of the Print class.
 * 
 */
void TCPClient::flush() {}


/*******************************************************************************
 *
 * @brief   Closes the socket. 
 * 
 */
void TCPClient::stop() {
    
    if( _socket < 0 ) {
        return;
    }

    g_wifisocket.close( _socket );

    _socket = -1;
}


/*******************************************************************************
 *
 * @brief   Check if the socket is connected. 
 * 
 * @return  1 if connected, 0 otherwise.
 * 
 */
uint8_t TCPClient::connected() {
    if( _socket < 0 ) {
        return 0;
    }

    return g_wifisocket.connected( _socket );
}


/*******************************************************************************
 *
 * @brief   Returns whether or not the class has a valid socket ID assigned
 *          to it.
 * 
 * @return  TRUE if the socket ID is valid or FALSE otherwise.
 * 
 */
TCPClient::operator bool()
{
    return _socket != -1;
}


/*******************************************************************************
 *
 * @brief   Compare the socket ID of this instance with another class instance.
 * 
 * @param   other    Pointer to the other class instance to test.
 * 
 * @return  TRUE if the socket ID is the same that the other 
 *          class or FALSE otherwise.
 * 
 */
bool TCPClient::operator==( const TCPClient &other ) const {

    return ( _socket == other._socket );
}


/*******************************************************************************
 *
 * @brief   Compare the socket ID of this instance with another class instance.
 * 
 * @param   other    Pointer to the other class instance to test.
 * 
 * @return  TRUE if the socket ID is NOT the same that the other 
 *          class or FALSE otherwise.
 * 
 */
bool TCPClient::operator!=( const TCPClient &other ) const {

    return ( _socket != other._socket );
}


/*******************************************************************************
 *
 * @brief   Get the server IP address the given socket is connected to.
 * 
 * @param   sock    Socket ID
 * 
 * @return  IPAddress structure containing the ip address. 
 * 
 */
IPAddress TCPClient::remoteIP() {

    if( _socket == -1 ) {
        return IPAddress( 0, 0, 0, 0 );
    }

    return g_wifisocket.remoteIP( _socket );
}


/*******************************************************************************
 *
 * @brief   Get the server port number the given socket is connected to.
 * 
 * @param   sock    Socket ID
 * 
 * @return  The port number.
 * 
 */
uint16_t TCPClient::remotePort() {

    if( _socket == -1 ) {
        return 0;
    }

    return _htons( g_wifisocket.remotePort( _socket ) );
}