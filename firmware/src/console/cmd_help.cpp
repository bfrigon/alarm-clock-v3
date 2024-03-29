//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/cmd_help.cpp
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

#include "console_base.h"



/*******************************************************************************
 * 
 * @brief   Starts the 'help' command task
 * 
 */
void ConsoleBase::beginTaskPrintHelp() {
    _taskIndex = 0;
    
    this->startTask( TASK_CONSOLE_PRINT_HELP );
    this->println();
}


/*******************************************************************************
 * 
 * @brief   Run the 'help' command task
 * 
 */
void ConsoleBase::runTaskPrintHelp() {
    this->print_P( (const char *)pgm_read_word( &( S_COMMANDS[ _taskIndex ])), 16, TEXT_ALIGN_LEFT );
    this->print_P( (const char *)pgm_read_word( &( S_HELP_COMMANDS[ _taskIndex ])));
    this->println();

    _taskIndex++;

    if( _taskIndex >= CONSOLE_HELP_MENU_ITEMS ) {
        this->endTask( TASK_SUCCESS );
    }
}