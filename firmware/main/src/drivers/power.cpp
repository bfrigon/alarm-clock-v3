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





bool g_lowPowerMode = false;


Power::Power( uint8_t pin_onbatt, uint8_t pin_lowbatt ) {

    this->_pin_onbatt = pin_onbatt;
    this->_pin_lowbatt = pin_lowbatt;
}


void Power::begin() {

    pinMode( this->_pin_onbatt, INPUT );

    this->_mode = POWER_MODE_NORMAL;

    this->detectPowerState();
}


uint8_t Power::getPowerMode() {
    return this->_mode;
}

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
    g_clock.updatePowerState();
    g_lamp.updatePowerState();
    g_alarm.updatePowerState();

    g_screenUpdate = true;

    if( prevMode == POWER_MODE_SUSPEND || mode == POWER_MODE_SUSPEND ) {
        g_screenClear = true;
    }

    return mode;
}

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

        return;
    }

    /* Already in normal power mode */
    if( this->_mode == POWER_MODE_NORMAL ) {
        return this->_mode;
    }

    return this->setPowerMode( POWER_MODE_NORMAL );
}


bool Power::isOnBatteryPower() {
    return ( digitalRead( this->_pin_onbatt ) == LOW );
}


void Power::resetSuspendDelay() {
    this->_lpwrTimerStart = millis();
}




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


void Power::enableWatchdog() {
    /* Enable watchdog timer, 8 seconds timeout */
    // wdt_enable( WDTO_8S );

    this->_wdt = true;
}

void Power::disableWatchdog() {

    wdt_disable();
    this->_wdt = false;
}

void Power::resetWatchdog() {
    // wtd_reset();

}
