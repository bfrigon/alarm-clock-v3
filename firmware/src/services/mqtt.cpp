//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/mqtt.cpp
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

#include "mqtt.h"
#include "logger.h"
#include "drivers/power.h"



/*******************************************************************************
 *
 * @brief   Class initializer
 *
 */
MqttClient::MqttClient() {
    _init = false;
    _buffer = nullptr;
    _bufferPos = 0;
    _bufferSize = 0;
    _keepAlive = MQTT_DEFAULT_KEEP_ALIVE;
    _currentPacketID = 0;
    _connected = false;
    _firstConnectAttempt = true;
    _lastConnectAttempt = 0;
    _will_topic = nullptr;
    _will_payload = nullptr;
    _will_retain = false;
    _lastPub = false;
    
}


/*******************************************************************************
 *
 * @brief   Initialize the class and connect to the MQTT broker if client 
 *          is enabled in config.
 * 
 */
void MqttClient::begin() {

    _init = true;
    _keepAlive = MQTT_DEFAULT_KEEP_ALIVE;
    _currentPacketID = 0;
    _connected = false;
    _firstConnectAttempt = true;
    _lastConnectAttempt = 0;
    _will_topic = nullptr;
    _will_payload = nullptr;
    _will_retain = false;
    _lastPub = false;
    _clientEnabled = g_config.network.mqtt_enabled;

    this->resetRxState();

    if( _clientEnabled ) {

        if( g_wifi.connected() == true ) {
            this->connect();
        }
    }        
}


/*******************************************************************************
 *
 * @brief   Disconnect client and de-init class
 *
 */
void MqttClient::end() {

    this->disconnect( true );
    _init = false;
}


/*******************************************************************************
 *
 * @brief   Allocate memory for the RX/TX packet buffer
 * 
 * @param   size   Buffer size to allocate
 *
 * @return  Pointer to the buffer or nullptr if the allocation
 *          failed.
 */
void* MqttClient::allocBuffer( size_t size ) {

    if( _buffer != nullptr ) {
        this->freeBuffer();
    }

    if( size > MQTT_MAX_BUF_SIZE ) {
        return nullptr;
    }

    _buffer = malloc( size );

    if( _buffer != nullptr ) {
        _bufferSize = size;
    }

    return _buffer;
}


/*******************************************************************************
 *
 * @brief   Free memory allocated for the TX/RX buffer
 * 
 */
void MqttClient::freeBuffer() {

    if( _buffer == nullptr ) {
        return;
    }

    free( _buffer );
    _buffer = nullptr;
    _bufferPos = 0;
    _bufferSize = 0;
}


/*******************************************************************************
 *
 * @brief   Resets receive states and free memory allocated for the buffer
 * 
 */
void MqttClient::resetRxState() {

    this->freeBuffer();

    _rxState = MQTT_RX_STATE_IDLE;
    _rxPacketType = 0;
    _rxPacketFlags = 0;
}


#ifdef MQTT_DEBUG_PACKET
/*******************************************************************************
 *
 * @brief   Prints the content of the RX/TX buffer to the serial console
 * 
 */
void MqttClient::dumpBuffer() {

    if( _buffer == nullptr ) {
        return;
    }

    size_t i;
    for( i = 0; i < _bufferSize; i++ ) {
        g_console.printf( "%02X ", (( uint8_t* )_buffer )[ i ] );
    }
    g_console.println();
}
#endif

    
/*******************************************************************************
 * 
 * @brief   Write MQTT packet fixed header to the packet buffer.
 * 
 * @param   type                    Type of packet to send. 
 * @param   flags                   Packet flags to set
 * @param   variableHeaderLenght    Length of the remaining packet including
 *                                  variable header and payload.         
 *
 * @return  Pointer to the buffer or nullptr if the allocation
 *          failed.
 */
size_t MqttClient::writeFixedHeader( uint8_t type, uint8_t flags, size_t variableHeaderLenght ) {
    
    if( _buffer == nullptr ) {
        return 0;
    }

    size_t len = this->writeInt(( type << 4 ) | ( flags & 0xF ));

    do {
        uint8_t b = variableHeaderLenght % 128;
        variableHeaderLenght /= 128;

        if( variableHeaderLenght > 0 ) {
            b |= 0x80;
        }

        (( uint8_t* )_buffer )[ _bufferPos++ ] = b;
        len++;
    } while( variableHeaderLenght > 0 );

    return len;
}


/*******************************************************************************
 * 
 * @brief   Writes an 8-bit integer to the packet buffer
 * 
 * @param   value   8-bit value to write 
 * 
 * @return  Number of bytes written.
 */
size_t MqttClient::writeInt( uint8_t value ) {
    if( _buffer == nullptr ) {
        return 0;
    }

    (( uint8_t* )_buffer )[ _bufferPos++ ] = value;
    return 1;
}


/*******************************************************************************
 * 
 * @brief   Writes an 16-bit integer to the packet buffer
 * 
 * @param   value   16-bit value to write 
 * 
 * @return  Number of bytes written.
 */
size_t MqttClient::writeInt16( uint16_t value ) {
    if( _buffer == nullptr ) {
        return 0;
    }

    /* Write MSB first */
    (( uint8_t* )_buffer )[ _bufferPos++ ] = ( value >> 8 );
    (( uint8_t* )_buffer )[ _bufferPos++ ] = ( value & 0xFF );

    return 2;
}


/*******************************************************************************
 * 
 * @brief   Writes an UTF-8 encoded string to the packet buffer
 * 
 * @param   value           Pointer to the UTF-8 encoded string.
 * @param   encodeLength    Writes the length of the string in a 16 bit integer 
 *                          before the string.
 * 
 * @return  Number of bytes written.
 */
size_t MqttClient::writeString( char* value, bool encodeLength ) {
    if( _buffer == nullptr ) {
        return 0;
    }

    size_t len = strlen( value );

    if( encodeLength == true ) {
        this->writeInt16( uint16_t(len) );
    }

    memcpy( (char*)_buffer + _bufferPos, value, len );
    _bufferPos += len;

    return len + ( encodeLength == true ? 2 : 0 );
}


/*******************************************************************************
 * 
 * @brief   Reads an 8-bit integer from the packet buffer at the current
 *          position.
 * 
 * @param   valuePtr    Pointer to the variable to store the result in
 * 
 * @return  Number of bytes read.
 */
size_t MqttClient::readInt( uint8_t* valuePtr ) {

    if( _bufferPos + 1 > _bufferSize ) {
        return 0;
    }

    *valuePtr = (( uint8_t* )_buffer )[ _bufferPos++ ];

    return 1;
}


/*******************************************************************************
 * 
 * @brief   Reads an 16-bit integer from the packet buffer at the current
 *          position.
 * 
 * @param   valuePtr    Pointer to the variable to store the result in
 * 
 * @return  Number of bytes read.
 */
size_t MqttClient::readInt16( uint16_t* valuePtr ) {

    if( _bufferPos + 2 > _bufferSize ) {
        return 0;
    }

    *valuePtr = ((( uint8_t* )_buffer )[ _bufferPos++ ] << 8 );
    *valuePtr += (( uint8_t* )_buffer )[ _bufferPos++ ];

    return 2;
}


/*******************************************************************************
 *
 * @brief   Sets whether or not MQTT client is enabled
 * 
 * @param   enabled    TRUE to enable, FALSE otherwise
 * 
 */
void MqttClient::enableClient( bool enabled ) {

    if( _init == false ) {
        return;
    }

    if( _clientEnabled == enabled ) {
        return;
    }

    _clientEnabled = enabled;

    if( enabled ) {
        g_log.add( EVENT_MQTT_ENABLED );

        if( g_wifi.connected() == true && g_wifi.isBusy() == false ) {
            this->connect();
        }
        
    } else {
        g_log.add( EVENT_MQTT_DISABLED );

        if( _connected == true && g_wifi.connected() == true ) {
            this->disconnect();
        }
    }
}


/*******************************************************************************
 *
 * @brief   Sets the WILL message that the broker will send if the connection
 *          to this client is lost. 
 * 
 * @details This function must be called before connecting to the broker. 
 *          The pointers to the topic and payload must remain valid while
 *          establishing the connection to the broker.
 * 
 * @param   topic   Pointer to a character array containing the will topic.
 * @param   payload Pointer to a character array containing the will payload. 
 * 
 */
void MqttClient::setWillMessage( char* topic, char *payload, bool retain, bool publishBeforeDisconnect ) {

    if( _init == false ) {
        return;
    }

    _will_topic = topic;
    _will_payload = payload;
    _will_retain = retain;
    _will_pub_on_disconnect = publishBeforeDisconnect;
}


/*******************************************************************************
 *
 * @brief   Return whether or not the client is currently connected to
 *          the broker
 * 
 * @return  TRUE if connected or FALSE otherwise.
 * 
 */
bool MqttClient::connected() {

    if( _init == false ) {
        return false;
    }

    return _connected;
}


/*******************************************************************************
 *
 * @brief   Return whether or not the client is currently cenabled
 * 
 * @return  TRUE if enabled or FALSE otherwise.
 * 
 */
bool MqttClient::enabled() {

    if( _init == false ) {
        return false;
    }

    return _clientEnabled;
}


/*******************************************************************************
 *
 * @brief   Connects to the MQTT broker
 * 
 * @details This function returns immediately, connection status can be monitored
 *          by calling the @ref connected function.
 * 
 * @return  TRUE if succesful or FALSE otherwise.
 * 
 */
bool MqttClient::connect() {

    /* WiFi not connected */
    if( g_wifi.connected() == false ) {
        return false;
    }

    this->freeBuffer();
    this->resetRxState();

    /* Disconnect first if socket is already open */
    if( _tcp.connected() == 1 ) {
        _tcp.stop();
        _connected = false;
    }

    /* Reset the last connection attempt timer */
    _lastConnectAttempt = millis();
    _firstConnectAttempt = false;

    /* If configured mqtt broker address is an ip address, skip the hostname resolve */
    if( _broker_ip.fromString( g_config.network.mqtt_host ) == true ) {

        if( _tcp.connect( _broker_ip, g_config.network.mqtt_port ) == 0 ) {
            this->setTaskError( ERR_MQTTBROKER_CANT_CONNECT );
            return false;
        }

        this->startTask( TASK_MQTT_CONNECT_HOST, true );
        return true;

    } else {

        this->startTask( TASK_MQTT_RESOLVE_HOSTNAME, true );

        if( g_wifi.isBusy() == true ) {
            this->endTask( ERR_WIFI_BUSY );
            return false;
        }

        if( g_wifi.startHostnameResolve( g_config.network.mqtt_host ) == false ) {

            /* Hostname resolve failed */
            this->endTask( g_wifi.getTaskError() );
            return false;
        }
    }

    return true;
}


/*******************************************************************************
 *
 * @brief   Disconnect from the MQTT broker
 * 
 * @details This function returns immediately, connection status can be monitored
 *          by calling the @ref connected function.
 * 
 * @param   immediate   TRUE will drop the connection immediately, FALSE will 
 *                      send a disconnect packet first.
 * 
 * @return  TRUE if succesful or FALSE otherwise.
 * 
 */
void MqttClient::disconnect( bool immediate ) {

    /* If tcp connection was dropped unexpectedly, 
       reset the connection flags */
    if( _tcp.connected() == 0 || immediate == true ) {
        _connected = false;
        _lastPub = false;
        _firstConnectAttempt = true;
        _lastConnectAttempt = 0;

        this->freeBuffer();

        _tcp.stop();
        
        g_log.add( EVENT_MQTT_DISCONNECTED );

        if( this->getCurrentTask() != TASK_NONE ) {
            this->endTask( ERR_MQTTBROKER_DISCONNECTED );
        }

        return;
    }

    if( _connected == false ) {
        return;
    }

    /* If enabled, publish the WILL topic before disconnecting */
    if( _will_pub_on_disconnect == true && _will_topic != nullptr ) {

        _lastPub = true;

        if( this->publish( _will_topic, _will_payload, _will_retain ) == false ) {
            this->sendDisconnectPacket();    
        }

    } else {
        this->sendDisconnectPacket();
    }
    
}


/*******************************************************************************
 *
 * @brief   Sends the packet contained in the packet buffer to the broker.
 * 
 * @return  TRUE if succesful or FALSE otherwise.
 * 
 */
bool MqttClient::sendPacket() {

    if( _tcp.connected() == 0 ) {
        return false;
    }

    if( _buffer == nullptr ) {
        return false;
    }

    bool res;
    res = ( _tcp.write( ( char* )_buffer, _bufferPos ) == _bufferPos );

    _lastPacketSent = millis();

    #ifdef MQTT_DEBUG_PACKET
        g_console.println();
        g_console.printfln( "MQTT TX (type=%d,length=%d) :", ((( uint8_t* )_buffer )[ 0 ] & 0xF0 ) >> 4, _bufferPos );

        this->dumpBuffer();
    #endif

    this->freeBuffer();

    if( res == false ) {
        this->endTask( ERR_MQTTCLIENT_WRITE_FAIL );

        return false;
    }

    return res;
}


/*******************************************************************************
 *
 * @brief   Check for incomming packets from the broker
 * 
 */
void MqttClient::poll() {
    size_t byteRead;

    if( _tcp.connected() == 0 ) {
        return;
    }

    while( _tcp.available() > 0 && byteRead <= MQTT_MAX_READ_BLOCK_SIZE ) {

        int rxByte;
        rxByte = _tcp.read();
        byteRead++;

        if( rxByte < 0 ) {
            this->endTask( ERR_MQTTCLIENT_READ_FAIL );
            _rxState = MQTT_RX_STATE_IDLE;

            return;
        }


        switch( _rxState ) {

            /* Packet type & flags */
            case MQTT_RX_STATE_IDLE: {

                _rxPacketType = ( rxByte & 0xF0 ) >> 4;
                _rxPacketFlags = rxByte & 0x0F;
                _rxState = MQTT_RX_STATE_READ_HEADER;

                if( _rxPacketType == 0 ) {
                    this->endTask( ERR_MQTT_MALFORMED_PACKET );
                    _rxState = MQTT_RX_STATE_IDLE;
                }

                /* allocate buffer for the remaining length field (1-4 bytes)*/
                if( this->allocBuffer( 4 ) == nullptr ) {

                    this->endTask( ERR_MQTTCLIENT_CANT_ALLOCATE );
                    _rxState = MQTT_RX_STATE_IDLE;

                    return;
                }

                memset( _buffer, 0, 4 );
            }
            break;

            /* Remaining length */
            case MQTT_RX_STATE_READ_HEADER: {

                /* Check for continuity bit on the 4th byte of 
                the remaining length field. Malformed packet
                if set. */
                if(( rxByte & 0x80 ) != 0  && _bufferPos > 2 ) {

                    this->endTask( ERR_MQTT_MALFORMED_PACKET );
                    _rxState = MQTT_RX_STATE_IDLE;

                    return;
                }

                (( uint8_t *)_buffer )[ _bufferPos++ ] = ( uint8_t )rxByte;

                /* End of length field if continuity bit is not set */
                if(( rxByte & 0x80 ) == 0 )  {

                    size_t packetLength = 0;
                    uint8_t i;
                    for( i = 0; i < _bufferPos; i++ ) {

                        packetLength += ((( uint8_t* )_buffer )[ i ] & 0x7F ) * ( 1 << ( i * 7 ));
                    }

                    /* No remaining data, mark RX complete */
                    if( packetLength == 0 ) {
                        _rxState = MQTT_RX_STATE_COMPLETE;
                        _bufferPos = 0;

                        #ifdef MQTT_DEBUG_PACKET

                            g_console.println();
                            g_console.printfln( "MQTT RX (type=%d, flags=0x%02X, length=0)", 
                                                _rxPacketType, _rxPacketFlags );
                        #endif

                        return;
                    }


                    if( this->allocBuffer( packetLength ) == nullptr ) {

                        this->endTask( ERR_MQTTCLIENT_CANT_ALLOCATE );
                        _rxState = MQTT_RX_STATE_IDLE;
                        return;
                    }

                    

                    _rxState = MQTT_RX_STATE_READ_DATA;
                }
            }
            break;

            /* Variable header and payload */
            case MQTT_RX_STATE_READ_DATA: {

                (( uint8_t* )_buffer )[ _bufferPos++ ] = ( uint8_t )rxByte;

                if( _bufferPos == _bufferSize ) {
                    _rxState = MQTT_RX_STATE_COMPLETE;
                    _bufferPos = 0;

                    #ifdef MQTT_DEBUG_PACKET

                        g_console.println();
                        g_console.printfln( "MQTT RX (type=%d, flags=0x%02X, length=%d) :", 
                                            _rxPacketType, _rxPacketFlags, _bufferSize );
                        
                        this->dumpBuffer();
                    #endif
                }
            }
            break;
        }
    }
}


/*******************************************************************************
 *
 * @brief   Send a CONNECT request packet to the broker
 * 
 * @details This function returns immediately. The completion can be monitored
 *          by calling isBusy() and check the error with getTaskError()
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool MqttClient::sendConnectPacket() {

    if( _tcp.connected() == 0 ) {
        return false;
    }

    this->startTask( TASK_MQTT_SEND_CONNECT_PACKET, true );
    this->resetRxState();

    /* Append WiFi MAC address to the client ID */
    uint8_t mac[ 6 ];
    g_wifi.getMacAddress( mac );

    char client_id[ MQTT_MAX_CLIENT_ID_LENGTH + 1 ];
    snprintf_P( client_id, MQTT_MAX_CLIENT_ID_LENGTH,  S_MQTT_CLIENTID_FORMAT, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );

    /* determine remaining packet length */
    size_t remainingLength;
    remainingLength = 10;                           /* Protocol name field (6), protocol level (1), flags (1), keep alive (2) */
    remainingLength += strlen( client_id ) + 2;     /* Client ID field (len + 2) */

    if( _will_topic != nullptr && _will_payload != nullptr ) {

        /* Will topic and payload field (len + 4 )*/
        remainingLength += strlen( _will_topic ) + strlen( _will_payload ) + 4;
    }
    
    if( strlen( g_config.network.mqtt_username ) > 0 ) {

        /* Username field (len + 2) */
        remainingLength += strlen( g_config.network.mqtt_username ) + 2;
    }
    
    if( strlen( g_config.network.mqtt_password ) > 0 ) {

        /* Password field (len + 2) */
        remainingLength += strlen( g_config.network.mqtt_password ) + 2;
    }


    /* Allocate TX buffer */
    if( this->allocBuffer( MQTT_MAX_FIXED_HEADER_LENGTH + remainingLength ) == nullptr ) {
        this->endTask( ERR_MQTTCLIENT_CANT_ALLOCATE );

        return false;
    }
    
    /* Fixed header */
    this->writeFixedHeader( MQTT_PACKET_CONNECT, 0, remainingLength );

    /* Protocol name */
    char protocol[ 5 ] = "MQTT";
    this->writeString( protocol );

    /* Protocol level */
    this->writeInt( MQTT_PROTOCOL_LEVEL_4 );

    /* Connect flags */
    uint8_t flags;
    flags = MQTT_CONN_FLAGS_CLEAN_SESSION;

    if( strlen( g_config.network.mqtt_username ) > 0 ) {
        flags |= MQTT_CONN_FLAGS_USERNAME;
    }

    if( strlen( g_config.network.mqtt_password ) > 0 ) {
        flags |= MQTT_CONN_FLAGS_PASSWORD;
    }

    if( _will_topic != nullptr && _will_payload != nullptr ) {
        flags |= MQTT_CONN_FLAGS_WILL;
        flags |= ( _will_retain  == true ) ? MQTT_CONN_FLAGS_WILL_RETAIN : 0;
    }

    this->writeInt( flags );

    /* Keep alive */
    this->writeInt16( _keepAlive );

    /* Write client ID */
    this->writeString( client_id );

    if( _will_topic != nullptr && _will_payload != nullptr ) {
        this->writeString( _will_topic );
        this->writeString( _will_payload );
    }

    if( strlen( g_config.network.mqtt_username ) > 0 ) {
        this->writeString( g_config.network.mqtt_username );
    }

    if( strlen( g_config.network.mqtt_password ) > 0 ) {
        this->writeString( g_config.network.mqtt_password );
    }

    /* Send the packet */
    return this->sendPacket();
}


/*******************************************************************************
 *
 * @brief   Send a DISCONNECT request packet to the broker
 * 
 * @details This function returns immediately. The completion can be monitored
 *          by calling isBusy() and check the error with getTaskError()
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool MqttClient::sendDisconnectPacket() {

    if( _tcp.connected() == 0 ) {
        return false;
    }

    this->startTask( TASK_MQTT_DISCONNECT, true );
    this->resetRxState();
    
    /* Prepare the disconnect packet */
    this->allocBuffer( MQTT_MAX_FIXED_HEADER_LENGTH );
    this->writeFixedHeader( MQTT_PACKET_DISCONNECT, 0, 0 );

    /* Send the disconnect packet */
    return this->sendPacket();
}


/*******************************************************************************
 *
 * @brief   Send a PING packet to the broker
 * 
 * @details This function returns immediately. The completion can be monitored
 *          by calling isBusy() and check the error with getTaskError()
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool MqttClient::ping() {

    if( _init == false ) {
        return false;
    }


    if( this->getCurrentTask() != TASK_NONE ) {
        return false;
    }

    if( _tcp.connected() == 0 ) {
        return false;
    }

    this->startTask( TASK_MQTT_SEND_PING_PACKET );
    this->resetRxState();


    this->allocBuffer( MQTT_MAX_FIXED_HEADER_LENGTH );
    this->writeFixedHeader( MQTT_PACKET_PINGREQ, 0, 0 );

    /* Send the packet */
    return this->sendPacket();
}


/*******************************************************************************
 *
 * @brief   Send a PUBLISH packet to the broker
 * 
 * @details This function returns immediately. The completion can be monitored
 *          by calling isBusy() and check the error with getTaskError()
 * 
 * @param   topic   Pointer to a character array containing the topic
 * @param   payload Pointer to a character array containing the message payload
 * @param   retain  Sets the retain flag
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool MqttClient::publish( char* topic, char* payload, bool retain ) {

    if( _init == false ) {
        return false;
    }

    if( _tcp.connected() == 0 ) {
        return false;
    }

    this->startTask( TASK_MQTT_SEND_PUBLISH_PACKET, true );
    this->resetRxState();


    /* determine remaining packet length */
    size_t remainingLength;
    remainingLength = strlen( topic ) + 2;      /* Topic field length */
    remainingLength += 2;                       /* Packet ID length */

    if( payload != nullptr ) {
        remainingLength += strlen( payload );   /* Message length */
    }

    /* Allocate TX buffer */
    if( this->allocBuffer( MQTT_MAX_FIXED_HEADER_LENGTH + remainingLength ) == nullptr ) {
        this->endTask( ERR_MQTTCLIENT_CANT_ALLOCATE );

        return false;
    }

    uint8_t flags;
    flags = ( retain == true ? MQTT_PUB_FLAGS_RETAIN : 0 ) | MQTT_PUB_FLAGS_QOS_1;
    
    /* Add fixed header */
    this->writeFixedHeader( MQTT_PACKET_PUBLISH, flags, remainingLength );

    _currentPacketID++;
    if( _currentPacketID == 0 ) {
        _currentPacketID++;
    }

    this->writeString( topic );
    this->writeInt16( _currentPacketID ); 

    if( payload != nullptr ) {
        this->writeString( payload, false );
    }

    /* Send the packet */
    return this->sendPacket();
}


/*******************************************************************************
 *
 * @brief   Execute the current task
 * 
 */
void MqttClient::runTasks() {

    if( _init == false ) {
        return;
    }

    /* If tcp connection lost while connected to the broker, reset connection flags */
    if( _connected == true && _tcp.connected() == 0 ) {
        this->disconnect( true );
        return;
    }

    switch( this->getCurrentTask() ) {

        /* Resolving MQTT broker hostname */
        case TASK_MQTT_RESOLVE_HOSTNAME: {
            if( this->getTaskRunningTime() > WIFI_RESOLVE_TIMEOUT ) {

                this->endTask( ERR_MQTTBROKER_UNKNOWN_HOSTNAME );
                g_log.add( EVENT_MQTT_CANT_RESOLVE_HOST );

                return;
            }

            if( g_wifi.getHostnameResolveResults( _broker_ip ) == true ) {

                if( _broker_ip != 0 ) {

                    if( _tcp.connect( _broker_ip, g_config.network.mqtt_port ) == 0 ) {

                        this->endTask( ERR_MQTTBROKER_CANT_CONNECT );
                        g_log.add( EVENT_MQTT_SOCKET_ERROR );

                        return;
                    }

                    this->startTask( TASK_MQTT_CONNECT_HOST, true );
                    
                } else {

                    this->endTask( ERR_MQTTBROKER_UNKNOWN_HOSTNAME );
                    g_log.add( EVENT_MQTT_CANT_RESOLVE_HOST );

                    return;
                }
            }
        }
        break;

        /* Connecting to broker */
        case TASK_MQTT_CONNECT_HOST: {

            if( this->getTaskRunningTime() > MQTT_BROKER_CONNECT_TIMEOUT ) {

                this->endTask( ERR_MQTTBROKER_NO_RESPONSE );

                if( _firstConnectAttempt == true ) {
                    g_log.add( EVENT_MQTT_BROKER_NO_RESPONSE );
                }

                _tcp.stop();

                return;
            }

            if( _tcp.connected() == 1 ) {

                if( this->sendConnectPacket() == false ) {

                    this->endTask( ERR_MQTTBROKER_REFUSED_CONNECT );
                    g_log.add( EVENT_MQTT_CONNECT_REFUSED );
                    
                    _tcp.stop();
                    this->freeBuffer();

                    return;
                }
            }

        }
        break;

        /* Sending connect request packet */
        case TASK_MQTT_SEND_CONNECT_PACKET: {

            /* Check for incomming packet */
            this->poll();
            
            if( this->getTaskRunningTime() > MQTT_BROKER_CONNECT_TIMEOUT ) {

                this->endTask( ERR_MQTTBROKER_NO_RESPONSE );
                g_log.add( EVENT_MQTT_BROKER_NO_RESPONSE );

                _tcp.stop();
                this->freeBuffer();

                return;
            }


            /* Check if response has been received */
            if( _rxState != MQTT_RX_STATE_COMPLETE ) {
                return;
            }

            /* Expects a CONNACK packet type */
            if( _rxPacketType != MQTT_PACKET_CONNACK ) {

                this->endTask( ERR_MQTTBROKER_UNEXPECTED_RESPONSE );
                g_log.add( EVENT_MQTT_UNEXPECTED_RESPONSE );

                /* Must receive CONNACK after connect request, droping connection */
                _tcp.stop();
                this->freeBuffer();

                return;
            }

            uint8_t reason = (( uint8_t *)_buffer )[ 1 ];


            if( reason != MQTT_CONNACK_ACCEPTED ) {

                this->endTask( ERR_MQTTBROKER_REFUSED_CONNECT );
                g_log.add( EVENT_MQTT_CONNECT_REFUSED, reason );

                _tcp.stop();
                this->freeBuffer();
                
                return;    
            }

            g_log.add( EVENT_MQTT_CONNECTED );
            _connected = true;
            _lastPub = false;

            this->endTask( TASK_SUCCESS );
        }
        break;

        /* Sending ping request packet */
        case TASK_MQTT_SEND_PING_PACKET: {

            /* Check for incomming packet */
            this->poll();

            if( this->getTaskRunningTime() > MQTT_BROKER_PING_TIMEOUT ) {

                this->endTask( ERR_MQTTBROKER_NO_RESPONSE );
                g_log.add( EVENT_MQTT_BROKER_NO_RESPONSE );

                return;
            }

            if( _tcp.connected() == 0 ) {
                this->disconnect( true );
                return;
            }
            
            /* Check if response has been received */
            if( _rxState != MQTT_RX_STATE_COMPLETE ) {
                return;
            }

            /* Expects a PINGRESP packet type */
            if( _rxPacketType != MQTT_PACKET_PINGRESP ) {
                return;
            }

            /* Ping successful */
            this->endTask( TASK_SUCCESS );
        } 
        break;

        /* Sending publish packet */
        case TASK_MQTT_SEND_PUBLISH_PACKET: {

            /* Check for incomming packet */
            this->poll();

            if( this->getTaskRunningTime() > MQTT_BROKER_PUBLISH_TIMEOUT ) {

                this->endTask( ERR_MQTTBROKER_NO_RESPONSE );
                g_log.add( EVENT_MQTT_BROKER_NO_RESPONSE );

                return;
            }

            if( _tcp.connected() == 0 ) {
                this->disconnect( true );
                return;
            }
            
            /* Check if response has been received */
            if( _rxState != MQTT_RX_STATE_COMPLETE ) {
                return;
            }

            /* Expects a PUBACK packet type */
            if( _rxPacketType != MQTT_PACKET_PUBACK ) {
                return;
            }

            uint16_t packetID;
            this->readInt16( &packetID );
            
            if( _currentPacketID != packetID ) {
                return;
            }

            /* Send disconnect packet if it was the final publish 
               before disconnect request */
            if( _lastPub == true ) {

                this->sendDisconnectPacket();
                return;
            }

            this->endTask( TASK_SUCCESS );

        }
        break;

        /* Disconnecting from broker */
        case TASK_MQTT_DISCONNECT: {

            if( this->getTaskRunningTime() < MQTT_DISCONNECT_DELAY ) {
                return;
            }

            this->disconnect( true );
            return;
        }
        break;

        
        /* Current task : NONE */
        default:
        {

            if( _clientEnabled == false || g_wifi.connected() == false ) {
                return;
            }

            
            if( _tcp.connected() == 0 ) {

                /* Don't attempt to reconnect while wifi is busy */
                if( g_wifi.isBusy() == true ) {
                    return;
                }

                /* Attempt to reconnect if connection was lost */
                if(( millis() - _lastConnectAttempt > MQTT_RECONNECT_ATTEMPT_DELAY ) || _firstConnectAttempt == true ) {
                    
                    /* Do not reconnect if power mode is suspended */
                    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
                        return;
                    }

                    if( _firstConnectAttempt == false ) {
                        g_log.add( EVENT_MQTT_ATTEMPT_RECONNECT );
                    }

                    this->connect();
                    return;
                }
            
            } else {

                /* Send keep-alive packet if connected */    
                if (( millis() - _lastPacketSent ) / 1000 > _keepAlive ) {

                    this->ping();
                    return;
                }
            }
        }
        break;
    }
}