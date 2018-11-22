//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/task.h
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
#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

#define TASK_NONE     0
#define TASK_SUCCESS  0


class Task {

  public:

    bool isBusy();
    virtual void runTask();
    uint8_t getCurrentTask();
    int getTaskError();

  private:
    uint8_t _currentTask = TASK_NONE;
    int _taskError = 0;

  protected:
    uint8_t startTask( uint8_t task );
    void endTask( int error = TASK_SUCCESS );
    void setTaskError( int error );
};

#endif /* TASK_H */


