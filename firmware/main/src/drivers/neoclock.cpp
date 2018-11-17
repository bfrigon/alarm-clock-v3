//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/neoclock.cpp
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
#include "neoclock.h"
#include "../config.h"
#include "../alarm.h"
#include "../libs/time.h"
#include "../resources.h"


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - pin_leds : Pin ID connected to the neopixel data line.
 *  - pin_shdn : Pin ID connected to the neopixel power MOSFET.
 */
NeoClock::NeoClock( int8_t pin_leds, int8_t pin_shdn ) : NeoPixel( pin_leds, pin_shdn ) {

    this->_flashTimerStart = millis();
}


/*--------------------------------------------------------------------------
 *
 * Refresh the lamp NeoPixel data.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void NeoClock::update() {

    uint8_t pixmap[5];

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        return;
    }

    if( this->_init == false ) {
        return;
    }

    bool is_pm = false;
    uint8_t hour = this->hour;

    if( ( g_config.clock_24h == false ) && ( this->hour != 0xFF ) ) {

        if( hour > 12 ) {
            hour -= 12;
        }

        if( hour == 0 ) {
            hour = 12;
        }

        is_pm = ( this->hour > 11 );
    }


    this->setPixel( pixmap, 0, is_pm );
    this->setPixel( pixmap, 1, this->status_set );

    if( this->hourFlashing == true && this->_flashState == false ) {
        this->setDigitPixels( pixmap, 2, SEG_SPACE );
        this->setDigitPixels( pixmap, 9, SEG_SPACE );

    } else if( this->hour == 0xFF ) {
        this->setDigitPixels( pixmap, 2, SEG_MINUS );
        this->setDigitPixels( pixmap, 9, SEG_MINUS );

    } else {
        this->setDigitPixels( pixmap, 2, ( hour > 9 ) ? hour / 10 : SEG_SPACE );
        this->setDigitPixels( pixmap, 9, hour % 10 );
    }

    this->setPixel( pixmap, 16, true );
    this->setPixel( pixmap, 17, true );

    if( this->minutesFlashing == true && this->_flashState == false ) {
        this->setDigitPixels( pixmap, 18, SEG_SPACE );
        this->setDigitPixels( pixmap, 25, SEG_SPACE );

    } else if( this->minute == 0xFF ) {
        this->setDigitPixels( pixmap, 18, SEG_MINUS );
        this->setDigitPixels( pixmap, 25, SEG_MINUS );

    } else {
        this->setDigitPixels( pixmap, 18, this->minute / 10 );
        this->setDigitPixels( pixmap, 25, this->minute % 10 );
    }

    this->setPixel( pixmap, 32, g_config.alarm_on[0] && g_alarm.isAlarmSwitchOn() );
    this->setPixel( pixmap, 33, g_config.alarm_on[1] && g_alarm.isAlarmSwitchOn() );


    /* Turn on all pixels in test mode */
    if( this->_testMode == true ) {
        pixmap[0] = 0xFF;
        pixmap[1] = 0xFF;
        pixmap[2] = 0xFF;
        pixmap[3] = 0xFF;
        pixmap[4] = 0xFF;
    }

    this->show( pixmap, 34 );
}


/*--------------------------------------------------------------------------
 *
 * Check if the pixels needs to be updated.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void NeoClock::processUpdateEvents() {

    if( millis() - this->_flashTimerStart > ( this->flashRate * 10 ) ) {
        this->_flashTimerStart = millis();
        this->_flashState = !( this->_flashState );

        if( this->hourFlashing == true || this->minutesFlashing == true ) {
            g_clockUpdate = true;
        }
    }


    if( g_clockUpdate == true ) {

        g_clock.update();
        g_clockUpdate = false;
    }
}


/*--------------------------------------------------------------------------
 *
 * Sets pixel for a given segment digit position. Each digits contains 7 pixels.
 *
 * Arguments
 * ---------
 *  - pixmap : Pointer to the pixel buffer ( 1 bit per pixel )
 *  - pos    : Position in the string where the segment digit begins.
 *  - value  : Value to assign to digit.
 * 
 * Returns : Nothing
 */
void NeoClock::setDigitPixels( uint8_t *pixmap, uint8_t pos, uint8_t value ) {

    uint8_t chr = pgm_read_byte( &_charmap[ value ] );

    for( uint8_t i = 0; i < 8; i++ ) {
        this->setPixel( pixmap, pos + i, ( chr & ( 1 << i ) ) );
    }
}


/*--------------------------------------------------------------------------
 *
 * Turn on all the leds on the display.
 *
 * Arguments
 * ---------
 *  - testMode : True to enable pixel test mdoe.
 *
 * Returns : Nothing
 */
void NeoClock::setTestMode( bool testMode ) {
    this->_testMode = testMode;
}