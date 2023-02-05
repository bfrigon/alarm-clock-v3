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

#include <task_errors.h>
#include <drivers/wifi/wifi.h>
#include <drivers/rtc.h>
#include <drivers/neoclock.h>
#include <console/console_base.h>
#include <timezone.h>
#include <tzdata.h>
#include "logger.h"
#include "ntpclient.h"



/*******************************************************************************
 *
 * @brief   Initialize class
 *
 */
NtpClient::NtpClient() {

    _nextSyncDelay = 0;
}


/*******************************************************************************
 *
 * @brief   Begin the synchronization request
 *
 * @param   verbose    TRUE to display messages
 *
 * @return  TRUE if the request was successfuly started or FALSE otherwise. 
 *          Completion of the synchronization should be monitored with
 *          the isBusy() function. Use getTaskError() to get the error ID.
 * 
 */
bool NtpClient::sync( ConsoleBase *console ) {

    _console = console;

    if( g_wifi.connected() == false ) {
        this->setTaskError( ERR_WIFI_NOT_CONNECTED );
        g_log.add( EVENT_NTP_FAIL_NO_WIFI, 0);
        return false;
    }

    if( this->isBusy() == true ) {
        this->endTask( TASK_SUCCESS );
    }
   

    if( console != NULL ) {

        if( _lastSync != DateTime()) {

            console->print_P( S_CONSOLE_NTP_LAST_SYNC );
            console->printDateTime( &_lastSync, TZ_UTC );
            console->println();
        }
    }

    _lastSync = g_rtc.now();

    /* If configured ntp server address is an ip address, skip the hostname resolve. */
    if( _server_ip.fromString( g_config.network.ntpserver ) == true ) {

        this->requestBind();

    /* Resolve the ntp server hostname. */
    } else {

        if( console != NULL ) {
            console->printf_P( S_CONSOLE_NTP_SYNC_WITH, g_config.network.ntpserver );
            console->println();
        }

        this->startTask( TASK_NTPCLIENT_RESOLVE_HOST, true );

        if( g_wifi.isBusy() == true ) {
            this->endTask( ERR_WIFI_BUSY );
            return false;
        }
    
        if( g_wifi.startHostnameResolve( g_config.network.ntpserver ) == false ) {

            /* Hostname resolve failed. */
            this->endTask( g_wifi.getTaskError() );
            return false;
        }
    }

    return true;
}


/*******************************************************************************
 *
 * @brief   Request a socket bind to the NTP port 123.
 *
 * @return  TRUE if the request was successful or FALSE otherwise. 
 * 
 */
bool NtpClient::requestBind() {
    this->startTask( TASK_NTPCLIENT_SOCKET_BIND, true );

    /* Request socket bind */
    if( _udp.begin( NTPCLIENT_NTP_PORT ) == false ) {

        this->endTask( ERR_NTPCLIENT_SOCKET_BIND_FAIL );
        g_log.add( EVENT_NTP_FAIL_SOCKET_ERR );
        return false;
    }

    /* Successfully requested socket bind, monitor status change 
       in runTask(). */
    return true;
}


/*******************************************************************************
 *
 * @brief   Format the NTP request packet and sent it.
 *
 * @return  TRUE if the request was successful or FALSE otherwise. 
 * 
 */
bool NtpClient::sendNtpPacket() {

    this->startTask( TASK_NTPCLIENT_SEND_PACKET, true );

    if( _udp.beginPacket( _server_ip, NTPCLIENT_NTP_PORT ) == 0 ) {

        this->endTask( ERR_NTPCLIENT_SEND_FAIL );
        g_log.add( EVENT_NTP_FAIL_SEND_PACKET );

        _udp.stop();
        return false;
    }

    memset( &_packet, 0, sizeof( ntp_packet_t ));

    _packet.flags = 0b11100011;     /* LI=3, version=4, mode=client */
    _packet.stratum = 0;
    _packet.precision = 0;
    _packet.poll = 6;

    DateTime now;
    g_rtc.readTime( &now );


    /* Set the transmit timestamp. */    
    uint32_t t1_sec = now.getEpoch() + EPOCH_NTP_OFFSET;
    _packet.ts_transmit.sec = _htonl( t1_sec );

    uint32_t t1_usec = (double)( g_rtc.getMillis() ) / 1000.0 * (double)0x100000000;
    _packet.ts_transmit.usec = _htonl( t1_usec );


    _udp.write( (char*)&_packet, sizeof( ntp_packet_t ));

    if( _console != NULL ) {

        _console->printf_P( S_CONSOLE_NTP_SENDING, _server_ip[ 0 ], _server_ip[ 1 ], _server_ip[ 2 ], _server_ip[ 3 ] );
        _console->println();
    }
    

    /* Send the packet */
    if( _udp.endPacket() == 0 ) {

        this->endTask( ERR_NTPCLIENT_SEND_FAIL );
        g_log.add( EVENT_NTP_FAIL_SEND_PACKET );

        _udp.stop();
        return false;
    }

    /* Successfully sent packet, monitor response
       in runTask(). */
    return true;
}


/*******************************************************************************
 *
 * @brief   Check if a response was received. If so, decode the NTP response 
 *          packet and update the clock accordingly
 *
 * @return  TRUE if a response was received and processed or FALSE otherwise. 
 * 
 */
bool NtpClient::readNtpResponse() {

    /* Check if we received a valid NTP packet */
    if( _udp.read( (char*)&_packet, sizeof( ntp_packet_t )) < (int)sizeof( ntp_packet_t )) {

        /* Close the socket */
        _udp.stop();

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

    
    /* Add the millisecond offset to the current time. */
    int16_t now_ms = g_rtc.getMillis() + ms_offset;

    /* If the millisecond offset overflow, adjust the seconds offset. */
    if( now_ms > 1000 ) {
        now.offset( now_ms / 1000 );
        now_ms %= 1000;
    }

    /* Since the RTC cannot store milliseconds, we need to synchronise the
       RTC at the next second. Calculate the needed delay. */
    int16_t wait = 1000 - now_ms;
    now.offset( sec_offset + 1 );

    /* Delayed clock adjustment */
    g_rtc.adjustClock( &now, wait );


    // g_console.printf( "t1_sec=%lu, t2_sec=%lu, t3_sec=%lu, t4_sec=%lu\r\n", t1_sec, t2_sec, t3_sec, t4_sec );
    // g_console.printf( "t1_ms=%ld, t2_ms=%ld, t3_ms=%ld, t4_ms=%ld\r\n", t1_ms, t2_ms, t3_ms, t4_ms );
    // g_console.printf( "offset: %ld sec, %ld ms\r\n", sec_offset, ms_offset );
    // g_console.printf( "Prev time: %lu.%03ld, adjust time: %lu.000, wait: %d ms\r\n", t4_sec, t4_ms, t4_sec + sec_offset + 1, wait );
    // g_console.println();


    /* Schedule the next synchronization. */
    if( _nextSyncDelay != 0 ) {

        randomSeed( analogRead( PIN_A0 ));
        _nextSyncDelay = random( NTPCLIENT_SYNC_SCHD_MIN, NTPCLIENT_SYNC_SCHD_MAX );
    }

    if( sec_offset > 0 && ms_offset < 0 ) {
        ms_offset += 1000;
        sec_offset--;
    }

    /* If ntp sync was called from console, print adjustment offset. */
    if( _console != NULL ) {
        _console->println();
        _console->printf_P( S_CONSOLE_NTP_ADJUST, 
                            ( sec_offset < 0 || ms_offset < 0 ) ? '-' : '+',
                            ( 1 - ((( sec_offset >> 31 ) & 0x1 ) << 1 )) * sec_offset, 
                            ( 1 - ((( ms_offset >> 31 ) & 0x1 ) << 1 )) * ms_offset );
        _console->println();
    }

    if( sec_offset > 10 || sec_offset < -10 ) {
        g_log.add( EVENT_NTP_ADJUST_CLOCK, sec_offset );
    }


    /* Close the socket */
    _udp.stop();

    g_rtc.processEvents();

    return true;
}


/*******************************************************************************
 *
 * @brief   Enable/disable the automatic time synchronization.
 *
 * @param   enabled    Enable the auto-sync feature
 * @param   verbose    Display messages on the console 
 * 
 */
void NtpClient::setAutoSync( bool enabled, ConsoleBase *console ) {

    if( enabled == false ) {

        _nextSyncDelay = 0;
     
    } else {

        if( console != NULL ) {

            console->println();
            console->print_P( S_CONSOLE_NTP_AUTOSYNC );
            console->println_P( S_ENABLED );
        }

        /* If WiFi is connected, immediately synchronize. */
        if( g_wifi.connected() == true ) {
            
            this->sync( console );

        /* Or schedule the next retry. */
        } else {
            this->setTaskError( ERR_WIFI_NOT_CONNECTED );

            _lastSync = g_rtc.now();
            _nextSyncDelay = 1;
        }
    }
}


/*******************************************************************************
 *
 * @brief   Monitor the different stages of the request.
 * 
 */
void NtpClient::runTasks() {

    switch( this->getCurrentTask() ) {

        /* Resolving NTP server hostname. */
        case TASK_NTPCLIENT_RESOLVE_HOST: {
            if( this->getTaskRunningTime() > WIFI_RESOLVE_TIMEOUT ) {
                this->endTask( ERR_NTPCLIENT_UNKNOWN_HOSTNAME );
                g_log.add( EVENT_NTP_FAIL_CANT_RESOLVE_HOST );
                return;
            }

            if( g_wifi.getHostnameResolveResults( _server_ip ) == true ) {

                if( _server_ip != 0 ) {

                    /* Request a socket bound to port 123. */
                    this->requestBind();
                    
                } else {

                    this->endTask( ERR_NTPCLIENT_UNKNOWN_HOSTNAME );
                    g_log.add( EVENT_NTP_FAIL_CANT_RESOLVE_HOST );
                    return;
                }
            }
        }
        break;


        /* Monitor bind socket status. */
        case TASK_NTPCLIENT_SOCKET_BIND: {
            
            if( this->getTaskRunningTime() > NTPCLIENT_BIND_TIMEOUT ) {
                this->endTask( ERR_NTPCLIENT_SOCKET_BIND_FAIL );
                g_log.add( EVENT_NTP_FAIL_SOCKET_ERR );

                _udp.stop();

                return;
            }

            if( _udp.bound() ) {

                /* Now that socket is bound, send the NTP request packet. */
                this->sendNtpPacket();
            }
        }
        break;


        /* Monitor NTP response packet. */
        case TASK_NTPCLIENT_SEND_PACKET: {
            if( this->getTaskRunningTime() > NTPCLIENT_REQ_TIMEOUT ) {

                this->endTask( ERR_NTPCLIENT_NO_RESPONSE );
                g_log.add( EVENT_NTP_FAIL_NO_RESPONSE );
                
                return;
            }

            if( _udp.parsePacket() > 0 ) {

                if( this->readNtpResponse() == true ) {

                    this->endTask( TASK_SUCCESS );
                } else {

                    this->endTask( ERR_NTPCLIENT_INVALID_RESPONSE );
                    g_log.add( EVENT_NTP_FAIL_INVALID_RESPONSE );

                    return;
                }
            }
        }
        break;

        /* No Task running */
        default: {
            if( g_wifi.connected() == true && _nextSyncDelay > 0 ) {
        
                DateTime now;
                now = g_rtc.now();

                if( now.getEpoch() > _lastSync.getEpoch() + _nextSyncDelay ) {
                
                    if( this->sync( NULL ) == false ) {
                        _nextSyncDelay = NTPCLIENT_RETRY_DELAY;

                        return;
                    }

                    if( this->getTaskError() != TASK_SUCCESS ) {

                        _nextSyncDelay = NTPCLIENT_RETRY_DELAY;
                    }
                }
            }
        }
        break;
    }
}


/*******************************************************************************
 *
 * @brief   Prints NTP client status on the console.
 * 
 * @param   console    ConsoleBase object to print results to.
 * 
 */
void NtpClient::printNTPStatus( ConsoleBase *console ) {

    /* Print the NTP client configuration */
    console->printf_P( S_CONSOLE_NTP_SERVER, g_config.network.ntpserver );
    console->println();

    console->print_P( S_CONSOLE_NTP_AUTOSYNC );
    console->println_P( (g_config.clock.use_ntp == true ) ? S_ENABLED : S_DISABLED );

    console->println();

    /* Print the last synchronization date/time. */
    console->print_P( S_CONSOLE_NTP_LAST_SYNC );
    console->printDateTime( &_lastSync, TZ_UTC );
    console->println();

    /* Print delay before next synchronization. */
    if( _nextSyncDelay == 1 && g_wifi.connected() == false ) {
        
        console->print_P( S_CONSOLE_NTP_NEXT_SYNC );
        console->println_P( S_CONSOLE_NTP_WAITING_WIFI );

    } else if( _nextSyncDelay > 0 ) {
        unsigned long remaining;
        remaining = ( _lastSync.getEpoch() + _nextSyncDelay ) - g_rtc.now()->getEpoch();

        if( remaining > 0 ) {
            console->print_P( S_CONSOLE_NTP_NEXT_SYNC );
            console->printTimeInterval( remaining, S_DATETIME_SEPARATOR_COMMA );
            console->println();
        }
    }

    console->println();

    /* Print the previous attempt error. */
    console->print_P( S_CONSOLE_NTP_LAST_ERROR );

    if( this->getTaskError() == TASK_SUCCESS ) {
        console->println_P( S_CONSOLE_NONE );
    } else {
        console->printErrorMessage( this->getTaskError() );
    }
}