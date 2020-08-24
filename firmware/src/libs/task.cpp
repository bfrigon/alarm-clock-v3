//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/task.cpp
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
#include "task.h"


bool Task::isBusy() {
    return this->_currentTask != TASK_NONE;
}

uint8_t Task::getCurrentTask() {
    return this->_currentTask;
}

int Task::getTaskError() {
    return this->_taskError;
}


uint8_t Task::startTask( uint8_t task ) {

    if( this->_currentTask != TASK_NONE ) {
        return this->_currentTask;
    }

    this->_currentTask = task;
    this->_taskError = TASK_SUCCESS;
    return task;
}

void Task::endTask( int error ) {
    
    this->_currentTask = TASK_NONE;

    if( this->_taskError == TASK_SUCCESS ) {
        this->_taskError = error;
    }
}

void Task::setTaskError( int error ) {
    this->_taskError = error;
}