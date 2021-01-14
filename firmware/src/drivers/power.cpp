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
#include "power.h"
#include "../screen.h"
#include "../hardware.h"
#include "../alarm.h"


/*! ------------------------------------------------------------------------
 *
 * @brief	Class constructor
 *
 * @param   pin_onbatt    Pin which detect when the system is running
 *                        on battery power.
 * @param   pin_sysoff    Pin connected to BQ24075 system off pin
 * @param   pin_cfgrst    Pin which is connected to the reset/factory 
 *                        reset button.
 * 
 */
Power::Power( int8_t pin_onbatt, int8_t pin_sysoff, int8_t pin_cfgrst ) {

    this->_pin_onbatt = pin_onbatt;
    this->_pin_sysoff = pin_sysoff;
    this->_pin_cfgrst = pin_cfgrst;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Initialize the power management class
 * 
 */
void Power::begin() {

    pinMode( this->_pin_onbatt, INPUT );
    pinMode( this->_pin_sysoff, OUTPUT );
    digitalWrite( this->_pin_sysoff, LOW );

    this->_mode = POWER_MODE_NORMAL;

    this->detectPowerState();

    
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Get the current power state.
 *
 * @return  Current power state value.
 * 
 */
uint8_t Power::getPowerMode() {
    return this->_mode;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Set the power state
 *
 * @param   mode    The new power state
 *
 * @return  The current power state.
 * 
 */
uint8_t Power::setPowerMode( uint8_t mode ) {
    uint8_t prevMode = this->_mode;

    if( prevMode == mode ) {
        return mode;
    }

    if( this->isOnBatteryPower() == false && mode != POWER_MODE_NORMAL ) {
        mode = POWER_MODE_NORMAL;
    }

    this->_mode = mode;


    this->resetSuspendDelay();

    /* Update modules power state */
    g_wifi.onPowerStateChange( mode );
    g_clock.onPowerStateChange( mode );
    g_lamp.onPowerStateChange( mode );
    g_alarm.onPowerStateChange( mode );
    g_als.onPowerStateChange( mode );
    

    g_screenUpdate = true;

    if( prevMode == POWER_MODE_SUSPEND || mode == POWER_MODE_SUSPEND ) {
        g_screenClear = true;
    }

    return mode;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Run power management tasks.
 *
 */
void Power::runTask() {

    uint8_t prevState = this->_mode;
    uint8_t newState = g_power.detectPowerState();


    if( newState != prevState ) {
        g_screenUpdate = true;

        if( prevState == POWER_MODE_SUSPEND || newState == POWER_MODE_SUSPEND ) {
            g_screenClear = true;
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Detect the state of the 'on battery' pin and set power mode 
 *          accordingly.
 *
 * @return  Detected power state
 * 
 */
uint8_t Power::detectPowerState() {

    if( this->isOnBatteryPower() ) {

        if( this->_mode == POWER_MODE_NORMAL ) {
            return this->setPowerMode( POWER_MODE_LOW_POWER );

        } else if( this->_mode == POWER_MODE_LOW_POWER && ( millis() - this->_lpwrTimerStart >= DELAY_BEFORE_SUSPEND ) ) {
            return this->setPowerMode( POWER_MODE_SUSPEND );

        } else if( this->_mode == POWER_MODE_SUSPEND ) {
            this->enterSleep();
            return this->_mode;
        }

        return this->_mode;
    }

    /* Already in normal power mode */
    if( this->_mode == POWER_MODE_NORMAL ) {
        return this->_mode;
    }

    return this->setPowerMode( POWER_MODE_NORMAL );
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Detect the state of the 'on battery' pin.
 *
 * @return  TRUE if currently running on battery power, FALSE otherwise
 * 
 */
bool Power::isOnBatteryPower() {
    return ( digitalRead( this->_pin_onbatt ) == HIGH );
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Resets the suspend timer.
 * 
 */
void Power::resetSuspendDelay() {
    this->_lpwrTimerStart = millis();
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Enter CPU sleep mode.
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
    if( this->_wdt == true ) {
        this->enableWatchdog();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Pull the reset pin low from the "factory config/reset" pin to reset 
 *          the processor.
 * 
 */
void Power::cpuReset() {

    if( this->_pin_cfgrst == -1 ) {
        return;
    }

    pinMode( this->_pin_cfgrst, OUTPUT );
    digitalWrite( this->_pin_cfgrst, LOW );

    /* Halt */
    while( true );
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Gracefuly reboot
 * 
 */
void Power::reboot() {
    g_alarm.end();
    g_wifi.end();
    g_clock.end();
    g_lamp.end();
    g_lcd.end();
    
    this->cpuReset();
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Check the state of the "factory config/reset" button
 *
 * @return  TRUE if the button is pressed or FALSE otherwise.
 * 
 */
bool Power::detectConfigResetButton() {
    if( this->_pin_cfgrst == -1 ) {
        return false;
    }

    return ( digitalRead( this->_pin_cfgrst ) == LOW );
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Enable the watchdog timer.
 * 
 */
void Power::enableWatchdog() {
    /* Enable watchdog timer, 8 seconds timeout */
    // wdt_enable( WDTO_8S );

    this->_wdt = true;
}

/*! ------------------------------------------------------------------------
 *
 * @brief	Disable the watchdog timer.
 * 
 */
void Power::disableWatchdog() {

    wdt_disable();
    this->_wdt = false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief	Reset the watchdog timer.
 * 
 */
void Power::resetWatchdog() {
    // wtd_reset();

}