//******************************************************************************
//
// Project : Alarm Clock V3
// File    : lib/itask/itask.h
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
#ifndef I_TASK_H
#define I_TASK_H

#include <Arduino.h>



#define TASK_NONE         0
#define TASK_SUCCESS      0
#define ERR_TASK_FAIL     (-1)
#define ERR_TASK_TIMEOUT  (-2)


/*******************************************************************************
 *
 * @brief   Task manager interface
 * 
 *******************************************************************************/
class ITask {

  public:
    bool isBusy();
    virtual void runTasks();
    uint8_t getCurrentTask();
    int getTaskError();
    void clearTaskError();
    unsigned long getTaskRunningTime();
    void resetTaskTimer();


  private:
    uint8_t _currentTask = TASK_NONE;   /* Current running task ID */
    unsigned long _timerTaskStart = 0;  /* Timestamp when current task started */
    int _taskError = 0;                 /* Last running task error */


  protected:
    uint8_t startTask( uint8_t task, bool force = true );
    void endTask( int error = TASK_SUCCESS );
    void setTaskError( int error );
};

#endif /* I_TASK_H */