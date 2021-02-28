//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/power.cpp
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

#include <ui/screen.h>
#include <ui/ui.h>
#include <hardware.h>
#include <alarm.h>
#include <services/telnet_console.h>

#include "power.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 * @param   pin_onbatt    Pin which detect when the system is running
 *                        on battery power.
 * @param   pin_sysoff    Pin connected to BQ24075 system off pin
 * @param   pin_cfgrst    Pin which is connected to the reset/factory 
 *                        reset button.
 * 
 */
Power::Power( int8_t pin_onbatt, int8_t pin_sysoff, int8_t pin_cfgrst ) {

    _pin_onbatt = pin_onbatt;
    _pin_sysoff = pin_sysoff;
    _pin_cfgrst = pin_cfgrst;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the power management class
 * 
 */
void Power::begin() {

    pinMode( _pin_onbatt, INPUT );
    pinMode( _pin_sysoff, OUTPUT );
    digitalWrite( _pin_sysoff, LOW );

    _mode = POWER_MODE_NORMAL;

    this->detectPowerState();

    
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the current power state.
 *
 * @return  Current power state value.
 * 
 */
uint8_t Power::getPowerMode() {
    return _mode;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the power state
 *
 * @param   mode    The new power state
 *
 * @return  The current power state.
 * 
 */
uint8_t Power::setPowerMode( uint8_t mode ) {
    uint8_t prevMode = _mode;

    if( prevMode == mode ) {
        return mode;
    }

    if( this->isOnBatteryPower() == false && mode != POWER_MODE_NORMAL ) {
        mode = POWER_MODE_NORMAL;
    }

    _mode = mode;


    this->resetSuspendDelay();

    if( mode == POWER_MODE_SUSPEND ) {
        g_screen.activate( &screen_suspend );
    }

    if( prevMode == POWER_MODE_SUSPEND && mode != POWER_MODE_SUSPEND ) {
        g_screen.exitScreen();
    }

    /* Update modules power state */
    g_wifi.onPowerStateChange( mode );
    g_clock.onPowerStateChange( mode );
    g_lamp.onPowerStateChange( mode );
    g_alarm.onPowerStateChange( mode );
    g_als.onPowerStateChange( mode );
    


    return mode;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run power management tasks.
 *
 */
void Power::processEvents() {

    g_power.detectPowerState();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Detect the state of the 'on battery' pin and set power mode 
 *          accordingly.
 *
 * @return  Detected power state
 * 
 */
uint8_t Power::detectPowerState() {

    if( this->isOnBatteryPower() ) {

        if( _mode == POWER_MODE_NORMAL ) {
            return this->setPowerMode( POWER_MODE_LOW_POWER );

        } else if( _mode == POWER_MODE_LOW_POWER && ( millis() - _lpwrTimerStart >= DELAY_BEFORE_SUSPEND ) ) {
            return this->setPowerMode( POWER_MODE_SUSPEND );

        } else if( _mode == POWER_MODE_SUSPEND ) {
            this->enterSleep();
            return _mode;
        }

        return _mode;
    }

    /* Already in normal power mode */
    if( _mode == POWER_MODE_NORMAL ) {
        return _mode;
    }

    return this->setPowerMode( POWER_MODE_NORMAL );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Detect the state of the 'on battery' pin.
 *
 * @return  TRUE if currently running on battery power, FALSE otherwise
 * 
 */
bool Power::isOnBatteryPower() {
    return ( digitalRead( _pin_onbatt ) == HIGH );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Resets the suspend timer.
 * 
 */
void Power::resetSuspendDelay() {
    _lpwrTimerStart = millis();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Enter CPU sleep mode.
 * 
 */
void Power::enterSleep() {


    /* Power-down unused modules during sleep */
    power_adc_disable();
    power_spi_disable();
    power_usart0_disable();
    power_usart2_disable();
    power_timer1_disable();
    power_timer2_disable();
    power_timer3_disable();
    power_timer4_disable();
    power_timer5_disable();
    power_twi_disable();

    /* Disable watchdog */
    wdt_disable();

    set_sleep_mode( SLEEP_MODE_PWR_DOWN );

    sleep_enable();
    sleep_mode();
    sleep_disable();


    /* Power up all modules */
    power_adc_enable();
    power_spi_enable();
    power_usart0_enable();
    power_usart2_enable();
    power_timer1_enable();
    power_timer2_enable();
    power_timer3_enable();
    power_timer4_enable();
    power_timer5_enable();
    power_twi_enable();

    /* Re-enable watchdog */
    if( _wdt == true ) {
        this->enableWatchdog();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Pull the reset pin low from the "factory config/reset" pin to reset 
 *          the processor.
 * 
 */
void Power::cpuReset() {

    if( _pin_cfgrst == -1 ) {
        return;
    }

    pinMode( _pin_cfgrst, OUTPUT );
    digitalWrite( _pin_cfgrst, LOW );

    /* Halt */
    while( true );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Gracefuly reboot
 * 
 */
void Power::reboot() {

    g_telnetConsole.stopServer();
    g_alarm.end();
    g_wifi.end();
    g_clock.end();
    g_lamp.end();
    g_lcd.end();
    
    this->cpuReset();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check the state of the "factory config/reset" button
 *
 * @return  TRUE if the button is pressed or FALSE otherwise.
 * 
 */
bool Power::detectConfigResetButton() {
    if( _pin_cfgrst == -1 ) {
        return false;
    }

    return ( digitalRead( _pin_cfgrst ) == LOW );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Enable the watchdog timer.
 * 
 */
void Power::enableWatchdog() {
    /* Enable watchdog timer, 8 seconds timeout */
    // wdt_enable( WDTO_8S );

    _wdt = true;
}

/*! ------------------------------------------------------------------------
 *
 * @brief   Disable the watchdog timer.
 * 
 */
void Power::disableWatchdog() {

    wdt_disable();
    _wdt = false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reset the watchdog timer.
 * 
 */
void Power::resetWatchdog() {
    // wtd_reset();

}