//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/logger.cpp
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

#include "logger.h"
#include "drivers/wifi/wifi.h"
#include "services/mqtt.h"
#include "drivers/rtc.h"
#include <time.h>
#include <timezone.h>
#include <tzdata.h>



/*******************************************************************************
 *
 * @brief   Class initializer
 *
 */
Logger::Logger() {
    memset( &_entries, 0, sizeof( LogEntry ) * MAX_LOG_ENTRIES);
    _ptrHead = 0;
    _ptrTail = 0;
}


/*******************************************************************************
 *
 * @brief   Get current timestamp.
 * 
 * @return  Number of seconds since January 1st 2000.
 *
 */
uint32_t Logger::getTimestamp() {
    DateTime now = g_rtc.now();

    if( now.year() < 2000 ) {
        return 0;
    }

    /* Number of days since Jan 1 2000 */
    uint32_t days;
    days = (( now.year() - 2000 ) * 365 );        

    /* Add number of leap years since 2000 */
    days += (( now.year() - 2000 ) / 4 );         
    days += ( now.year() % 4 ) ? 1 : 0;           

    /* Add number of days since the begining of the current year 
       until the start of the current month */
    uint8_t i;
    for( i = 1; i <= now.month() - 1; i++ ) {
        days += getMonthNumDays( i, now.year() );
    }

    /* Add number of days in the current month */
    days += ( now.day() - 1 );                   

    return ( days * 86400L ) + ( now.hour() * 3600L ) + ( now.minute() * 60L ) + now.second();
}


/*******************************************************************************
 *
 * @brief   Add an event entry to the system log.
 *
 * @param   eventType   Event type ID
 * @param   flags       Additional event information to include 
 *                      with the event entry.
 * 
 */
void Logger::add( uint8_t eventType, uint32_t flags ) {

    /* Invalid event type */
    if( eventType == EVENT_EMPTY ) {
        return;
    }

    /* If the previous log entry is a duplicate, don't create another entry, 
       increments the repeat counter instead. */
    if( _entries[ _ptrTail ].type == eventType && _entries[ _ptrTail ].flags == flags ) {
        
        _entries[ _ptrTail ].repeat++;
        _entries[ _ptrTail ].timestamp = this->getTimestamp();

        if( _entries[ _ptrTail ].repeat > 250 ) {
            _entries[ _ptrTail ].repeat = 250;
        }

        return;
    }

    if( _entries[ 0 ].type != EVENT_EMPTY ) {
        _ptrTail++;

        if( _ptrTail > MAX_LOG_ENTRIES - 1 ) {
            _ptrTail = 0;
            _ptrHead = 1;
        }

        if( _ptrHead == _ptrTail ) {
            _ptrHead++;
        }

        if( _ptrHead > MAX_LOG_ENTRIES - 1 ) {
            _ptrHead = 0;
        }
    }

    _entries[ _ptrTail ].type = eventType;
    _entries[ _ptrTail ].flags = flags;
    _entries[ _ptrTail ].timestamp = this->getTimestamp();
}


/*******************************************************************************
 *
 * @brief   Gets the pointer to the most recent entry im the event log table.
 * 
 * @return  Index of the most recent event in the log table.
 * 
 */
uint8_t Logger::getFirstIndex() {
    return _ptrHead;
}


/*******************************************************************************
 *
 * @brief   Gets the pointer to the oldest entry im the event log table.
 * 
 * @return  Index of the oldest event in the log table.
 * 
 */
uint8_t Logger::getLastIndex() {
    return _ptrTail;
}


/*******************************************************************************
 *
 * @brief   Prints a event entry from the system log table.
 * 
 * @param   console     Console object to print to.
 * @param   index       Entry ID in the system log table.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool Logger::printLogEntry( ConsoleBase *console, uint8_t index ) {

    if( index > MAX_LOG_ENTRIES - 1 ) {
        return false;
    }

    if( _entries[ index ].type == EVENT_EMPTY ) {
        return false;
    }

    
    /* Convert log entry timestamp to DateTime object. */
    DateTime evTime = DateTime(2000, 1, 1, 0, 0, 0 );
    evTime += _entries[ index ].timestamp;

    /* Convert to local time. */
    g_timezone.toLocal( &evTime );

    console->printf_P( PSTR( "[ %d-%02d-%02d %02d:%02d:%02d ] " ), 
                       evTime.year(),
                       evTime.month(),
                       evTime.day(),
                       evTime.hour(),
                       evTime.minute(),
                       evTime.second());    


    this->printLogEntryMessage( console, _entries[ index ].type, _entries[ index ].flags );

    /* Print how many times the log entry occured if more than 1. */
    if( _entries[ index ].repeat >= 250 ) {
        console->print_P( S_LOG_REPEAT_LIMIT );

    } else  if( _entries[ index ].repeat > 0 ) {
        console->printf_P( S_LOG_REPEAT, _entries[ index ].repeat + 1 );
    }

    console->println();
    
    return true;
}


/*******************************************************************************
 *
 * @brief   Print the message associated with an event type.
 * 
 * @param   output  Console object to print to.
 * @param   type    Event type
 * @param   flags   Additional event information
 * 
 */
void Logger::printLogEntryMessage( IPrint *output, uint8_t type, uint32_t flags ) {
    /* MCU Reset */
    if( type == EVENT_RESET ) {
        output->printf_P( S_LOGMSG_RESET, flags );
    
    /* Factory reset */
    } else if( type == EVENT_FACTORY_RESET ) {
        output->print_P( S_LOGMSG_FACTORY_RESET );

    /* Config restored */
    } else if( type == EVENT_RESTORED_CONFIG ) {
        output->print_P( S_LOGMSG_CONFIG_RESTORED );

    /* Running on battery power */
    } else if( type == EVENT_POWER_ON_BATTERY ) {
        output->print_P( S_LOGMSG_POWER_ON_BATTERY );

    /* Main power restored */
    } else if( type == EVENT_POWER_RESTORED ) {
        output->print_P( S_LOGMSG_POWER_RESTORED );

    /* Telnet: Session start */
    } else if( type == EVENT_TELNET_SESSION_START ) {
        IPAddress ip = flags;
        output->printf_P( S_LOGMSG_TELNET_SESSION_START, ip[0], ip[1], ip[2], ip[3] );
    
    /* Telnet: Session ended */
    } else if( type == EVENT_TELNET_DISCONNECT && flags == 0 ) {
        output->print_P( S_LOGMSG_TELNET_SESSION_END );

    /* Telnet: Session timeout */
    } else if( type == EVENT_TELNET_DISCONNECT && flags != 0 ) {
        output->print_P( S_LOGMSG_TELNET_SESSION_TIMEOUT );

    /* WiFi: Connection established */
    } else if( type == EVENT_WIFI_CONNECTED ) {
        output->print_P( S_LOGMSG_WIFI_CONNECTED );

    /* WiFi: Disconnected */
    } else if( type == EVENT_WIFI_DISCONNECTED ) {
        output->print_P( S_LOGMSG_WIFI_DISCONNECTED );

    /* WiFi: Connection fail (Disconnected) */
    } else if( type == EVENT_WIFI_CONNECT_FAIL && flags == WIFI_STATUS_DISCONNECTED ) {
        output->print_P( S_LOGMSG_WIFI_FAIL_CONN_LOST );

    /* WiFi: Connection fail (Timeout) */
    } else if( type == EVENT_WIFI_CONNECT_FAIL && flags == WIFI_STATUS_CONNECT_TIMEOUT ) {
        output->print_P( S_LOGMSG_WIFI_FAIL_CONN_TIMEOUT );

    /* WiFi: Connection fail */
    } else if( type == EVENT_WIFI_CONNECT_FAIL ) {
        output->print_P( S_LOGMSG_WIFI_FAIL_CONN );

    /* Changed timezone from console */
    } else if( type == EVENT_CON_ADJ_TZ ) {

        TimeZone tz;
        tz.setTimezoneByID( flags );

        output->printf_P( S_LOGMSG_CON_ADJ_TZ, tz.getName() );

    /* Changed date/time from console */
    } else if( type == EVENT_CON_ADJ_DATETIME ) {

        output->print_P( S_LOGMSG_CON_ADJ_TIME );

    /* NTP: Sync fail (Not connected to WiFi) */
    } else if( type == EVENT_NTP_FAIL_NO_WIFI ) {
        
        output->print_P( S_LOGMSG_NTP_FAIL_NO_WIFI );

    /* NTP: Sync fail (cannot resolve host) */
    } else if( type == EVENT_NTP_FAIL_CANT_RESOLVE_HOST ) {
        
        output->print_P( S_LOGMSG_NTP_FAIL_CANT_RESOLVE_HOST );

    /* NTP: Sync fail (Socket error) */
    } else if( type == EVENT_NTP_FAIL_SOCKET_ERR ) {
        
        output->print_P(S_LOGMSG_NTP_FAIL_SOCKET_ERR );

    /* NTP: Sync fail (No response from server) */
    } else if( type == EVENT_NTP_FAIL_NO_RESPONSE ) {
        
        output->print_P(S_LOGMSG_NTP_FAIL_NO_RESPONSE );

    /* NTP: Sync fail (Received invalid response from server) */
    } else if( type == EVENT_NTP_FAIL_INVALID_RESPONSE ) {
        
        output->print_P(S_LOGMSG_NTP_FAIL_INVALID_RESPONSE );

    /* NTP: Sync fail (No response from server) */
    } else if( type == EVENT_NTP_FAIL_SEND_PACKET ) {
        
        output->print_P(S_LOGMSG_NTP_FAIL_SEND_PACKET);

    /* Telnet service enabled */
    } else if( type == EVENT_TELNET_SERVICE_ENABLED ) {

        output->print_P(S_LOGMSG_TELNET_SERVICE_ENABLED );
    
    /* Telnet service disabled */
    } else if( type == EVENT_TELNET_SERVICE_DISABLED ) {

        output->print_P(S_LOGMSG_TELNET_SERVICE_DISABLED );

    /* MQTT: Cannot connect to broker (no wifi) */
    } else if( type == EVENT_MQTT_FAIL_NO_WIFI ) {

        output->print_P( S_LOGMSG_MQTT_FAIL_NO_WIFI );

    /* MQTT: Cannot resolve broker hostname */
    } else if( type == EVENT_MQTT_CANT_RESOLVE_HOST ) {

        output->print_P( S_LOGMSG_MQTT_CANT_RESOLVE_HOST );

    /* MQTT: No response from broker */
    } else if( type == EVENT_MQTT_BROKER_NO_RESPONSE ) {

        output->print_P( S_LOGMSG_MQTT_NO_RESPONSE );

    /* MQTT: Connection refused */
    } else if( type == EVENT_MQTT_CONNECT_REFUSED && flags != MQTT_CONNACK_NOT_AUTHORIZED ) {

        output->printf_P( S_LOGMSG_MQTT_CONNECT_REFUSED, flags );

    /* MQTT: Connection refused (unauthorized) */
    } else if( type == EVENT_MQTT_CONNECT_REFUSED && flags == MQTT_CONNACK_NOT_AUTHORIZED ) {

        output->printf_P( S_LOGMSG_MQTT_CONNECT_UNAUTHORIZED, flags );

    /* MQTT: client enabled */
    } else if( type == EVENT_MQTT_ENABLED ) {

        output->print_P( S_LOGMSG_MQTT_CLIENT_ENABLED );

    /* MQTT: client disabled */
    } else if( type == EVENT_MQTT_DISABLED ) {

        output->print_P( S_LOGMSG_MQTT_CLIENT_DISABLED );

    /* MQTT: Attempt reconnect */
    } else if( type == EVENT_MQTT_ATTEMPT_RECONNECT ) {

        output->print_P( S_LOGMSG_MQTT_ATTEMPT_RECONNECT );

    /* MQTT: Connected */
    } else if( type == EVENT_MQTT_CONNECTED ) {

        output->print_P( S_LOGMSG_MQTT_CONNECTED );

    /* MQTT: Disconnected */
    } else if( type == EVENT_MQTT_DISCONNECTED ) {

        output->print_P( S_LOGMSG_MQTT_DISCONNECTED );

    /* MQTT: Unexpected response */
    } else if( type == EVENT_MQTT_UNEXPECTED_RESPONSE ) {

        output->print_P( S_LOGMSG_MQTT_UNEXPECTED_RESPONSE );

    /* MQTT: Socket error */
    } else if( type == EVENT_MQTT_SOCKET_ERROR ) {

        output->print_P( S_LOGMSG_MQTT_SOCKET_ERROR );

    /* Unknown log entry */
    } else {
        output->printf_P( S_LOGMSG_UNKNOWN, type, flags );
    }
}