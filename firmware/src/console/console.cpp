//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/console.cpp
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
#include "console.h"
#include "../hardware.h"
#include "../task_errors.h"
#include "../services/ntpclient.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 */
Console::Console() {

    memset( _inputBuffer, 0, INPUT_BUFFER_LENGTH + 1);
    memset( _historyBuffer, 0, CMD_HISTORY_BUFFER_LENGTH + 1);

    
    _inputParameter= NULL;
    _inputBufferLimit = INPUT_BUFFER_LENGTH;
    _inputEnabled = false;
    _inputHidden = false;
    _taskIndex = 0;
    _escapeSequence = 0;

    this->resetInput();

    /* Initialize IPrint interface */
    this->_initPrint();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   IPrint interface callback for printing a single character. Sends the 
 *          output to the serial port.
 * 
 * @param   c   character to print
 *
 * @return  Number of bytes written
 */
uint8_t Console::_print( char c ) {
    return Serial.write( c );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the console
 *
 * @param   baud    Speed of the serial port
 * 
 */
void Console::begin( unsigned long baud ) {

    Serial.begin( baud );
    while (!Serial);
    
    this->resetConsole();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Send control sequences to clear the remote terminal screen
 * 
 */
void Console::clearScreen() {

    this->sendControlSequence( CTRL_SEQ_CLEAR_SCREEN );
    this->sendControlSequence( CTRL_SEQ_CURSOR_POSITION, 0, 0 );
    this->sendControlSequence( CTRL_SEQ_CLEAR_SCROLLBACK );

}


/*! ------------------------------------------------------------------------
 *
 * @brief   Clear the screen and display the login message
 * 
 */
void Console::resetConsole() {

    this->clearScreen();

    this->println_P( S_CONSOLE_WELCOME );
    this->println();

    this->resetInput();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sends a control sequence to the remote terminal.
 * 
 * @param   sequence    Sequence code to send
 * @param   row         Optional row parameter
 * @param   col         Optional column parameter
 * 
 */
void Console::sendControlSequence( uint8_t sequence, uint8_t row, uint8_t col ) {

    switch( sequence ) {

        case CTRL_SEQ_CLEAR_SCREEN:
            this->print_P( PSTR( "\033[2J" ));
            break;

        case CTRL_SEQ_CLEAR_SCROLLBACK:
            this->print_P( PSTR( "\033[3J" ));
            break;

        case CTRL_SEQ_CURSOR_POSITION:
            this->printf_P( PSTR( "\033[%d;%dH" ), row, col );
            break;

        case CTRL_SEQ_ERASE_LINE:
            this->print_P( PSTR( "\033[2K" ));
            break;

        case CTRL_SEQ_CURSOR_COLUMN:
            this->printf_P( PSTR( "\033[%dG" ), col );
            break;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Decode incomming control sequence characters
 * 
 * @param   ch    Next character in the sequence to process
 * 
 */
void Console::processControlSequence( char ch ) {

    if( _escapeSequence == 0) {
        return;
    }

    /* Check if control sequence is valid */
    if( _escapeSequence == 1) {

        _escapeSequence = ( ch == '[' ) ? 2 : 0;
        return;
    }


    switch( ch ) {

        /* Cursor up */
        case 'A':
            this->readHistoryBuffer( true );
            break;

        /* Cursor down */
        case 'B':
            this->readHistoryBuffer( false );
            break;
        
    }

    _escapeSequence = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Discard the user input from the serial port
 * 
 */
void Console::resetInput() {

    /* Reset input buffer */
    _inputBuffer[ 0 ] = '\0';

    _cmdHistoryPtr = NULL;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Enable user input
 * 
 */
void Console::enableInput() {
    this->resetInput();
    this->displayPrompt();

    _inputEnabled = true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Inhibit user input
 * 
 */
void Console::disableInput() {
    _inputEnabled = false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check if the input buffer contains the specified command
 *
 * @param   command         Command name to find
 * @param   hasParameter    TRUE if the command expects a parameter, 
 *                          FALSE otherwise
 *
 * @return  TRUE if command name is matching, FALSE otherwise
 * 
 */
bool Console::matchCommandName( const char *command, bool hasParameter ) {
    _inputParameter = NULL;

    /* If no parameter, match the entire input buffer */
    if( hasParameter == false ) {
        return strcasecmp_P( _inputBuffer, command ) == 0;
    }

    /* Otherwise, match only the length of the command */
    if( strncasecmp_P( _inputBuffer, command, strlen_P( command )) != 0 ) {
        return false;
    }


    _inputParameter = _inputBuffer + strlen_P( command );

    /* If next character after command is a null character, command
       still match but no parameters was given */
    if( *_inputParameter == 0x00 ) {
        return true;
    }

    /* if next character after command is not a space, the match 
       is invalid. */
    if( isspace( *_inputParameter ) == false ) {
        return false;
    }

    /* Parameter was given and properly spaced from the command */
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns the pointer to the start of the parameter in the input buffer
 *
 * @return  Pointer to the parameter start, 0 if none is found.
 * 
 */
char* Console::getInputParameter() {
    if( _inputParameter == NULL ) {
        return 0;
    }

    while( isspace( *_inputParameter )) _inputParameter++;

    if( *_inputParameter == 0x00 ) {
        return 0;
    }

    return _inputParameter;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Remove leading and trailing white spaces from the input buffer
 * 
 */
void Console::trimInput() {
    uint8_t i;
    uint8_t length = strlen( _inputBuffer );

    if( length == 0 ) {
        return;
    }

    char *begin = _inputBuffer;
    while( isspace( *begin )) begin++;

    char *end = _inputBuffer + length - 1;
    while( isspace( *end ) && end >= begin ) {
        end--;
    } 

    length = end + 1 - begin;

    if (begin > _inputBuffer) {
        memcpy( _inputBuffer, begin, length );
    } 

    _inputBuffer[ length ] = '\0';
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Process incomming character from the serial port and echo the 
 *          input back on the serial port accordingly.
 *
 * @return  TRUE if end of line is detected, FALSE otherwise
 * 
 */
bool Console::processInput() {

    uint8_t length = strlen( _inputBuffer );
    char ch = Serial.read();


    if( _inputEnabled == false ) {
        return false;
    }

    if( _escapeSequence > 0 ) {

        this->processControlSequence( ch );
        return false;
    }

    /* Printable character */
    if( isprint( ch )) {

        /* If limit has been reach, discard the character */
        if( length >= _inputBufferLimit ) {
            return false;
        }

        _inputBuffer[ length ] = ch;
        _inputBuffer[ length + 1 ] = '\0';

        this->print( _inputHidden == true ? '*' : ch );

    /* Backspace */
    } else if( ch == '\b' || ch == 0x7f ) {

        if( length > 0 ) {
            this->print( '\b' );
            this->print( 0x20 );
            this->print( '\b' );
            
            _inputBuffer[ length - 1 ] = '\0';
        }

    /* Enter */
    } else if( ch == '\r' || ch == '\n' ) {
        if( Serial.peek() == '\n' ) {
            Serial.read();
        }

        _inputBuffer[ length ] = '\0';

        Serial.println();

        return true;

    /* Control sequence start */
    } else if( ch == '\033' ) {
        _escapeSequence = 1;
    }


    return false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Display an input prompt
 * 
 */
void Console::displayPrompt() {

    this->printf_P( S_CONSOLE_PROMPT, g_config.network.hostname );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Move the history buffer cursor forward or back and copy the
 *          stored command to the input buffer
 * 
 * @param   forward    TRUE to move forward in the history buffer, FALSE
 *                     otherwise.
 * 
 */
void Console::readHistoryBuffer( bool forward ) {

    char *pos = _cmdHistoryPtr;

    if( forward ) {

        if( pos == NULL ) {
            pos = _historyBuffer;

        } else {

            while( pos++ < _historyBuffer + CMD_HISTORY_BUFFER_LENGTH ) {

                if( *pos == '\0' ) {
                    pos++;
                    break;
                }
            }
        }

        if( *pos == '\0' ) {
            return;
        }
        
    } else {

        if( pos == NULL ) {
            return;
        }
        
        if( pos == _historyBuffer ) {

            sendControlSequence( CTRL_SEQ_ERASE_LINE );
            sendControlSequence( CTRL_SEQ_CURSOR_COLUMN, 1 );
            this->displayPrompt();
            this->resetInput();

            return;
        }

        /* Find the start of the previous command in the history buffer */
        for( pos -= 2; pos > _historyBuffer; pos-- ) {

            if( *pos == '\0' ) {
                pos++;
                break;
            }
        }
    }
    

    sendControlSequence( CTRL_SEQ_ERASE_LINE );
    sendControlSequence( CTRL_SEQ_CURSOR_COLUMN, 1 );
    this->displayPrompt();
    this->resetInput();

    strcpy( _inputBuffer, pos );
    _cmdHistoryPtr = pos;

    this->print( _inputBuffer );
}



/*! ------------------------------------------------------------------------
 *
 * @brief   Store the current command in the input buffer to the start of 
 *          the history buffer
 * 
 */
void Console::writeHistoryBuffer() {

    /* Don't add empty lines in the history buffer */
    if( strlen( _inputBuffer ) == 0 ) {
        return;
    }

    /* Don't add if the new item is a duplicate of the 
       first one in the history buffer */
    if( strcmp( _inputBuffer, _historyBuffer ) == 0 ) {
        return;
    }

    /* Make space for the new item by shifting the content of the history 
       buffer to the right */
    memmove( _historyBuffer + strlen( _inputBuffer ) + 1, _historyBuffer, 
             CMD_HISTORY_BUFFER_LENGTH - strlen( _inputBuffer ));

    /* Remove the last command in the history buffer if it does not 
       fit entierly*/
    char *ptr = _historyBuffer + CMD_HISTORY_BUFFER_LENGTH ;
    while( *ptr != '\0' && ptr >= _historyBuffer ) {
        *ptr-- = '\0';
    }

    /* Insert the new command in the buffer */
    strcpy( _historyBuffer, _inputBuffer );

    /* Reset the history buffer pointer to the first item */
    _cmdHistoryPtr = NULL;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Scan the input buffer for known commands and run them accordingly.
 * 
 */
void Console::parseCommand() {
    bool started = false;
  
    /* 'help' command */
    if( this->matchCommandName( S_COMMAND_HELP ) == true ) {
        this->startTaskPrintHelp();
        started = true;

    /* 'reboot' command */
    } else if( this->matchCommandName( S_COMMAND_REBOOT ) == true ) {

        this->clearScreen();
        g_power.reboot();

    /* 'net restart' command */
    } else if( this->matchCommandName( S_COMMAND_NET_RESTART ) == true ) {
        started = this->startTaskNetRestart();

    /* 'net start' command */
    } else if( this->matchCommandName( S_COMMAND_NET_START ) == true ) {
        started = this->startTaskNetStart();

    /* 'net stop' command */
    } else if( this->matchCommandName( S_COMMAND_NET_STOP ) == true ) {
        started = this->startTaskNetStop();

    /* 'net status' command */
    } else if( this->matchCommandName( S_COMMAND_NET_STATUS ) == true ) {
        this->printNetStatus();
        this->println();

    /* 'nslookup' command */
    } else if( this->matchCommandName( S_COMMAND_NET_NSLOOKUP, true ) == true ) {
        started = this->startTaskNslookup();
    
    /* 'ping' command */
    } else if( this->matchCommandName( S_COMMAND_PING, true ) == true || 
               this->matchCommandName( S_COMMAND_NET_PING, true ) == true ) {

        started = this->startTaskPing();
    
    /* 'net config' command */
    } else if( this->matchCommandName( S_COMMAND_NET_CONFIG, false ) == true ) {
        started = this->startTaskNetworkConfig();

    /* 'date' command */
    } else if( this->matchCommandName( S_COMMAND_DATE, false ) == true ) {
        this->runTaskPrintDateTime();
        this->println();

    /* 'set date' command */
    } else if( this->matchCommandName( S_COMMAND_SET_DATE, false ) == true ||
               this->matchCommandName( S_COMMAND_SET_TIME, false ) == true ) {

        started = this->startTaskSetDate();

    /* 'set timezone' and 'tz set' command */
    } else if( this->matchCommandName( S_COMMAND_SET_TIMEZONE, true ) == true ||
               this->matchCommandName( S_COMMAND_TZ_SET, true ) == true ) {

        started = this->startTaskSetTimeZone();

    /* 'tz info' command */
    } else if( this->matchCommandName( S_COMMAND_TZ_INFO, false ) == true ) {
        this->showTimezoneInfo();
        this->println();

    /* 'config backup' command */
    } else if( this->matchCommandName( S_COMMAND_SETTING_BACKUP, true ) == true ) {
        started = this->startTaskConfigBackup();

    /* 'config restore' command */
    } else if( this->matchCommandName( S_COMMAND_SETTING_RESTORE, true ) == true ) {
        started = this->startTaskConfigRestore();

    /* 'factory reset' command */
    } else if( this->matchCommandName( S_COMMAND_FACTORY_RESET, false ) == true ) {
        started = this->startTaskFactoryReset();

    /* 'ntp sync' command */
    } else if( this->matchCommandName( S_COMMAND_NTP_SYNC, false ) == true ) {
        started = this->startTaskNtpSync();

    /* 'ntp status' command */
    } else if( this->matchCommandName( S_COMMAND_NTP_STATUS, false ) == true ) {
        g_ntp.printNTPStatus();
        this->println();

    /* 'exit' command */
    } else if( this->matchCommandName( S_COMMAND_EXIT, false ) == true ) {
        this->resetConsole();

    /* 'clear' command */
    } else if( this->matchCommandName( S_COMMAND_CLEAR, false ) == true ) {
        this->clearScreen();

    /* No command entered, display the prompt again */
    } else if( strlen( _inputBuffer ) == 0 ) {

    /* Unknown command */
    } else {
        this->println_P( S_CONSOLE_INVALID_COMMAND );
        this->println();
    }

    /* If command was not executed, display the prompt on a new line
       and wait for another command  */
    if( started == false ) {

        if( this->getTaskError() != TASK_SUCCESS ) {
            this->printErrorMessage( this->getTaskError() );
            this->println();

            this->clearError();
        }

        this->displayPrompt();
    }

    /* Reset the input buffer for the next command */    
    this->resetInput();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run current tasks
 * 
 */
void Console::runTask() {

    if( this->getCurrentTask() == TASK_NONE ) {

        /* Reset input buffer limit to it's maximum */
        _inputBufferLimit = INPUT_BUFFER_LENGTH;
        _inputHidden = false;

        /* If no task is running, process the input buffer */
        if( this->processInput() == true ) {

            /* Remove unnecessary spaces */
            this->trimInput();

            /* Store the command in the history buffer */
            this->writeHistoryBuffer();
            
            /* If new line is found, parse the line */
            this->parseCommand();
        }

    } else {

        /* Task is currently running, call the task runner */
        switch( this->getCurrentTask() ) {

            case TASK_CONSOLE_PRINT_HELP:
                this->runTaskPrintHelp();
                break;

            case TASK_CONSOLE_NET_START:
            case TASK_CONSOLE_NET_RESTART:
                this->runTaskNetRestart();
                break;

            case TASK_CONSOLE_NET_STOP:
                this->runTaskNetStop();
                break;
            
            case TASK_CONSOLE_NET_NSLOOKUP:
                this->runTaskNsLookup();
                break;

            case TASK_CONSOLE_NET_PING:
                this->runTaskPing();
                break;

            case TASK_CONSOLE_NET_CONFIG:
                this->runTaskNetworkConfig();
                break;

            case TASK_CONSOLE_SET_TZ:
                this->runTaskSetTimeZone();
                break;

            case TASK_CONSOLE_SET_DATE:
                this->runTaskSetDate();
                break;

            case TASK_CONSOLE_CONFIG_BACKUP:
                this->runTaskConfigBackup();
                break;

            case TASK_CONSOLE_CONFIG_RESTORE:
                this->runTaskConfigRestore();
                break;

            case TASK_CONSOLE_FACTORY_RESET:
                this->runTaskFactoryReset();
                break;

            case TASK_CONSOLE_NTP_SYNC:
                this->runTaskNtpSync();
                break;
        }

        /* If task is done, displays the prompt and reset input buffer */
        if( this->getCurrentTask() == TASK_NONE ) {

            if( this->getTaskError() != TASK_SUCCESS ) {

                this->printErrorMessage( this->getTaskError() );
                this->clearError();
            }
            
            this->println();
            this->displayPrompt();

            this->resetInput();
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   prints the date and time on the console.
 * 
 * @param   dt          dateTime object holding the date/time to print
 * @param   timezone    Timezone abbreviation
 * @param   ms          Milliseconds to display. Set to -1 to not display 
 *                      the milliseconds
 * 
 */
void Console::printDateTime( DateTime *dt, const char *timezone, int16_t ms ) {

    if( ms >= 0 ) {
        this->printf_P( PSTR( "%S %S %d %02d:%02d:%02d.%03d %S %d" ), 
                        getDayName( dt->dow(), true ),
                        getMonthName( dt->month(), true ),
                        dt->day(),
                        dt->hour(),
                        dt->minute(),
                        dt->second(),
                        ms,
                        timezone,
                        dt->year());
    } else {
        this->printf_P( PSTR( "%S %S %d %02d:%02d:%02d %S %d" ), 
                        getDayName( dt->dow(), true ),
                        getMonthName( dt->month(), true ),
                        dt->day(),
                        dt->hour(),
                        dt->minute(),
                        dt->second(),
                        timezone,
                        dt->year());
    }
    
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Print the error message of the last command.
 *
 */
void Console::printErrorMessage( int8_t error ) {

    switch( error ) {

        case TASK_SUCCESS:
            this->println_P( S_CONSOLE_SUCCESS );
            break;

        case ERR_CONSOLE_INVALID_TIMEZONE:
            this->println_P( S_CONSOLE_TIME_INVALID_TZ );
            break;

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

        case ERR_WIFI_ALREADY_CONNECTED:
            this->println_P( S_CONSOLE_NET_ALREADY_CONN );
            break;

        case ERR_WIFI_PING_ERROR:
            this->println_P( S_CONSOLE_NET_PING_ERROR );
            break;

        case ERR_WIFI_PING_TIMEOUT:
            this->println_P( S_CONSOLE_NET_PING_TIMEOUT );
            break;

        case ERR_WIFI_INVALID_HOSTNAME:
            this->println_P( S_CONSOLE_NET_INVALID_HOST );
            break;

        case ERR_WIFI_UNKNOWN_HOSTNAME:
            this->println_P( S_CONSOLE_NET_PING_UNKNOWN );
            break;

        case ERR_WIFI_NETWORK_UNREACHABLE:
            this->println_P( S_CONSOLE_NET_PING_UNREACH );
            break;

        case ERR_NTPCLIENT_UNKNOWN_HOSTNAME:
            this->println_P( S_CONSOLE_NTP_UNKNOWN_HOST );
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