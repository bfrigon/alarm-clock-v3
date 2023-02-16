//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/mqtt.h
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
#ifndef MQTT_H
#define MQTT_H


#include <Arduino.h>
#include <itask.h>
#include <drivers/wifi/wifi.h>
#include <task_errors.h>
#include <config.h>
#include <drivers/wifi/tcpclient.h>
#include "console.h"



/* MQTT protocol version */
#define MQTT_PROTOCOL_LEVEL_4           4       /* MQTT 3.1.1 */

/* Limits */
#define MQTT_MAX_CLIENT_ID_LENGTH       23
#define MQTT_MAX_BUF_SIZE               1024
#define MQTT_MAX_READ_BLOCK_SIZE        64
#define MQTT_MAX_FIXED_HEADER_LENGTH    5

/* Delays (ms) */
#define MQTT_BROKER_PUBLISH_TIMEOUT     5000
#define MQTT_BROKER_SUBSCRIBE_TIMEOUT   5000
#define MQTT_BROKER_PING_TIMEOUT        5000
#define MQTT_BROKER_CONNECT_TIMEOUT     5000
#define MQTT_RECONNECT_ATTEMPT_DELAY    15000
#define MQTT_DISCONNECT_DELAY           250
#define MQTT_DEFAULT_KEEP_ALIVE         60

/* Connect flags */
#define MQTT_CONN_FLAGS_USERNAME        0x80
#define MQTT_CONN_FLAGS_PASSWORD        0x40
#define MQTT_CONN_FLAGS_WILL_RETAIN     0x20
#define MQTT_CONN_FLAGS_WILL            0x04
#define MQTT_CONN_FLAGS_CLEAN_SESSION   0x02

/* Publish flags */
#define MQTT_PUB_FLAGS_RETAIN           0x01
#define MQTT_PUB_FLAGS_QOS_0            0x00
#define MQTT_PUB_FLAGS_QOS_1            0x02
#define MQTT_PUB_FLAGS_QOS_2            0x04
#define MQTT_PUB_FLAGS_DUP              0x08

/* Subscribe flags */
#define MQTT_SUB_FLAGS                  0x02

/* Quality-of-service levels */
#define MQTT_QOS_0                      0
#define MQTT_QOS_1                      1
#define MQTT_QOS_2                      2



PROG_STR( S_MQTT_CLIENTID_FORMAT, "clock-v3-%02X%02X%02X%02X%02X%02X" );



/* Packet types */
enum MqttPacketTypes {
    MQTT_PACKET_CONNECT = 1,
    MQTT_PACKET_CONNACK,
    MQTT_PACKET_PUBLISH,
    MQTT_PACKET_PUBACK,
    MQTT_PACKET_PUBREC,
    MQTT_PACKET_PUBREL,
    MQTT_PACKET_PUBCOMP,
    MQTT_PACKET_SUBSCRIBE,
    MQTT_PACKET_SUBACK,
    MQTT_PACKET_UNSUBSCRIBE,
    MQTT_PACKET_UNSUBACK,
    MQTT_PACKET_PINGREQ,
    MQTT_PACKET_PINGRESP,
    MQTT_PACKET_DISCONNECT,
    MQTT_PACKET_AUTH,
};

/* Task ID's */
enum MqttTaskIds {
    TASK_MQTT_RESOLVE_HOSTNAME = 1,
    TASK_MQTT_CONNECT_HOST,
    TASK_MQTT_SEND_CONNECT_PACKET,
    TASK_MQTT_SEND_PING_PACKET,
    TASK_MQTT_SEND_PUBLISH_PACKET,
    TASK_MQTT_SEND_SUBSCRIBE_PACKET,
    TASK_MQTT_DISCONNECT,
};

/* Client packet transmision state */
enum MqttReceiveStates {
    MQTT_RX_STATE_IDLE = 0,
    MQTT_RX_STATE_READ_HEADER,
    MQTT_RX_STATE_READ_DATA,
    MQTT_RX_STATE_COMPLETE,
};

/* CONNACK response codes */
enum MqttConnAckResponses {
    MQTT_CONNACK_ACCEPTED = 0,
    MQTT_CONNACK_PROTOCOL_REFUSED,
    MQTT_CONNACK_IDENT_REJECTED,
    MQTT_CONNACK_SERVER_UNAVAILABLE,
    MQTT_CONNACK_CRED_MALFORMED,
    MQTT_CONNACK_NOT_AUTHORIZED,
};



typedef void (*mqttPubRxFunc)( char* topic, size_t topicLength, char* payload, size_t payloadLength, bool retain );



/*******************************************************************************
 *
 * @brief   MQTT client class
 * 
 *******************************************************************************/
class MqttClient : public ITask {
  public:

    MqttClient();
    void begin();
    void end();
    void enableClient( bool enabled );
    bool connected();
    bool enabled();
    bool ping();
    bool publish( char* topic, char* payload, bool retain = false );
    bool subscribe( char* topic );
    void setWillMessage( char* topic, char *payload, bool retain = false, bool publishBeforeDisconnect = false );
    void runTasks();
    void setPublishReceiveCallback( mqttPubRxFunc func );

  private:
    bool connect();
    void disconnect( bool immediate = false );
    bool sendPacket();
    bool sendConnectPacket();
    bool sendDisconnectPacket();
    bool sendPublishAck( uint16_t packetID );
    void* allocBuffer( size_t size );
    void freeBuffer();
    void poll();
    void resetRxState();
    size_t writeFixedHeader( uint8_t type, uint8_t flags, size_t variableHeaderLenght );
    size_t writeInt( uint8_t value );
    size_t writeInt16( uint16_t value );
    size_t writeString( char *value, bool encodeLength = true );
    size_t readInt16( uint16_t* valuePtr );
    size_t readInt( uint8_t* valuePtr );
    bool checkForPublishAck();
    bool checkForConnectAck();
    bool checkForSubscribeAck();
    bool checkForPingResponse();
    uint16_t getNewPacketID();
    void onReceivePublishMessage();

    TCPClient _tcp;                         /* TCP connection instance */
    IPAddress _broker_ip;                   /* MQTT broker IP address */
    bool _clientEnabled;                    /* Client enabled */
    bool _connected;                        /* Connection established */
    bool _lastPub;                          /* Mark the next PUBLISH as the last before disconnect */
    bool _init;                             /* Class initialized */
    uint8_t _rxState;                       /* Receive buffer current state */
    uint8_t _rxPacketType;                  /* Last received packet type */
    uint8_t _rxPacketFlags;                 /* Last received packet flags */
    unsigned long _lastPacketSent;          /* Timestamp of the last packet sent */
    uint16_t _currentPacketID;              /* Last received packet ID */
    void* _buffer;                          /* RX/TX buffer */
    size_t _bufferPos;                      /* RX/TX buffer current position */
    size_t _bufferSize;                     /* RX?TX buffer allocated size */
    uint16_t _keepAlive;                    /* Keep alive timeout (seconds) */
    unsigned long _lastConnectAttempt;      /* Timestamp of the last connection attemps */
    bool _firstConnectAttempt;              /* First connection attempt flag */
    char* _will_topic;                      /* Pointer to the WILL message topic */
    char* _will_payload;                    /* Pointer to the WILL message payload */
    bool _will_retain;                      /* WILL message retain flag */
    bool _will_pub_on_disconnect;           /* Publish the WILL message when client disconnects */
    mqttPubRxFunc _pubReceiveCallback;      /* Function to be called when a publish message is received */
};

extern MqttClient g_mqtt;

#endif /* MQTT_H */