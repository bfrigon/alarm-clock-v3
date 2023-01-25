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

#include <alarm.h>
#include <resources.h>
#include <ui/screen.h>
#include <ui/ui.h>
#include <config.h>
#include "neoclock.h"
#include "rtc.h"
#include "power.h"



/*******************************************************************************
 *
 * @brief   Class constructor
 *
 * @param   pin_leds    Pin ID connected to the neopixel data line.
 * @param   pin_shdn    Pin ID connected to the neopixel power MOSFET.
 * 
 */
NeoClock::NeoClock( int8_t pin_leds, int8_t pin_shdn ) : NeoPixel( pin_leds, pin_shdn ) {

    _flashTimerStart = millis();
}


/*******************************************************************************
 *
 * @brief   Set the clock display back to it's default state.
 *
 */
void NeoClock::restoreClockDisplay() {

    DateTime local = g_rtc.now();
    g_timezone.toLocal( &local );

    _testMode = false;
    this->hour = local.hour();
    this->minute = local.minute();
    g_clock.hourFlashing = false;
    g_clock.minutesFlashing = false;
    g_clock.status_set = false;


    _updateRequested = true;
    g_screen.requestScreenUpdate( false );
}


/*******************************************************************************
 *
 * @brief   Refresh the lamp NeoPixel data.
 * 
 */
void NeoClock::update() {

    uint8_t pixmap[5];

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        return;
    }

    if( _init == false ) {
        return;
    }

    bool is_pm = false;
    uint8_t hour = this->hour;

    if( ( g_config.clock.display_24h == false ) && ( this->hour != 0xFF ) ) {

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

    if( this->hourFlashing == true && _flashState == false ) {
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

    if( this->minutesFlashing == true && _flashState == false ) {
        this->setDigitPixels( pixmap, 18, SEG_SPACE );
        this->setDigitPixels( pixmap, 25, SEG_SPACE );

    } else if( this->minute == 0xFF ) {
        this->setDigitPixels( pixmap, 18, SEG_MINUS );
        this->setDigitPixels( pixmap, 25, SEG_MINUS );

    } else {
        this->setDigitPixels( pixmap, 18, this->minute / 10 );
        this->setDigitPixels( pixmap, 25, this->minute % 10 );
    }

    this->setPixel( pixmap, 32, g_config.clock.alarm_on[0] && g_alarm.isAlarmSwitchOn() );
    this->setPixel( pixmap, 33, g_config.clock.alarm_on[1] && g_alarm.isAlarmSwitchOn() );


    /* Turn on all pixels in test mode */
    if( _testMode == true ) {
        pixmap[0] = 0xFF;
        pixmap[1] = 0xFF;
        pixmap[2] = 0xFF;
        pixmap[3] = 0xFF;
        pixmap[4] = 0xFF;
    }

    this->show( pixmap, 34 );
}


/*******************************************************************************
 *
 * @brief   Check if the pixels needs to be updated.
 * 
 */
void NeoClock::processEvents() {

    DateTime now = g_rtc.now();
    

    /* If time has changed, update the clock display. */
    if( now != _prevDate ) {
        _prevDate = now;

        if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
            g_screen.requestScreenUpdate( false );
        }

        this->requestClockUpdate();
    }

    /* Update the clock display when blinking state changes. */
    if( millis() - _flashTimerStart > ( this->flashRate * 10 ) ) {
        _flashTimerStart = millis();
        _flashState = !( _flashState );

        if( this->hourFlashing == true || this->minutesFlashing == true ) {
            _updateRequested = true;
        }
    }

    /* Update the clock display if requested. */
    if( _updateRequested == true ) {

        g_clock.update();
        _updateRequested = false;
    }
}


/*******************************************************************************
 *
 * @brief   Sets pixel for a given segment digit position. Each digits 
 *          contains 7 pixels.
 *
 * @param   pixmap    Pointer to the pixel buffer ( 1 bit per pixel )
 * @param   pos       Position in the string where the segment digit begins.
 * @param   value     Value to assign to digit.
 * 
 */
void NeoClock::setDigitPixels( uint8_t* pixmap, uint8_t pos, uint8_t value ) {

    uint8_t chr = pgm_read_byte( &_charmap[ value ] );

    for( uint8_t i = 0; i < 8; i++ ) {
        this->setPixel( pixmap, pos + i, ( chr & ( 1 << i ) ) );
    }
}


/*******************************************************************************
 *
 * @brief   Turn on all the leds on the display.
 *
 * @param   testMode    True to enable pixel test mode.
 */
void NeoClock::setTestMode( bool testMode ) {
    _testMode = testMode;
}


/*******************************************************************************
 *
 * @brief   Request a clock display update. It only accepts the request 
 *          when the current screen is neither the clock set screen or the 
 *          alarm display screen.
 *
 * @return  TRUE if the request is accepted, FALSE otherwise.
 * 
 */
bool NeoClock::requestClockUpdate( bool force ) {

    DateTime now = g_rtc.now();

    switch( g_screen.getId() ) {
        case SCREEN_ID_SET_TIME:
        case SCREEN_ID_SHOW_ALARMS:

            if( force == true ) {
                _updateRequested = true;
                return true;
            }
        
            /* Don't update clock display on these screens. */
            break;

        case SCREEN_ID_ROOT:
            g_screen.requestScreenUpdate( false );

        /* Fall-through */

        default:
            g_timezone.toLocal( &now );

            this->hour = now.hour();
            this->minute = now.minute();

            _updateRequested = true;
            return true;
    }

    return false;
}