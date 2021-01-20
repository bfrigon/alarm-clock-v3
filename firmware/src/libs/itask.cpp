//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/itask.cpp
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
#include "itask.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Checks if a task is currently running.
 *
 * @return  TRUE if task running, FALSE otherwise.
 * 
 */
bool ITask::isBusy() {
    return this->_currentTask != TASK_NONE;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the current running task ID
 *
 * @return  Task ID
 * 
 */
uint8_t ITask::getCurrentTask() {
    return this->_currentTask;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the previous task exit error code.
 *
 * @return  Error code
 * 
 */
int ITask::getTaskError() {
    return this->_taskError;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the time in milliseconds the task has been running.
 *
 * @return  Time elapsed
 * 
 */
unsigned long ITask::getTaskRunningTime() {
    return millis() - this->_timerTaskStart;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts a new task.
 *
 * @param   task     Task ID
 * @param   force    Cancel the previous task if one is currently running.
 *
 * @return  Current running task ID. Will return the new task ID or the
 *          previous task ID if one is still running.
 * 
 */
uint8_t ITask::startTask( uint8_t task, bool force ) {

    if( this->_currentTask != TASK_NONE && force == false ) {
        return this->_currentTask;
    }

    this->_timerTaskStart = millis();
    this->_currentTask = task;
    this->_taskError = TASK_SUCCESS;
    return task;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Ends the current task
 *
 * @param   error    Error code 
 * 
 */
void ITask::endTask( int error ) {
    
    this->_currentTask = TASK_NONE;
    this->_timerTaskStart = 0;

    if( this->_taskError == TASK_SUCCESS ) {
        this->_taskError = error;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the error code of the running task
 *
 * @param   error    Error code 
 * 
 */
void ITask::setTaskError( int error ) {
    this->_taskError = error;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Clear the error code of the running task
 * 
 */
void ITask::clearTaskError() {
    this->_taskError = TASK_SUCCESS;
}