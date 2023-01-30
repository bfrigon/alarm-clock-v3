//******************************************************************************
//
// Project : Alarm Clock V3
// File    : power.h
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
#ifndef POWER_H
#define POWER_H

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>


#define DELAY_BEFORE_SUSPEND        30000

#define POWER_MODE_NORMAL           0
#define POWER_MODE_ON_BATTERY       1
#define POWER_MODE_SUSPEND          2



/*******************************************************************************
 *
 * @brief   Power management class
 * 
 *******************************************************************************/
class Power {

  public:
    Power( int8_t pin_onbatt, int8_t pin_sysoff, int8_t pin_cfgrst = -1 );
    void begin();
    uint8_t detectPowerState();
    uint8_t getPowerMode();
    uint8_t setPowerMode( uint8_t mode );
    void resetSuspendDelay();
    bool isOnBatteryPower();
    void enableWatchdog();
    void disableWatchdog();
    void resetWatchdog();
    void cpuReset();
    void reboot();
    bool detectConfigResetButton();


  private:
    void enterSleep();

    int8_t _pin_onbatt;
    int8_t _pin_sysoff;
    int8_t _pin_cfgrst;
    uint8_t _mode = POWER_MODE_NORMAL;
    bool _init = false;
    bool _wdt = false;
    uint32_t _lpwrTimerStart;
};


/* Power manager */
extern Power g_power;

#endif /* POWER_H */