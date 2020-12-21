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


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  None
  */
Console::Console() {

    memset( _logbuffer, 0, 1024 );
    memset( _inputbuffer, 0, INPUT_BUFFER_LENGTH );


    /* Initialize IPrint interface */
    this->_initPrint();
}


/*--------------------------------------------------------------------------
 *
 * IPrint interface callback for printing a single character. Sends the 
 * output to the serial port.
 *
 * Arguments
 * ---------
 *  - c : Character to print
 *
 * Returns : Number of bytes written
 */
uint8_t Console::_print( char c ) {
    return Serial.write( c );
}


/*--------------------------------------------------------------------------
 *
 * Initialize the console
 *
 * Arguments
 * ---------
 *  - baud : Speed of the serial port
 *
 * Returns : Nothing
 */
void Console::begin( unsigned long baud ) {

    Serial.begin( baud );
    while (!Serial);
    

    this->println();
    this->println_P( S_CONSOLE_WELCOME_1 );
    this->println_P( S_CONSOLE_WELCOME_2 );
    this->println();

    this->resetInput();
}


/*--------------------------------------------------------------------------
 *
 * Discard the user input from the serial port
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::resetInput() {

    /* Reset input buffer */
    _inputlength = 0;
    _inputbuffer[ 0 ] = 0;
}


/*--------------------------------------------------------------------------
 *
 * Enable user input
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::enableInput() {
    this->resetInput();
    this->displayPrompt();

    _inputenabled = true;
}


/*--------------------------------------------------------------------------
 *
 * Inhibit user input
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::disableInput() {
    _inputenabled = false;
}


/*--------------------------------------------------------------------------
 *
 * Check if the input buffer contains the specified command
 *
 * Arguments
 * ---------
 *  - hasParameter : TRUE if the command expects a parameter, false otherwise
 *
 * Returns : TRUE if command name is matching, FALSE otherwise
 */
bool Console::matchCommandName( const char *command, bool hasParameter ) {
    _inputParameter = NULL;

    /* If no parameter, match the entire input buffer */
    if( hasParameter == false ) {
        return strcasecmp_P( _inputbuffer, command ) == 0;
    }

    /* Otherwise, match only the length of the command */
    if( strncasecmp_P( _inputbuffer, command, strlen_P( command )) != 0 ) {
        return false;
    }


    _inputParameter = _inputbuffer + strlen_P( command );

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


/*--------------------------------------------------------------------------
 *
 * Returns the pointer to the start of the parameter in the input buffer
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Pointer to the parameter start, 0 if none is found.
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


/*--------------------------------------------------------------------------
 *
 * Remove leading and trailing white spaces from the input buffer
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::trimInput() {
    uint8_t i;

    if( _inputlength == 0 ) {
        return;
    }

	char *begin = _inputbuffer;
	while( isspace( *begin )) begin++;

	char *end = _inputbuffer + _inputlength - 1;
	while( isspace( *end ) && end >= begin ) {
        end--;
    } 

	_inputlength = end + 1 - begin;

	if (begin > _inputbuffer) {
        memcpy( _inputbuffer, begin, _inputlength );
    } 

	_inputbuffer[ _inputlength ] = 0;
}


/*--------------------------------------------------------------------------
 *
 * Process incomming character from the serial port and echo the 
 * input back on the serial port accordingly.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if end of line is detected, FALSE otherwise
 */
bool Console::processInput() {
    
    while( Serial.available() > 0 ) {

        char ch = Serial.read();

        if( _inputenabled == false ) {
            continue;
        }

        /* Printable character */
        if( isprint( ch )) {

            _inputbuffer[ _inputlength++ ] = ch;
            Serial.write( ch );

        /* Backspace */
        } else if( ch == '\b' || ch == 0x7f ) {

            if( _inputlength > 0 ) {
                Serial.write( '\b' );
                Serial.write( 0x20 );
                Serial.write( '\b' );
                _inputlength--;
            }

        /* Enter */
        } else if( ch == '\r' || ch == '\n' ) {
            if( Serial.peek() == '\n' ) {
                Serial.read();
            }

            _inputbuffer[ _inputlength ] = 0;

            Serial.println();

            return true;
        } 

            
        /* Reached the end of buffer, reject input */
        if( _inputlength >= INPUT_BUFFER_LENGTH ) {
            _inputlength = 0;
            return false;
        }
    }

    return false;
}


/*--------------------------------------------------------------------------
 *
 * Display an input prompt
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::displayPrompt() {
    this->print_P( S_CONSOLE_PROMPT );
}


/*--------------------------------------------------------------------------
 *
 * Scan the input buffer for known commands and run them accordingly.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::parseCommand() {
    bool started;

    if( this->isBusy() ) {
        this->println_P( S_CONSOLE_BUSY );
        this->println();

        this->resetInput();

        return;
    }

    this->trimInput();

    if( this->matchCommandName( S_COMMAND_HELP ) == true ) {
        started = this->startTaskPrintHelp();

    } else if( this->matchCommandName( S_COMMAND_REBOOT ) == true ) {
        g_power.reboot();

    } else if( this->matchCommandName( S_COMMAND_NET_RESTART ) == true ) {
        started = this->startTaskNetRestart();

    } else if( this->matchCommandName( S_COMMAND_NET_STATUS ) == true ) {
        started = this->startTaskNetStatus();

    } else if( this->matchCommandName( S_COMMAND_NET_NSLOOKUP, true ) == true ) {
        started = this->startTaskNslookup();
    
    } else if( this->matchCommandName( S_COMMAND_NET_PING, true ) == true ) {
        started = this->startTaskPing();
    
    } else if( strlen( _inputbuffer ) == 0 ) {
        started = false;

    } else {
        this->println_P( S_CONSOLE_INVALID_COMMAND );
        this->println();

        started = false;
    }

    if( started == false ) {
        this->displayPrompt();
    }
    this->resetInput();
}


/*--------------------------------------------------------------------------
 *
 * Run current tasks
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::runTask() {

    switch( this->getCurrentTask() ) {

        case TASK_CONSOLE_PRINT_HELP:
            this->runTaskPrintHelp();
            break;

        case TASK_CONSOLE_NET_RESTART:
            this->runTaskNetRestart();
            break;

        case TASK_CONSOLE_NET_STATUS:
            this->runTaskNetStatus();
            break;
        
        case TASK_CONSOLE_NET_NSLOOKUP:
            this->runTaskNsLookup();
            break;

        case TASK_CONSOLE_NET_PING:
            this->runTaskPing();
            break;

        default:
            if( this->processInput() == true ) {
                this->parseCommand();
            }
            break;
    }
}


/*--------------------------------------------------------------------------
 *
 * Starts a new task
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Currently running task ID
 */
uint8_t Console::startTask( uint8_t task ) {

    return ITask::startTask( task );

    _timerCommandStart = millis();
}


/*--------------------------------------------------------------------------
 *
 * Remove leading and trailing white spaces from the input buffer
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Console::endTask( int error ) {
    ITask::endTask( error );

    this->println();
    this->displayPrompt();

    this->resetInput();
}