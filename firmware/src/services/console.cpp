//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/console.cpp
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



/*******************************************************************************
 *
 * @brief   IPrint interface callback for printing a single character. Sends the 
 *          output to the serial port.
 * 
 * @param   c   character to print
 *
 * @return  Number of bytes written
 */
size_t Console::_print( char c ) {
    return Serial.write( c );
}


/*******************************************************************************
 *
 * @brief   Reads the next character in the receive buffer.
 * 
 * @return  Character or -1 if an error occured.
 * 
 */
int Console::_read() {
    return Serial.read();
}


/*******************************************************************************
 *
 * @brief   Reads the next character in the receive buffer without 
 *          discarding it.
 * 
 * @return  Character read or -1 if no character is available
 * 
 */
int Console::_peek() {
    return Serial.peek();
}


/*******************************************************************************
 *
 * @brief   Get the number of bytes (characters) available for reading from 
 *          the buffer.
 * 
 * @return  Number of bytes available
 * 
 */
int Console::_available() {
    return Serial.available();
}


/*******************************************************************************
 *
 * @brief   Initialize the serial console.
 *
 * @param   baud    Speed of the serial port
 * 
 */
void Console::begin( unsigned long baud ) {

    Serial.begin( baud );

    /* Wait until the serial port is ready */
    while( !Serial );
}


/*******************************************************************************
 *
 * @brief   Terminate the console session.
 *
 * @param   timeout    TRUE when the session has timed out.
 * 
 */
void Console::exitConsole( bool timeout ) {

    /* Cannot exit from a serial console session, clear the screen */
    this->resetConsole();
}


/*******************************************************************************
 *
 * @brief   Clear the screen and display the welcome message.
 * 
 */
void Console::resetConsole() {

    /* Stop currently running console commands. */
    this->endTask();    

    this->clearScreen();

    this->println_P( S_CONSOLE_WELCOME );
    this->println();

    this->resetInput();
    this->displayPrompt();
}


/*******************************************************************************
 *
 * @brief   Run console tasks
 * 
 */
void Console::runTasks() {
    ConsoleBase::runTasks();
}
