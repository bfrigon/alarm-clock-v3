//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/task_errors.h
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
#ifndef TASKERROR_H
#define TASKERROR_H

#include <Arduino.h>

//extern const char* getErrorMessage( int8_t error );

enum {
    ERR_CONSOLE_INVALID_TIMEZONE  = -127,

    ERR_WIFI_BUSY,
    ERR_WIFI_NOT_CONNECTED,
    ERR_WIFI_INVALID_HOSTNAME,
    ERR_WIFI_UNKNOWN_HOSTNAME,
    ERR_WIFI_NETWORK_UNREACHABLE,
    ERR_WIFI_ALREADY_CONNECTED,
    ERR_WIFI_PING_TIMEOUT,
    ERR_WIFI_PING_ERROR,

    ERR_CONFIG_NO_SDCARD,
    ERR_CONFIG_FILE_WRITE,
    ERR_CONFIG_FILE_NOT_FOUND,
    ERR_CONFIG_FILE_READ,
    ERR_CONFIG_FILE_CANT_OPEN,
    ERR_CONFIG_FILE_EXISTS,

    ERR_NTPCLIENT_UNKNOWN_HOSTNAME,
    ERR_NTPCLIENT_SOCKET_BIND_FAIL,
    ERR_NTPCLIENT_SEND_FAIL,
    ERR_NTPCLIENT_NO_RESPONSE,
    ERR_NTPCLIENT_INVALID_RESPONSE,

};




#endif /* TASKERROR_H */

