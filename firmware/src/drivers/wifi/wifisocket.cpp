//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/wifisocket.cpp
// Author  : Benoit Frigon <www.bfrigon.com>
//
// Credit  : This file contains large portions of code from the WiFi101
//           arduino library. It has been adapted to make the calls to the
//           WiFi module non-blocking. Original code at: 
//           https://github.com/arduino-libraries/WiFi101/blob/master/src/WiFi.cpp
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
			// sint16 *s16Sent = (sint16 *)pvMsg;
		}
		break;

		default:
			break;

	}
}


SOCKET WiFiSocket::create( uint16 u16Domain, uint8 u8Type, uint8 u8Flags ) {

	SOCKET sock = socket( u16Domain, u8Type, u8Flags );

	if( sock >= 0 ) {
		_info[ sock ].state = SOCKET_STATE_IDLE;
		_info[ sock ].parent = -1;
	}

	return sock;
}


bool WiFiSocket::requestBind( SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen ) {

	if( bind(sock, pstrAddr, u8AddrLen ) < 0 ) {
		return false;
	}

	_info[ sock ].state = SOCKET_STATE_BINDING;
	_info[ sock ].recvMsg.s16BufferSize = 0;

	return true;
}

bool WiFiSocket::bound( SOCKET sock ) {

	m2m_wifi_handle_events( NULL );

	return ( _info[ sock ].state == SOCKET_STATE_BOUND );
}

bool WiFiSocket::requestListen( SOCKET sock, uint8 backlog ) {

	if( listen(sock, backlog ) < 0 ) {
		return false;
	}

	_info[ sock ].state = SOCKET_STATE_LISTEN;

	return true;
}

bool WiFiSocket::listening( SOCKET sock ) {

	m2m_wifi_handle_events( NULL );

	return ( _info[ sock ].state == SOCKET_STATE_LISTENING );
}

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

bool WiFiSocket::connected( SOCKET sock ) {

	m2m_wifi_handle_events( NULL );

	return ( _info[ sock ].state == SOCKET_STATE_CONNECTED );
}

sint8 WiFiSocket::setopt( SOCKET socket, uint8 u8Level, uint8 option_name, const void *option_value, uint16 u16OptionLen ) {
	
	return setsockopt( socket, u8Level, option_name, option_value, u16OptionLen );
}

int WiFiSocket::available( SOCKET sock ) {

	m2m_wifi_handle_events( NULL );

	if( _info[ sock ].state != SOCKET_STATE_CONNECTED && _info[ sock ].state != SOCKET_STATE_BOUND ) {
		return 0;
	}

	return ( _info[ sock ].buffer.length + _info[ sock ].recvMsg.s16BufferSize );
}


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
		if( _info[sock].buffer.length == 0 && _info[sock].recvMsg.s16BufferSize ) {
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


IPAddress WiFiSocket::remoteIP( SOCKET sock )
{
	return _info[ sock ].recvMsg.strRemoteAddr.sin_addr.s_addr;
}

uint16_t WiFiSocket::remotePort( SOCKET sock )
{
	return _info[ sock ].recvMsg.strRemoteAddr.sin_port;
}

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

int WiFiSocket::hasParent( SOCKET sock, SOCKET child ) {

	if( _info[child].parent != sock ) {
		return 0;
	}

	return 1;
}

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



WiFiSocket g_wifisocket;