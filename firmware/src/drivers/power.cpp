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


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - pin_onbatt : Pin which detect when the system is running
 *                  on battery power.
 *  - pin_sysoff : Pin connected to BQ24075 system off pin
 *  - pin_cfgrst : Pin which is connected to the reset/factory reset button.
 */
Power::Power( int8_t pin_onbatt, int8_t pin_sysoff, int8_t pin_cfgrst ) {

    this->_pin_onbatt = pin_onbatt;
    this->_pin_sysoff = pin_sysoff;
    this->_pin_cfgrst = pin_cfgrst;
}


/*--------------------------------------------------------------------------
 *
 * Initialize the power management class
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Power::begin() {

    pinMode( this->_pin_onbatt, INPUT );
    pinMode( this->_pin_sysoff, OUTPUT );
    digitalWrite( this->_pin_sysoff, LOW );

    this->_mode = POWER_MODE_NORMAL;

    this->detectPowerState();

    
}


/*--------------------------------------------------------------------------
 *
 * Get the current power state.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Current power state value.
 */
uint8_t Power::getPowerMode() {
    return this->_mode;
}


/*--------------------------------------------------------------------------
 *
 * Set the power state
 *
 * Arguments
 * ---------
 *  - mode : The new power state
 *
 * Returns : New power state.
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
    g_wifimanager.onPowerStateChange( mode );
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


/*--------------------------------------------------------------------------
 *
 * Run power management tasks.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : New power state.
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


/*--------------------------------------------------------------------------
 *
 * Detect the state of the 'on battery' pin and set power mode accordingly.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : New power state.
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


/*--------------------------------------------------------------------------
 *
 * Detect the state of the 'on battery' pin.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : New power state.
 */
bool Power::isOnBatteryPower() {
    return ( digitalRead( this->_pin_onbatt ) == HIGH );
}


/*--------------------------------------------------------------------------
 *
 * Resets the suspend timer.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : New power state.
 */
void Power::resetSuspendDelay() {
    this->_lpwrTimerStart = millis();
}


/*--------------------------------------------------------------------------
 *
 * Enter CPU sleep mode.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
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


/*--------------------------------------------------------------------------
 *
 * Pull the reset pin low from the "factory config/reset" pin to reset 
 * the processor.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
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


/*--------------------------------------------------------------------------
 *
 * Gracefuly reset the clock
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Power::reboot() {
    g_alarm.end();
    g_wifimanager.end();
    g_clock.end();
    g_lamp.end();
    g_lcd.end();
    
    this->cpuReset();
}


/*--------------------------------------------------------------------------
 *
 * Checkd the state of the "factory config/reset" button
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if the button is pressed or FALSE otherwise.
 */
bool Power::detectConfigResetButton() {
    if( this->_pin_cfgrst == -1 ) {
        return false;
    }

    return ( digitalRead( this->_pin_cfgrst ) == LOW );
}


/*--------------------------------------------------------------------------
 *
 * Enable watchdog timer.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Power::enableWatchdog() {
    /* Enable watchdog timer, 8 seconds timeout */
    // wdt_enable( WDTO_8S );

    this->_wdt = true;
}

/*--------------------------------------------------------------------------
 *
 * Disable watchdog timer.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Power::disableWatchdog() {

    wdt_disable();
    this->_wdt = false;
}


/*--------------------------------------------------------------------------
 *
 * Reset watchdog timer.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Power::resetWatchdog() {
    // wtd_reset();

}