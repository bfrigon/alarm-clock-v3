//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi/udpclient.cpp
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

#include "udpclient.h"
#include "WiFi.h"
#include "wifisocket.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 * 
 */
UDPClient::UDPClient() {

    _socket = -1;
    _sndSize = 0;
    _parsedPacketSize = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts listening on specified port
 * 
 * @return  1 if successful, 0 if there are no socket available
 * 
 */
uint8_t UDPClient::begin( uint16_t port ) {

    struct sockaddr_in addr;
    uint32 u32EnableCallbacks = 0;

    _sndSize = 0;
    _parsedPacketSize = 0;

    // Initialize socket address structure.
    addr.sin_family = AF_INET;
    addr.sin_port = _htons(port);
    addr.sin_addr.s_addr = 0;

    if( _socket != -1 && g_wifisocket.bound( _socket )) {
        g_wifisocket.close( _socket );
        _socket = -1;
    }

    // Open UDP server socket.
    if(( _socket = g_wifisocket.create( AF_INET, SOCK_DGRAM, 0 )) < 0) {
        return 0;
    }

    g_wifisocket.setopt( _socket, SOL_SOCKET, SO_SET_UDP_SEND_CALLBACK, &u32EnableCallbacks, 0 );

    /* Bind socket: */
    if( g_wifisocket.requestBind( _socket, (struct sockaddr *)&addr, sizeof( struct sockaddr_in )) == false ) {

        g_wifisocket.close( _socket );
        _socket = -1;
        return 0;
    }

    return 1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts listening on specified multicast IP address and port
 * 
 * @return  1 if successful, 0 if there are no socket available
 * 
 */
uint8_t UDPClient::beginMulticast( IPAddress ip, uint16_t port ) {

    uint32_t multiIp = ip;

    if ( this->begin( port ) == false ) {
        return 0;
    }

    setsockopt( _socket, SOL_SOCKET, IP_ADD_MEMBERSHIP, &multiIp, sizeof(multiIp) );

    return 1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check if the socket is bound. 
 * 
 * @return  1 if bound, 0 otherwise
 * 
 */
uint8_t UDPClient::bound() {
    if( _socket < 0 ) {
        return false;
    }

    return g_wifisocket.bound( _socket );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the number of bytes (characters) available for reading from 
 *          the buffer.
 * 
 * @return  Number of bytes available
 * 
 */
int UDPClient::available() {

    if( _socket == -1 ) {
        return 0;
    }

    if( _parsedPacketSize <= 0 ) {
        return 0;
    }

    return g_wifisocket.available( _socket );
 }


/*! ------------------------------------------------------------------------
 *
 * @brief   Close the socket 
 * 
 */
 void UDPClient::stop() {

    if( _socket == -1 ) {
        return;
    }

    g_wifisocket.close( _socket );
    _socket = -1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief  	Start building up a packet to send to the remote host specific 
 *          in ip and port.
 * 
 * @param   ip      IP address of the remote host
 * @param   port    The port of the remote host 
 * 
 * @return  1 if successful, 0 if there was an error.
 */
int UDPClient::beginPacket( IPAddress ip, uint16_t port )
{
    _sndIP = ip;
    _sndPort = port;
    _sndSize = 0;

    return 1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Start building up a packet to send to the remote host specific 
 *          in host and port
 * 
 * @param   host    The address of the remote host.
 * @param   port    The port of the remote host. 
 * 
 * @return  1 if successful, 0 if there was an error
 * 
 */
int UDPClient::beginPacket( const char *host, uint16_t port ) {

    if( g_wifi.startHostnameResolve( host ) == false ) {
        return 0;
    }

    IPAddress ip;

    unsigned long start = millis();
    while( g_wifi.getHostnameResolveResults( ip ) == false && millis() - start < WIFI_PING_TIMEOUT ) {
        m2m_wifi_handle_events( NULL );
    }

    if( ip > 0 ) {
        return this->beginPacket( ip, port );
    }

    return 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Finish off this packet and send it
 * 
 * @return  1 if the packet was sent successfully, 0 if there was an error
 * 
 */
int UDPClient::endPacket() {

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = _htons(_sndPort);
    addr.sin_addr.s_addr = _sndIP;

    int result = g_wifisocket.sendto( _socket, (void *)_sndBuffer, _sndSize, 0, (struct sockaddr *)&addr, sizeof(addr) );

    return ( result < 0 ) ? 0 : 1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Write a single byte into the packet buffer
 * 
 * @param   byte    Byte to write
 * 
 * @return  Number of bytes written
 * 
 */
size_t UDPClient::write( uint8_t byte ) {

  return write( &byte, 1 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Copy from the given buffer into the packet buffer
 * 
 * @param   buffer    Pointer to the buffer containing the data to write
 * @param   size      Size of the buffer
 * 
 * @return  Number of bytes written
 * 
 */
size_t UDPClient::write( const uint8_t *buffer, size_t size )
{
    if(( size + _sndSize ) > sizeof( _sndBuffer )) {
        size = sizeof( _sndBuffer ) - _sndSize;
    }

    memcpy( _sndBuffer + _sndSize, buffer, size );

    _sndSize += size;

    return size;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts processing the next available incoming packet, checks for 
 *          the presence of a UDP packet, and reports the size.
 * 
 * @return  Size of the packet in bytes or 0 if no packet are available.
 * 
 */
int UDPClient::parsePacket() {

    if( _socket == -1 ) {
        return 0;
    }

    if( _parsedPacketSize > 0 ) {

        /* previously parsed data, discard data */
        while( available() ) {
            read();
        }
    }

    _parsedPacketSize = g_wifisocket.available( _socket );

    return _parsedPacketSize;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads the next character from the packet buffer.
 * 
 * @return  Character or -1 if an error occured.
 * 
 */
int UDPClient::read() {
    uint8_t b;

    if( read( &b, sizeof( b )) != 1 ) {
        return -1;
    }

    return b;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads from the packet buffer and copy the data to
 *          the specified buffer
 * 
 * @param   buf     Pointer to a buffer to hold incoming packets
 * @param   size    Maximum size of the buffer
 * 
 * @return  The number of bytes read.
 * 
 */
int UDPClient::read( unsigned char* buf, size_t size ) {

    // sizeof(size_t) is architecture dependent
    // but we need a 16 bit data type here
    uint16_t size_tmp = available();

    if( size_tmp == 0 ) {
        return -1;
    }

    if( size < size_tmp ) {
        size_tmp = size;
    }

    int result = g_wifisocket.read( _socket, buf, size );

    if( result > 0 ) {
        _parsedPacketSize -= result;
    }

    return result;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads the next character in the buffer without discarding it.
 * 
 * @return  Character read or -1 if no character is available
 * 
 */
int UDPClient::peek() {

    if( !available() ) {
        return -1;
    }

    return g_wifisocket.peek( _socket );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Unimplemented method of the Print class
 * 
 */
void UDPClient::flush() {}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the server IP address the given socket is connected to.
 * 
 * @param   sock    Socket ID
 * 
 * @return  IPAddress structure containing the ip address. 
 * 
 */
IPAddress UDPClient::remoteIP() {

    if( _socket == -1 ) {
        return IPAddress(0, 0, 0, 0);
    }

    return g_wifisocket.remoteIP( _socket );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the server port number the given socket is connected to.
 * 
 * @param   sock    Socket ID
 * 
 * @return  The port number
 * 
 */
uint16_t UDPClient::remotePort() {

    if( _socket == -1 ) {
        return 0;
    }

    return _htons( g_wifisocket.remotePort( _socket ));
}