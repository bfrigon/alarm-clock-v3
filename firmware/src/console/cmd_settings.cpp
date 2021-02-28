//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/cmd_settings.cpp
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

#include <config.h>
#include <task_errors.h>
#include <drivers/power.h>

#include "console_base.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'config backup' command task
 *
 * @return  TRUE if successful, FALSE if another task is already running.
 *           
 */
bool ConsoleBase::openCommandConfigBackup() {
    _taskIndex = 0;
    

    this->startTask( TASK_CONSOLE_CONFIG_BACKUP );
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Monitor the 'config backup' command task. Display prompts and validate 
 *          responses required before executing the task.
 *           
 */
void ConsoleBase::runCommandConfigBackup() {

    /* Even index display the prompt, odd index process user input */
    if( _taskIndex % 2 ) {

        if( this->processInput() == false ) {
            return;
        }
    }

    /* Check for a filename in the command parameters */
    if( _taskIndex == 0 ) {
        char *filename;
        filename = this->getInputParameter();

        if( filename != 0 ) {
            memmove( _inputBuffer, filename, strlen( filename ) + 1);

            /* Skip filename prompt if a filename is provided in the command parameter */
            _taskIndex = 1;
        }
    }

    switch( _taskIndex++ ) {

        /* Display filename prompt */
        case 0:
            _inputBufferLimit = INPUT_BUFFER_LENGTH - 4;

            this->print_P( S_CONSOLE_CFG_SAVE_FILENAME );
            break;

        /* Validate filename prompt */
        case 1:

            /* If filename is empty, use the default one */
            if( strlen( _inputBuffer ) == 0 ) {
                strcpy( _inputBuffer, CONFIG_BACKUP_FILENAME );
            }

            /* Start the backup task */
            if( g_config.startBackup( _inputBuffer, false ) == false ) {

                if( g_config.getTaskError() == ERR_CONFIG_FILE_EXISTS ) {

                    /* Move filename to an unused area of the input buffer */
                    memmove( _inputBuffer + 4, _inputBuffer, strlen( _inputBuffer ) + 1 );

                    /* Continue on step 2, display overwrite prompt */
                    _taskIndex = 2;
                    
                } else {

                    this->endTask( g_config.getTaskError() );
                    return;
                }

            } else {

                /* Backup started, skip overwrite prompt */
                _taskIndex = 4;

                this->println();
                this->println_P( S_CONSOLE_CFG_SAVING );
            }

            break;


        /* Display overwrite prompt if file exists */
        case 2:
            _inputBufferLimit = 1;

            this->printf_P( S_CONSOLE_CFG_FILE_EXISTS, _inputBuffer + 4 );
            break;

        /* Validate overwrite file prompt */
        case 3:
            if( tolower( _inputBuffer[ 0 ] ) == 'y' ) {
                
                /* Start backup */
                g_config.startBackup( _inputBuffer + 4, true );

                this->println();
                this->println_P( S_CONSOLE_CFG_SAVING );

                _taskIndex = 4;

            } else if ( tolower( _inputBuffer[ 0 ] ) == 'n' ) {
                this->endTask( TASK_SUCCESS );
                return;

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );
                this->println();

                /* try again */
                _taskIndex = 2;
            }
            break;

        /* Monitor the task */
        default:
            _taskIndex = 4;

            /* Check if settings backup task is done */
            if( g_config.getCurrentTask() == TASK_NONE ) {

                if( g_config.getTaskError() == TASK_SUCCESS) {

                    this->println_P( S_STATUS_DONE );
                    this->endTask( TASK_SUCCESS );

                } else {
                    this->endTask( g_config.getTaskError() );
                }
            }

            return;
    }

    this->resetInput();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'config restore' command task
 *
 * @return  TRUE if successful, FALSE if another task is already running.
 *           
 */
bool ConsoleBase::openCommandConfigRestore() {
    _taskIndex = 0;

    this->startTask( TASK_CONSOLE_CONFIG_RESTORE );
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Monitor the 'config restore' command task. Display prompts and validate 
 *          responses required before executing the task.
 *           
 */
void ConsoleBase::runCommandConfigRestore() {

    /* Even index display the prompt, odd index process user input */
    if( _taskIndex % 2 ) {

        if( this->processInput() == false ) {
            return;
        }
    }

    /* Check for a filename in the command parameters */
    if( _taskIndex == 0 ) {
        char *filename;
        filename = this->getInputParameter();

        if( filename != 0 ) {
            memmove( _inputBuffer, filename, strlen( filename ) + 1);

            /* Skip filename prompt if a filename is provided in the command parameter */
            _taskIndex = 1;
        }
    }

    switch( _taskIndex++ ) {

        /* Display filename prompt */
        case 0:
            _inputBufferLimit = INPUT_BUFFER_LENGTH - 4;

            this->print_P( S_CONSOLE_CFG_LOAD_FILENAME );
            break;

        /* Validate filename prompt */
        case 1:

            /* If filename is empty, use the default one */
            if( strlen( _inputBuffer ) == 0 ) {
                strcpy( _inputBuffer, CONFIG_BACKUP_FILENAME );
            }

            /* Move filename to an unused area of the input buffer */
            memmove( _inputBuffer + 4, _inputBuffer, strlen( _inputBuffer ) + 1 );

            this->println();
            this->println_P( S_CONSOLE_CFG_RESTORE_MSG );
            break;

        /* Display overwrite prompt if file exists */
        case 2:
            _inputBufferLimit = 1;

            this->print_P( S_CONSOLE_CONTINUE );
            break;

        /* Validate overwrite file prompt */
        case 3:
            if( tolower( _inputBuffer[ 0 ] ) == 'y' ) {
                
                /* Start the restore task */
                if( g_config.startRestore( _inputBuffer + 4 ) == false ) {

                    this->endTask( g_config.getTaskError() );
                    return;
                }

                this->println();
                this->println_P( S_CONSOLE_CFG_RESTORING );

                _taskIndex = 4;

            } else if ( tolower( _inputBuffer[ 0 ] ) == 'n' ) {
                this->endTask( TASK_SUCCESS );
                return;

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );
                this->println();

                /* try again */
                _taskIndex = 2;
            }
            break;

        /* Monitor the task */
        default:
            _taskIndex = 4;

            /* Check if settings backup task is done */
            if( g_config.getCurrentTask() == TASK_NONE ) {

                if( g_config.getTaskError() == TASK_SUCCESS) {

                    this->println_P( S_STATUS_DONE );
                    this->endTask( TASK_SUCCESS );

                } else {
                    
                    this->endTask( g_config.getTaskError() );
                }
            }

            return;
    }

    this->resetInput();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the 'factory reset' command task
 *
 * @return  TRUE if successful, FALSE if another task is already running.
 *           
 */
bool ConsoleBase::openCommandFactoryReset() {
    _taskIndex = 0;

    this->println();
    this->println_P( S_CONSOLE_CFG_RESET_MSG );

    this->startTask( TASK_CONSOLE_FACTORY_RESET );
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Monitor the 'factory reset' command task. Display prompts and validate 
 *          responses required before executing the task.
 *           
 */
void ConsoleBase::runCommandFactoryReset() {
    
    /* Even index display the prompt, odd index process user input */
    if( _taskIndex % 2 ) {

        if( this->processInput() == false ) {
            return;
        }
    }

    switch( _taskIndex++ ) {

        /* Display do you want to continue prompt */
        case 0:
            _inputBufferLimit = 1;

            this->print_P( S_CONSOLE_CONTINUE );
            break;

        /* Validate overwrite file prompt */
        case 1:
            if( tolower( _inputBuffer[ 0 ] ) == 'y' ) {

                this->println();
                this->println_P( S_CONSOLE_CFG_RESETTING );
                this->println();
                
                /* Factory reset */
                g_config.formatEeprom();
                g_power.reboot();
                return;

            } else if ( tolower( _inputBuffer[ 0 ] ) == 'n' ) {
                this->endTask( TASK_SUCCESS );
                return;

            } else {
                this->println_P( S_CONSOLE_INVALID_INPUT_BOOL );
                this->println();

                /* try again */
                _taskIndex = 0;
            }
            break;
    }

    this->resetInput();

}