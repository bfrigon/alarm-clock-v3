//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/ntpclient.cpp
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

#include "ntpclient.h"
#include "../drivers/wifi/wifi.h"
#include "../drivers/rtc.h"
#include "../drivers/neoclock.h"
#include "../console/console.h"


/*! ------------------------------------------------------------------------
 *
 * @brief	Initialize class
 *
 */
NtpClient::NtpClient() {

    _nextSyncDelay = -1;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Begin the synchronization request
 *
 * @param   verbose    TRUE to display messages
 *
 * @return  TRUE if the request was successfuly started or FALSE otherwise. 
 *          Completion of the synchronization should be monitored with
 *          the isBusy() function. Use getTaskError() to get the error ID.
 * 
 */
bool NtpClient::sync( bool verbose ) {

    _verbose = verbose;

    if( g_wifi.isConnected() == false ) {
        this->setTaskError( ERR_WIFI_NOT_CONNECTED );
        return false;
    }

    if( this->isBusy() == true ) {
        this->endTask( TASK_SUCCESS );
    }
   

    if( _verbose == true ) {

        if( _lastSync != DateTime()) {
            g_console.print_P( S_CONSOLE_NTP_LAST_SYNC );
            g_console.printf_P( S_CONSOLE_DATE_FMT_UTC, 
                                getDayName( _lastSync.dow(), true ),
                                getMonthName( _lastSync.month(), true ),
                                _lastSync.day(),
                                _lastSync.hour(),
                                _lastSync.minute(),
                                _lastSync.second(),
                                0, 
                                _lastSync.year());;

            g_console.println();
        }
    }

    _lastSync = g_rtc.now();

    /* If configured ntp server address is an ip address, skip the hostname resolve */
    if( _server_ip.fromString( g_config.network.ntpserver ) == true ) {

        this->requestBind();

    /* Resolve the ntp server hostname */
    } else {

        if( _verbose == true ) {
            g_console.printf_P( S_CONSOLE_NTP_SYNC_WITH, g_config.network.ntpserver );
            g_console.println();
        }

        this->startTask( TASK_NTPCLIENT_RESOLVE_HOST, true );

        if( g_wifi.isBusy() == true ) {
            this->endTask( ERR_WIFI_BUSY );
            return false;
        }
    
        if( g_wifi.startHostnameResolve( g_config.network.ntpserver ) == false ) {

            /* Hostname resolve failed */
            this->endTask( g_wifi.getTaskError() );
            return false;
        }
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Request a socket bind to the NTP port 123
 *
 * @return  TRUE if the request was successful or FALSE otherwise. 
 * 
 */
bool NtpClient::requestBind() {
    this->startTask( TASK_NTPCLIENT_SOCKET_BIND, true );

    /* Request socket bind */
    if( _udp.begin( NTPCLIENT_NTP_PORT ) == false ) {

        this->endTask( ERR_NTPCLIENT_SOCKET_BIND_FAIL );
        return false;
    }

    /* Successfully requested socket bind, monitor status change 
       in runTask() */
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Format the NTP request packet and sent it
 *
 * @return  TRUE if the request was successful or FALSE otherwise. 
 * 
 */
bool NtpClient::sendNtpPacket() {

    this->startTask( TASK_NTPCLIENT_SEND_PACKET, true );

    if( _udp.beginPacket( _server_ip, NTPCLIENT_NTP_PORT ) == 0 ) {

        this->endTask( ERR_NTPCLIENT_SEND_FAIL );
        return false;
    }

    memset( &_packet, 0, sizeof( ntp_packet_t ));

    _packet.flags = 0b11100011;     /* LI=3, version=4, mode=client */
    _packet.stratum = 0;
    _packet.precision = 0;
    _packet.poll = 6;

    DateTime now;
    g_rtc.readTime( &now );


    /* Set the transmit timestamp */    
    uint32_t t1_sec = now.getEpoch() + EPOCH_NTP_OFFSET;
    _packet.ts_transmit.sec = _htonl( t1_sec );

    uint32_t t1_usec = (double)( g_rtc.getMillis() ) / 1000.0 * (double)0x100000000;
    _packet.ts_transmit.usec = _htonl( t1_usec );


    _udp.write( (char*)&_packet, sizeof( ntp_packet_t ));

    if( _verbose == true ) {

        g_console.printf_P( S_CONSOLE_NTP_SENDING, _server_ip[ 0 ], _server_ip[ 1 ], _server_ip[ 2 ], _server_ip[ 3 ] );
        g_console.println();
    }
    

    /* Send the packet */
    if( _udp.endPacket() == 0 ) {

        this->endTask( ERR_NTPCLIENT_SEND_FAIL );
        return false;
    }

    /* Successfully sent packet, monitor response
       in runTask() */
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Check if a response was received. If so, decode the NTP response 
 *          packet and update the clock accordingly
 *
 * @return  TRUE if a response was received and processed or FALSE otherwise. 
 * 
 */
bool NtpClient::readNtpResponse() {

    /* Check if we received a valid NTP packet */
    if( _udp.read( (char*)&_packet, sizeof( ntp_packet_t )) < sizeof( ntp_packet_t )) {

        /* Close the socket */
        _udp.stop();

        this->endTask( ERR_NTPCLIENT_INVALID_RESPONSE );
        return false;
    }

    DateTime now;
    g_rtc.readTime( &now );

    DateTime test;
    test = now;
    
    /* Read time stamps from the NTP response packet. */
    uint32_t t1_sec, t2_sec, t3_sec, t4_sec;
    t1_sec = _ntohl( _packet.ts_originate.sec );
    t2_sec = _ntohl( _packet.ts_receive.sec );
    t3_sec = _ntohl( _packet.ts_transmit.sec );
    t4_sec = now.getEpoch() + EPOCH_NTP_OFFSET;

    int32_t t1_ms, t2_ms, t3_ms, t4_ms;
    t1_ms = _ntohl( _packet.ts_originate.usec ) / (float)0x100000000 * 1000;
    t2_ms = _ntohl( _packet.ts_receive.usec ) / (float)0x100000000 * 1000;
    t3_ms = _ntohl( _packet.ts_transmit.usec ) / (float)0x100000000 * 1000;
    t4_ms = g_rtc.getMillis();


    /* Calculate offset from local clock to ntp server clock */    
    int32_t sec_offset = (( t2_sec - t1_sec ) + ( t3_sec - t4_sec ));
    int32_t ms_offset = (( t2_ms - t1_ms ) + ( t3_ms - t4_ms )) / 2;

    /* if delta t2-t1 and t3-t4 calculation gives an odd number, the
       division by 2 in an integer will result in a 0.5s loss. Add
       that 0.5s to the millisecond offset. */
    if( sec_offset % 2 ) {
        ms_offset += ( sec_offset >= 0 ) ? 500 : -500;
    }

    sec_offset /= 2;

    
    /* Add the millisecond offset to the current time */
    int16_t now_ms = g_rtc.getMillis() + ms_offset;

    /* If the millisecond offset overflow, adjust the seconds offset */
    if( now_ms > 1000 ) {
        now.offset( now_ms / 1000 );
        now_ms %= 1000;
    }

    /* Since the RTC cannot store milliseconds, we need to synchronise the
       RTC at the next second. Calculate the needed delay */
    int16_t wait = 1000 - now_ms;
    now.offset( sec_offset + 1 );

    /* Delayed clock adjustment */
    g_rtc.adjustClock( &now, wait );


    // g_console.printf( "t1_sec=%lu, t2_sec=%lu, t3_sec=%lu, t4_sec=%lu\r\n", t1_sec, t2_sec, t3_sec, t4_sec );
    // g_console.printf( "t1_ms=%ld, t2_ms=%ld, t3_ms=%ld, t4_ms=%ld\r\n", t1_ms, t2_ms, t3_ms, t4_ms );
    // g_console.printf( "offset: %ld sec, %ld ms\r\n", sec_offset, ms_offset );
    // g_console.printf( "Prev time: %lu.%03ld, adjust time: %lu.000, wait: %d ms\r\n", t4_sec, t4_ms, t4_sec + sec_offset + 1, wait );
    // g_console.println();


    /* Schedule the next synchronization */
    if( _nextSyncDelay != -1 ) {
        randomSeed( analogRead( PIN_A0 ));
        _nextSyncDelay = random( NTPCLIENT_SYNC_SCHD_MIN, NTPCLIENT_SYNC_SCHD_MAX );
    }


    if( _verbose == true ) {


        if( sec_offset > 0 && ms_offset < 0 ) {
            ms_offset += 1000;
            sec_offset--;
        }
        
        g_console.println();
        g_console.printf_P( S_CONSOLE_NTP_ADJUST, 
                            ( sec_offset < 0 || ms_offset < 0 ) ? '-' : '+',
                            ( 1 - ((( sec_offset >> 31 ) & 0x1 ) << 1 )) * sec_offset, 
                            ( 1 - ((( ms_offset >> 31 ) & 0x1 ) << 1 )) * ms_offset );
        g_console.println();                                    
    }


    /* Close the socket */
    _udp.stop();

    g_rtc.processEvents();

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Enable/disable the automatic time synchronization
 *
 * @param   enabled    Enable the auto-sync feature
 * @param   verbose    Display messages on the console 
 * 
 */
void NtpClient::setAutoSync( bool enabled, bool verbose ) {

    if( enabled == false ) {

        _nextSyncDelay = -1;
     
    } else {

        if( verbose == true ) {

            g_console.println();
            g_console.println_P( S_CONSOLE_NTP_AUTOSYNC_ON );
        }

        /* If WiFi is connected, immediately synchronize */
        if( g_wifi.isConnected() == true ) {
            
            this->sync( verbose );

        /* Or schedule the next retry */
        } else {

            _lastSync = g_rtc.now();
            _nextSyncDelay = NTPCLIENT_RETRY_DELAY;
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Monitor the different stages of the request.
 * 
 */
void NtpClient::runTask() {

    
    if( _nextSyncDelay != -1 && this->isBusy() == false && g_config.clock.use_ntp == true ) {
        
        DateTime now;
        now = g_rtc.now();

        if( now.getEpoch() > _lastSync.getEpoch() + _nextSyncDelay ) {
            this->sync( false );

            _nextSyncDelay = NTPCLIENT_RETRY_DELAY;
        }
    }

    switch( this->getCurrentTask() ) {

        /* Resolving NTP server hostname */
        case TASK_NTPCLIENT_RESOLVE_HOST: {
            if( this->getTaskRunningTime() > WIFI_RESOLVE_TIMEOUT ) {
                this->endTask( ERR_NTPCLIENT_UNKNOWN_HOSTNAME );
                return;
            }

            if( g_wifi.getHostnameResolveResults( _server_ip ) == true ) {

                if( _server_ip != 0 ) {

                    /* Request a socket bound to port 123 */
                    this->requestBind();
                    
                } else {

                    this->endTask( ERR_NTPCLIENT_UNKNOWN_HOSTNAME );
                    return;
                }
            }
        }
        break;


        /* Monitor bind socket status */
        case TASK_NTPCLIENT_SOCKET_BIND: {
            if( this->getTaskRunningTime() > NTPCLIENT_BIND_TIMEOUT ) {
                this->endTask( ERR_NTPCLIENT_SOCKET_BIND_FAIL );
                return;
            }

            if( _udp.bound() == true ) {

                /* Now that socket is bound, send the NTP request packet */
                this->sendNtpPacket();
            }
        }
        break;


        /* Monitor NTP response packet */
        case TASK_NTPCLIENT_SEND_PACKET: {
            if( this->getTaskRunningTime() > NTPCLIENT_REQ_TIMEOUT ) {

                this->endTask( ERR_NTPCLIENT_NO_RESPONSE );
                return;
            }

            if( _udp.parsePacket() > 0 ) {

                if( this->readNtpResponse() == true ) {

                    this->endTask( TASK_SUCCESS );
                } else {

                    this->endTask( ERR_NTPCLIENT_INVALID_RESPONSE );
                    return;
                }
            }
        }
        break;
    }
}