//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifi/wifisocket.cpp
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

#include "wifisocket.h"


extern uint8 hif_receive_blocked;
WiFiSocket g_wifisocket;


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 * 
 */
WiFiSocket::WiFiSocket() {

    for( int i = 0; i < MAX_SOCKET; i++ ) {
        _info[ i ].state = SOCKET_STATE_INVALID;
        _info[ i ].parent = -1;
        _info[ i ].recvMsg.s16BufferSize = 0;
        _info[ i ].buffer.data = NULL;
        _info[ i ].buffer.head = NULL;
        _info[ i ].buffer.length = 0;

        memset( &_info[ i ]._lastSendtoAddr, 0x00, sizeof( _info[ i ]._lastSendtoAddr ));
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Handle socket events
 *
 * @param   socket    Socket ID
 * @param   u8Msg     Event ID
 * @param   pvMsg     Pointer to additional event data
 * 
 */
void WiFiSocket::handleEvent( SOCKET sock, uint8 u8Msg, void *pvMsg ) {

    switch( u8Msg ) {

        /* Socket bind. */
        case SOCKET_MSG_BIND: {
            
            tstrSocketBindMsg *pstrBind = (tstrSocketBindMsg *)pvMsg;

            if( pstrBind && pstrBind->status == 0 ) {
                _info[ sock ].state = SOCKET_STATE_BOUND;
                

                if( sock < TCP_SOCK_MAX ) {
                    /* TCP */
                } else {
                    /* UDP */
                    recvfrom( sock, NULL, 0, 0 );
                }
                
            } else {
                _info[ sock ].state = SOCKET_STATE_IDLE;
            }
        }
        break;


        /* Socket listen. */
        case SOCKET_MSG_LISTEN: {
            tstrSocketListenMsg *pstrListen = (tstrSocketListenMsg *)pvMsg;

            if( pstrListen && pstrListen->status == 0 ) {
                _info[ sock ].state = SOCKET_STATE_LISTENING;
            } else {
                _info[ sock ].state = SOCKET_STATE_IDLE;
            }
        }
        break;


        /* Socket connected. */
        case SOCKET_MSG_CONNECT: {
            tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;

            if( pstrConnect && pstrConnect->s8Error >= 0 ) {
                _info[ sock ].state = SOCKET_STATE_CONNECTED;
                _info[ sock ].recvMsg.s16BufferSize = 0;

                recv( sock, NULL, 0, 0 );
            } else {
                _info[ sock ].state = SOCKET_STATE_IDLE;
                _info[ sock ].recvMsg.strRemoteAddr.sin_port = 0;
                _info[ sock ].recvMsg.strRemoteAddr.sin_addr.s_addr = 0;
            }
        }
        break;


        /* Socket accept. */
        case SOCKET_MSG_ACCEPT: {
            tstrSocketAcceptMsg *pstrAccept = (tstrSocketAcceptMsg*)pvMsg;

            if( pstrAccept && pstrAccept->sock > -1 ) {
                _info[ pstrAccept->sock ].state = SOCKET_STATE_ACCEPTED;
                _info[ pstrAccept->sock ].parent = sock;
                _info[ pstrAccept->sock ].recvMsg.strRemoteAddr = pstrAccept->strAddr;
            }
        }
        break;


        /* Socket data received. */
        case SOCKET_MSG_RECV:
        case SOCKET_MSG_RECVFROM: {

            tstrSocketRecvMsg *pstrRecvMsg = (tstrSocketRecvMsg *)pvMsg;

            if( pstrRecvMsg->s16BufferSize <= 0 ) {

                this->close( sock );

            } else if( _info[ sock ].state == SOCKET_STATE_CONNECTED || _info[ sock ].state == SOCKET_STATE_BOUND ) {

                _info[ sock ].recvMsg.pu8Buffer = pstrRecvMsg->pu8Buffer;
                _info[ sock ].recvMsg.s16BufferSize = pstrRecvMsg->s16BufferSize;
                if( sock < TCP_SOCK_MAX ) {
                    // TCP
                } else {
                    // UDP
                    _info[ sock ].recvMsg.strRemoteAddr = pstrRecvMsg->strRemoteAddr;
                }

                fillRecvBuffer( sock );

            } else {
                // not connected or bound, discard data
                hif_receive( 0, NULL, 0, 1 );
            }
        }
        break;

        /* Socket data sent. */
        case SOCKET_MSG_SEND: {
            sint16 *s16Sent = (sint16 *)pvMsg;

            /* Close socket if acknowledge is not received */
            if( s16Sent <= 0 ) {
                this->close( sock );
            }
        }
        break;

        default:
            break;

    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Create a socket
 * 
 * @param   u16Domain    Family. The only allowed value is AF_INET (IPv4.0) 
 *                       for TCP/UDP sockets.
 * @param   u8Type       Socket type :
 *                        - SOCK_STREAM
 *                        - SOCK_DGRAM
 * @param   u8Flags      Used to specify the socket creation flags. It shall be 
 *                       set to zero for normal TCP/UDP sockets. It could be  
 *                       SOCKET_FLAGS_SSL if the socket is used for SSL session.
 * 
 * @return  If successful, returns the socket ID or a negative number to 
 *          indicate an error.
 */
SOCKET WiFiSocket::create( uint16 u16Domain, uint8 u8Type, uint8 u8Flags ) {

    SOCKET sock = socket( u16Domain, u8Type, u8Flags );

    if( sock >= 0 ) {
        _info[ sock ].state = SOCKET_STATE_IDLE;
        _info[ sock ].parent = -1;
    }

    return sock;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Associates the provided address and local port to the socket
 * 
 * @details This functions returns immediately. Upon socket bind completion, 
 *          the application will receive a SOCKET_MSG_BIND message in the 
 *          socket callback. The function bound() can be used to determine
 *          if the socket is ready.
 * 
 * @param   sock         Socket ID
 * @param   pstrAddr     Pointer to socket address structure "sockaddr_in" 
 * @param   u8AddrLen    Size of the given socket address structure in bytes.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool WiFiSocket::requestBind( SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen ) {

    if( bind(sock, pstrAddr, u8AddrLen ) < 0 ) {
        return false;
    }

    _info[ sock ].state = SOCKET_STATE_BINDING;
    _info[ sock ].recvMsg.s16BufferSize = 0;

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check if the given socket is bound. 
 * 
 * @param   sock    Socket ID
 * 
 * @return  1 if connected, 0 otherwise
 * 
 */
uint8_t WiFiSocket::bound( SOCKET sock ) {

    if( sock < 0 ) {
        return 0;
    }

    m2m_wifi_handle_events( NULL );

    return ( _info[ sock ].state == SOCKET_STATE_BOUND );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   After successful socket binding to an IP address and port on the 
 *          system, start listening on a passive socket for incoming connections.
 * 
 * @details This functions returns immediately. Upon the call to the asynchronous 
 *          listen function, response is received through the event SOCKET_MSG_BIND
 *          in the socket callback. The function listening() can be used to determine
 *          if the socket is ready to accept connections.
 * 
 * @param   sock        Socket ID
 * @param   backlog    	Not used by the current implementation
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool WiFiSocket::requestListen( SOCKET sock, uint8 backlog ) {

    if( listen(sock, backlog ) < 0 ) {
        return false;
    }

    _info[ sock ].state = SOCKET_STATE_LISTEN;

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check if the given socket is listening. 
 * 
 * @param   sock    Socket ID
 * 
 * @return  1 if connected, 0 otherwise
 * 
 */
uint8_t WiFiSocket::listening( SOCKET sock ) {

    if( sock < 0 ) {
        return 0;
    }

    m2m_wifi_handle_events( NULL );

    return ( _info[ sock ].state == SOCKET_STATE_LISTENING );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Establishes a TCP connection with a remote server.
 * 
 * @details This functions returns immediately. The application socket callback 
 *          function is notified of a successful new socket connection through 
 *          the event SOCKET_MSG_CONNECT. The function connected() can be used 
 *          to determine the connection is established.
 * 
 * @param   sock        Socket ID
 * @param   pstrAddr    Pointer to socket address structure "sockaddr_in" 
 * @param   u8AddrLen   Size of the given socket address structure in bytes.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool WiFiSocket::requestConnect( SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen ) {

    if( connect( sock, pstrAddr, u8AddrLen ) < 0) {
        return false;
    }

    _info[ sock ].state = SOCKET_STATE_CONNECTING;
    _info[ sock ].recvMsg.s16BufferSize = 0;
    _info[ sock ].recvMsg.strRemoteAddr.sin_port = ((struct sockaddr_in*)pstrAddr)->sin_port;
    _info[ sock ].recvMsg.strRemoteAddr.sin_addr.s_addr = ((struct sockaddr_in*)pstrAddr)->sin_addr.s_addr;

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check if the given socket is connected to a remote server.
 * 
 * @param   sock    Socket ID
 * 
 * @return  1 if connected, 0 otherwise
 * 
 */
uint8_t WiFiSocket::connected( SOCKET sock ) {

    if( sock < 0 ) {
        return 0;
    }

    m2m_wifi_handle_events( NULL );

    return ( _info[ sock ].state == SOCKET_STATE_CONNECTED );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the socket option.
 * 
 * @param   socket          Socket ID
 * @param   u8Level         protocol level
 * @param   option_name     option to be set
 * @param   option_value    pointer to user provided value.
 * @param   u16OptionLen    length of the option value in bytes
 * 
 * @return  SOCK_ERR_NO_ERROR if successful or a negative number indicating
 * 	        an error
 * 
 */
sint8 WiFiSocket::setopt( SOCKET socket, uint8 u8Level, uint8 option_name, const void *option_value, uint16 u16OptionLen ) {
    
    return setsockopt( socket, u8Level, option_name, option_value, u16OptionLen );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the number of received bytes in the buffer
 * 
 * @param   sock    Socket ID
 * 
 * @return  Number of bytes available
 * 
 */
int WiFiSocket::available( SOCKET sock ) {

    m2m_wifi_handle_events( NULL );

    if( _info[ sock ].state != SOCKET_STATE_CONNECTED && _info[ sock ].state != SOCKET_STATE_BOUND ) {
        return 0;
    }

    return ( _info[ sock ].buffer.length + _info[ sock ].recvMsg.s16BufferSize );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads the next character in the buffer without discarding it.
 * 
 * @param   sock    Socket ID
 * 
 * @return  Character read or -1 if no character is available
 * 
 */
int WiFiSocket::peek( SOCKET sock ) {

    m2m_wifi_handle_events( NULL );

    if( _info[ sock ].state != SOCKET_STATE_CONNECTED && _info[ sock ].state != SOCKET_STATE_BOUND ) {
        return -1;
    }

    if( available( sock ) == 0) {
        return -1;
    }

    if( _info[ sock ].buffer.length == 0 && _info[ sock ].recvMsg.s16BufferSize ) {
        if( !fillRecvBuffer( sock )) {
            return -1;
        }
    }

    return *_info[ sock ].buffer.head;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads from the packet buffer and copy the data to
 *          the specified buffer
 * 
 * @param   sock    Socket ID
 * @param   buf     Pointer to a buffer to hold incoming packets
 * @param   size    Maximum size of the buffer
 * 
 * @return  The number of bytes read.
 * 
 */
int WiFiSocket::read( SOCKET sock, uint8_t* buf, size_t size ) {

    m2m_wifi_handle_events( NULL );

    if( _info[ sock ].state != SOCKET_STATE_CONNECTED && _info[ sock ].state != SOCKET_STATE_BOUND ) {
        return 0;
    }

    int avail = available( sock );

    if( avail <= 0 ) {
        return 0;
    }

    if( (int)size > avail ) {
        size = avail;
    }

    int bytesRead = 0;

    while( size ) {
        if( _info[sock].buffer.length == 0 && _info[ sock ].recvMsg.s16BufferSize ) {
            if( !fillRecvBuffer( sock ) ) {
                break;
            }
        }

        int toCopy = size;

        if( toCopy > _info[sock].buffer.length ) {
            toCopy = _info[sock].buffer.length;
        }

        memcpy( buf, _info[sock].buffer.head, toCopy );
        _info[ sock ].buffer.head += toCopy;
        _info[ sock ].buffer.length -= toCopy;

        buf += toCopy;
        size -= toCopy;
        bytesRead += toCopy;
    }

    if( _info[ sock ].buffer.length == 0 && _info[ sock ].recvMsg.s16BufferSize == 0 ) {

        if( sock < TCP_SOCK_MAX ) {
            // TCP
            recv( sock, NULL, 0, 0 );
        } else {
            // UDP
            recvfrom( sock, NULL, 0, 0 );
        }
        m2m_wifi_handle_events( NULL );
    }

    return bytesRead;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the server IP address the given socket is connected to.
 * 
 * @param   sock    Socket ID
 * 
 * @return  IPAddress structure containing the ip address.
 * 
 */
IPAddress WiFiSocket::remoteIP( SOCKET sock )
{
    return _info[ sock ].recvMsg.strRemoteAddr.sin_addr.s_addr;
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
uint16_t WiFiSocket::remotePort( SOCKET sock )
{
    return _info[ sock ].recvMsg.strRemoteAddr.sin_port;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Transmit the data contained in the given buffer to the connected
 *          remote host.
 * 
 * @param   sock    Socket ID
 * @param   buf     Pointer to a buffer holding data to be transmitted
 * @param   size    Length of the buffer to send
 * 
 * @return  Number of bytes sent if successful or negative number inticating 
 *          an error.
 * 
 */
size_t WiFiSocket::write( SOCKET sock, const uint8_t *buf, size_t size ) {

    m2m_wifi_handle_events( NULL );

    if( _info[ sock ].state != SOCKET_STATE_CONNECTED ) {
        return 0;
    }

    sint16 err;

    while(( err = send( sock, (void *)buf, size, 0 )) < 0 ) {

        /*  Exit on fatal error, retry if buffer not ready. */
        if( err != SOCK_ERR_BUFFER_FULL ) {
            size = 0;
            break;
        } else if( hif_receive_blocked ) {
            size = 0;
            break;
        }

        m2m_wifi_handle_events( NULL );
    }

    return size;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Transmit the data contained in the given buffer to the connected
 *          remote host.
 * 
 * @param   sock             Socket ID
 * @param   pvSendBuffer     Pointer to a buffer holding data to be transmitted
 * @param   u16SendLength    The buffer size in bytes
 * @param   flags            Not used in the current implementation
 * @param   pstrDestAddr     The destination address.
 * @param   u8AddrLen        Destination address length in bytes.
 * 
 * @return  SOCK_ERR_NO_ERROR if successful or negative number inticating an
 * 	        error
 * 
 */
sint16 WiFiSocket::sendto( SOCKET sock, void *pvSendBuffer, uint16 u16SendLength, uint16 flags, struct sockaddr *pstrDestAddr, uint8 u8AddrLen ) {

    m2m_wifi_handle_events( NULL );

    if( _info[ sock ].state != SOCKET_STATE_BOUND ) {
        return -1;
    }

    if( memcmp( &_info[ sock ]._lastSendtoAddr, pstrDestAddr, sizeof( _info[ sock ]._lastSendtoAddr )) != 0 ) {
        memcpy( &_info[ sock ]._lastSendtoAddr, pstrDestAddr, sizeof( _info[ sock ]._lastSendtoAddr ));

        return ::sendto( sock, pvSendBuffer, u16SendLength, flags, pstrDestAddr, u8AddrLen );
    } else {
        return ::send( sock, pvSendBuffer, u16SendLength, 0 );
    }	
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Close the socket
 * 
 * @param   sock    Socket ID
 * 
 * @return  SOCK_ERR_NO_ERROR if successful or negative number inticating an
 *          error
 * 
 */
sint8 WiFiSocket::close( SOCKET sock ) {

    m2m_wifi_handle_events( NULL );

    if( _info[ sock ].state == SOCKET_STATE_CONNECTED || _info[ sock ].state == SOCKET_STATE_BOUND ) {
        if( _info[ sock ].recvMsg.s16BufferSize > 0 ) {
             _info[ sock ].recvMsg.s16BufferSize = 0;

            /* flush any data not processed */
            hif_receive( 0, NULL, 0, 1 );
        }
    }

    _info[ sock ].state = SOCKET_STATE_INVALID;
    _info[ sock ].parent = -1;

    if( _info[ sock ] .buffer.data != NULL ) {
        free( _info[ sock ].buffer.data );
    }
    _info[ sock ].buffer.data = NULL;
    _info[ sock ].buffer.head = NULL;
    _info[ sock ].buffer.length = 0;
    _info[ sock ].recvMsg.s16BufferSize = 0;

    memset( &_info[ sock ]._lastSendtoAddr, 0x00, sizeof( _info[ sock ]._lastSendtoAddr ));

    return ::close( sock );	

}


/*! ------------------------------------------------------------------------
 *
 * @brief   Checks if a given socket belongs to the parent socket.
 * 
 * @param   sock     Parent socket ID
 * @param   child    Child socket ID
 * 
 * @return  1 if successful, 0 if an error occured
 * 
 */
int WiFiSocket::hasParent( SOCKET sock, SOCKET child ) {

    if( _info[ child ].parent != sock ) {
        return 0;
    }

    return 1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Find the next child socket that belong to the given parent 
 *          socket who have accepted a connection from a client.
 * 
 * @param   sock    Socket ID
 * 
 * @return  Available socket or -1 if no socket has accepted a connection.
 * 
 */
SOCKET WiFiSocket::accepted( SOCKET sock ) {

    m2m_wifi_handle_events( NULL );

    for( SOCKET s = 0; s < TCP_SOCK_MAX; s++ ) {
        if( _info[ s ].parent == sock && _info[ s ].state == SOCKET_STATE_ACCEPTED ) {
            _info[ s ].state = SOCKET_STATE_CONNECTED;

            _info[ s ].recvMsg.s16BufferSize = 0;
            recv( s, NULL, 0, 0 );

            return s;
        }
    }

    return -1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Fill the packet buffer with the received data.
 * 
 * @param   sock    Socket ID
 * 
 * @return  1 if successful, 0 if an error occured.
 * 
 */
int WiFiSocket::fillRecvBuffer( SOCKET sock )
{
    if( _info[ sock ].buffer.data == NULL) {
        _info[ sock ].buffer.data = (uint8_t*)malloc( SOCKET_BUFFER_SIZE );
        _info[ sock ].buffer.head = _info[sock].buffer.data;
        _info[ sock ].buffer.length = 0;
    }

    int size = _info[sock].recvMsg.s16BufferSize;

    if( size > SOCKET_BUFFER_SIZE) {
        size = SOCKET_BUFFER_SIZE;
    }

    uint8 lastTransfer = ((sint16)size == _info[ sock ].recvMsg.s16BufferSize);

    if( hif_receive( (uint32)_info[ sock ].recvMsg.pu8Buffer, _info[ sock ].buffer.data, (sint16)size, lastTransfer) != M2M_SUCCESS ) {
        return 0;
    }

    _info[ sock ].buffer.head = _info[ sock ].buffer.data;
    _info[ sock ].buffer.length = size;
    _info[ sock ].recvMsg.pu8Buffer += size;
    _info[ sock ].recvMsg.s16BufferSize -= size;

    return 1;
}