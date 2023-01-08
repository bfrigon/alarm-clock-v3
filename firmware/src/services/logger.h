//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/logger.h
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
#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "console/console_base.h"



#define MAX_LOG_ENTRIES 50


enum {
    EVENT_EMPTY = 0,
    EVENT_RESET,
    EVENT_FACTORY_RESET,
    EVENT_RESTORED_CONFIG,
    EVENT_TELNET_SERVICE_ENABLED,
    EVENT_TELNET_SERVICE_DISABLED,
    EVENT_TELNET_SESSION_START,
    EVENT_TELNET_DISCONNECT,
    EVENT_WIFI_CONNECTED,
    EVENT_WIFI_DISCONNECTED,
    EVENT_WIFI_CONNECT_FAIL,
    EVENT_CON_ADJ_DATETIME,
    EVENT_CON_ADJ_TZ,
    EVENT_NTP_FAIL_NO_WIFI,
    EVENT_NTP_FAIL_SOCKET_ERR,
    EVENT_NTP_FAIL_CANT_RESOLVE_HOST,
    EVENT_NTP_FAIL_NO_RESPONSE,
    EVENT_NTP_FAIL_INVALID_RESPONSE,
    EVENT_NTP_FAIL_SEND_PACKET,
    EVENT_POWER_ON_BATTERY,
    EVENT_POWER_RESTORED,

};





struct LogEntry {
    uint8_t type;
    uint32_t timestamp;
    uint32_t flags;
    uint8_t repeat;
};

class Logger {
  public:

    Logger();

    void add( uint8_t eventID, uint32_t flags = 0 );
    uint8_t getLastIndex();
    uint8_t getFirstIndex();

    bool printLogEntry( ConsoleBase *console, uint8_t index );

    

  
  private:
    LogEntry _entries[ MAX_LOG_ENTRIES ];
    uint8_t _ptrHead = 0;
    uint8_t _ptrTail = 0;

    uint32_t getTimestamp();
    void printLogEntryMessage( IPrint *objPrint, uint8_t type, uint32_t flags );

    
};

extern Logger g_log;

#endif /* LOGGER_H */