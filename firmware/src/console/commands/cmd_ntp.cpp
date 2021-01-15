//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/commands/cmd_net.cpp
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
#include "../../services/ntpclient.h"


/*! ------------------------------------------------------------------------
 * 
 * @brief   Starts the 'ntp sync' command task
 * 
 * @return  TRUE if successful, FALSE otherwise
 * 
 */
bool Console::startTaskNtpSync() {

    this->startTask( TASK_CONSOLE_NTP_SYNC );

    if( g_ntp.sync( true ) == false ) {

        this->endTask( g_ntp.getTaskError() );
        return false;
    }
    
    return true;
}


/*! ------------------------------------------------------------------------
 * 
 * @brief   Monitor the 'ntp sync' task
 * 
 */
void Console::runTaskNtpSync() {

    if( g_ntp.isBusy() == true ) {
        return;
    }
   
    this->endTask( g_ntp.getTaskError() );
}