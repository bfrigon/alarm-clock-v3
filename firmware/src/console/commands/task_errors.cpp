//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/commands/task_errors.cpp
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
#include "../console.h"
#include "../../hardware.h"
#include "../../services/ntpclient.h"



/*! ------------------------------------------------------------------------
 *
 * @brief   Print the error message of the last command.
 *
 */
void Console::printCommandError() {

    switch( this->getTaskError() ) {

        case ERR_CONFIG_NO_SDCARD:
            this->println_P( S_STATUS_ERROR_NO_SDCARD );
            break;

        case ERR_CONFIG_FILE_CANT_OPEN:
        case ERR_CONFIG_FILE_WRITE:
            this->println_P( S_STATUS_ERROR_WRITE );
            break;

        case ERR_CONFIG_FILE_READ:
            this->println_P( S_STATUS_ERROR_READ );
            break;

        case ERR_CONFIG_FILE_NOT_FOUND:
            this->println_P( S_STATUS_ERROR_NOTFOUND );
            break;        

        case ERR_WIFI_BUSY:
            this->println_P( S_CONSOLE_WIFI_BUSY );
            break;

        case ERR_WIFI_NOT_CONNECTED:
            this->println_P( S_CONSOLE_NET_NOT_CONNECTED );
            break;

        case ERR_WIFI_INVALID_HOSTNAME:
            this->println_P( S_CONSOLE_NET_INVALID_HOST );
            break;

        case ERR_NTPCLIENT_UNKNOWN_HOSTNAME:
            this->printf_P( S_CONSOLE_NTP_UNKNOWN_HOST, g_config.network.ntpserver );
            this->println();
            break;

        case ERR_NTPCLIENT_SOCKET_BIND_FAIL:
            this->println_P( S_CONSOLE_NTP_BIND_FAIL );
            break;

        case ERR_NTPCLIENT_SEND_FAIL:
            this->println_P( S_CONSOLE_NTP_SEND_FAIL );
            break;

        case ERR_NTPCLIENT_INVALID_RESPONSE:
            this->println_P( S_CONSOLE_NTP_INVALID_RESP );
            break;

        case ERR_NTPCLIENT_NO_RESPONSE:
            this->println_P( S_CONSOLE_NTP_NO_RESP );
            break;

        default:
            this->printf_P( S_CONSOLE_UNKNOWN_ERROR, this->getTaskError() );
            this->println();
            break;
    }
}