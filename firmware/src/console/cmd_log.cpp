//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/cmd_log.cpp
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
#include "services/logger.h"
#include <time.h>
#include <timezone.h>
#include <tzdata.h>



/*******************************************************************************
 * 
 * @brief   Starts the print logs command task.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
void ConsoleBase::beginTaskPrintLogs() {

    _taskIndex = g_log.getFirstIndex();

    this->startTask( TASK_CONSOLE_PRINT_LOGS );
    this->println();
}


/*******************************************************************************
 * 
 * @brief   Print the next line of the log to the console
 * 
 */
void ConsoleBase::runTaskPrintLogs() {

    if( g_log.printLogEntry( this, _taskIndex ) == false ) {
        this->endTask( TASK_SUCCESS );
        return;
    }

    _taskIndex++;

    if( _taskIndex == g_log.getLastIndex() + 1 ) {
        this->endTask( TASK_SUCCESS );
        return;
    }

    if( _taskIndex > MAX_LOG_ENTRIES - 1 ) {
        _taskIndex = 0;
    }
}