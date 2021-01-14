//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/ntpclient.h
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
#ifndef NTPCLIENT_H
#define NTPCLIENT_H

#include <Arduino.h>

#include "../libs/itask.h"
#include "../drivers/wifi/wifi.h"
#include "../config.h"
#include "../drivers/wifi/udpclient.h"


#define NTPCLIENT_NTP_PORT      123
#define NTPCLIENT_PACKET_SIZE   48
#define NTPCLIENT_REQ_TIMEOUT   5000
#define NTPCLIENT_BIND_TIMEOUT  2000

#define NTPCLIENT_RETRY_DELAY   120     /* 2 */
#define NTPCLIENT_SYNC_SCHD_MIN 36000   /* 10 hours */
#define NTPCLIENT_SYNC_SCHD_MAX 50400   /* 14 hours */



/* Tasks ID's */
enum {
    TASK_NTPCLIENT_RESOLVE_HOST = 1,
    TASK_NTPCLIENT_SOCKET_BIND,
    TASK_NTPCLIENT_SEND_PACKET
};


/* Task errors */
enum {
    ERR_NTPCLIENT_UNKNOWN_HOSTNAME = (-20),
    ERR_NTPCLIENT_SOCKET_BIND_FAIL,
    ERR_NTPCLIENT_SEND_FAIL,
    ERR_NTPCLIENT_NO_RESPONSE,
    ERR_NTPCLIENT_INVALID_RESPONSE,
};


typedef struct
{
    uint8_t flags;            // Flags
                              // li.   2 bits. Leap indicator.
                              // vn.   3 bits. Version number of the protocol.
                              // mode. 3 bits. Client will pick mode 3 for client.

    uint8_t stratum;          // Stratum level of the local clock.
    uint8_t poll;             // Maximum interval between successive messages.
    uint8_t precision;        // Precision of the local clock.

    uint32_t rootDelay;       // Total round trip delay time.
    uint32_t rootDispersion;  // Max error aloud from primary clock source.
    uint32_t refId;           // Reference clock identifier.
    
    struct {
        uint32_t sec;         // Reference time-stamp seconds.
        uint32_t usec;        // Reference time-stamp fraction of a second.
    } ts_ref;

    struct {
        uint32_t sec;         // Originate time-stamp seconds.
        uint32_t usec;        // Originate time-stamp fraction of a second.
    } ts_originate;

    struct {
        uint32_t sec;         // Received time-stamp seconds.
        uint32_t usec;        // Received time-stamp fraction of a second.
    } ts_receive;

    struct {
        uint32_t sec;         // Transmit time-stamp seconds.
        uint32_t usec;        // Transmit time-stamp fraction of a second.
    } ts_transmit;

} ntp_packet_t;


//**************************************************************************
//
// NTP client
//
//**************************************************************************
class NtpClient : public ITask {
  public:

    NtpClient();

    bool sync( bool verbose = false);
    void runTask();
    void getPreviousSync( DateTime &dt );
    void getPreviousSyncOffset( int32_t seconds, int16_t milliseconds );
    void setAutoSync( bool enabled, bool verbose = false );


  private:
    IPAddress _server_ip;
    UDPClient _udp;
    ntp_packet_t _packet;
    DateTime _lastSync;
    long _nextSyncDelay = 0;
    bool _verbose = false;

    bool requestBind();
    bool sendNtpPacket();
    bool readNtpResponse();
};

extern NtpClient g_ntp;



#endif /* NTPCLIENT_H */