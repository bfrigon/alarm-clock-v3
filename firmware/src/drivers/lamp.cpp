//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/lamp.cpp
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

#include "lamp.h"
#include "power.h"



/*******************************************************************************
 *
 * @brief   Class constructor
 *
 * @param   pin_leds    Pin ID connected to the nepoxel data line.
 * 
 */
Lamp::Lamp( int8_t pin_leds ) : NeoPixel( pin_leds, -1 ) {

}


/*******************************************************************************
 *
 * @brief   Sets the brightness of the lamp
 *
 * @param   brightness      Brighness value 0-100 %
 * @param   force           Force set even if night light is active
 * 
 */
void Lamp::setBrightness( uint8_t brightness, bool force ) {

    if( _mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( brightness > 100 ) {
        brightness = 100;
    }

    /* Need to update the fading speed accordingly if the brightness changes. */
    if( _mode == LAMP_MODE_FADING ) {
        this->updateVisualStepDelay();
        return;
    }

    if( _mode == LAMP_MODE_FLASHING && _brightness == 0 ) {
        return;
    }

    NeoPixel::setBrightness( brightness );
    this->update();
}


/*******************************************************************************
 *
 * @brief   Sets the lamp color using the RGB table color ID.
 *
 * @param   id      value 0-12 (color table in ressource.h)
 * @param   force   Force set even if night light is active
 * 
 */
void Lamp::setColorFromTable( uint8_t id, bool force ) {

    if( _mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( _mode == LAMP_MODE_RAINBOW ) {
        return;
    }
    
    NeoPixel::setColorFromTable( id );
    this->update();
}


/*******************************************************************************
 *
 * @brief   Sets the lamp color using an RGB value.
 *
 * @param   r       Red component
 * @param   g       Green component
 * @param   b       Blue component
 * @param   force   Force set even if night light is active
 * 
 */
void Lamp::setColorRGB( uint8_t r, uint8_t g, uint8_t b, bool force ) {

    if( _mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( _mode == LAMP_MODE_RAINBOW ) {
        return;
    }

    NeoPixel::setColorRGB( r, g, b );
    this->update();
}


/*******************************************************************************
 *
 * @brief   Sets the visual effect animation speed.
 *
 * @param   speed   1 (slowest), 10 (fastest)
 * 
 */
void Lamp::setEffectSpeed( uint8_t speed ) {
    if( speed > 10 ) {
        speed = 10;
    }

    if( speed == 0 ) {
        speed = 1;
    }

    _visualStepSpeed = speed;

    this->updateVisualStepDelay();
}


/*******************************************************************************
 *
 * @brief   Sets the lamp off delay.
 *
 * @param   delay       Delay in minutes before the lamp turn off 
 * 
 */
void Lamp::setDelayOff( uint8_t delay ) {
    if( delay == _delay_off ) {
        return;
    }

    _delay_off = delay;

    if( _mode == LAMP_MODE_NIGHTLIGHT ) {

        /* Reset timer */
        _timerStart = millis();
    }
}


/*******************************************************************************
 *
 * @brief   Turn on the lamp
 *
 * @param   settings    Structure containing the lamp settings.
 * @param   test_mode   True to force the lamp to remain on.
 * @param   force       Force activate even if night light active
 * 
 */
void Lamp::activate( struct NightLampSettings *settings, bool test_mode, bool force, uint8_t mode ) {

    if( _mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( mode == LAMP_MODE_NOOVERRIDE ) {

        if( _mode == LAMP_MODE_NIGHTLIGHT ) {
            mode = LAMP_MODE_NIGHTLIGHT;
        } else {
            mode = settings->mode;
        } 
    }

    if( test_mode == true && mode == LAMP_MODE_OFF ) {
        mode = LAMP_MODE_ON;
    }
    
    _settings = settings;
    _delay_off = ( test_mode == true ) ? 0 : settings->delay_off;
    _visualStepSpeed = settings->speed;
    _visualStepReverse = false;
    _timerStart = millis();
    this->setColorFromTable( settings->color, force );
    this->setBrightness( settings->brightness, force );
    _mode = mode;

    this->update();

    switch( _mode ) {
        case LAMP_MODE_FADING:
            _visualStepValue = settings->brightness;
            break;

        default:
            _visualStepValue = 0;
            break;
    }

    this->updateVisualStepDelay();
}


/*******************************************************************************
 *
 * @brief   Returns whether or not the lamp is on
 *
 * @return  TRUE if lamp is ON, FLASE otherwise
 * 
 */
bool Lamp::isActive() {
    return ( _mode != LAMP_MODE_OFF );
}


/*******************************************************************************
 *
 * @brief   Update the visual effect animation next step delay when speed 
 *          settings changes.
 *
 */
void Lamp::updateVisualStepDelay() {

    switch( _mode ) {
        case LAMP_MODE_FLASHING:
            _visualStepDelay = 2500 / _visualStepSpeed;
            break;

        case LAMP_MODE_FADING:
            _visualStepDelay = ( 25000 / _visualStepSpeed ) /
                                     max( _settings->brightness, LAMP_MIMIMUM_FADING_BRIGHTNESS );
            break;

        case LAMP_MODE_RAINBOW:
            _visualStepDelay = 250 / _visualStepSpeed;
            break;

        default:
            _visualStepDelay = 0;
            break;
    }
}


/*******************************************************************************
 *
 * @brief   Turn off the lamp
 *
 * @param   force   Force deactivate even if night light is active
 * 
 */
void Lamp::deactivate( bool force ) {

    if( _mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    _mode = LAMP_MODE_OFF;
    this->update();
}


/*******************************************************************************
 *
 * @brief   Check if the turn-off delay timer has elapsed and execute the 
 *          visual effect animation next step.
 * 
 */
void Lamp::processEvents() {

    if( _mode == LAMP_MODE_OFF ) {
        return;
    }

    /* Check if the OFF delay is elapsed */
    if( _delay_off > 0 ) {

        uint32_t timerEnd;
        timerEnd = _timerStart + ( _delay_off * 60000UL);

        if( millis() >= timerEnd ) {

            if( millis() >= timerEnd + 5000 ) {
                this->deactivate( true );
                return;

            } else {

                uint8_t brightness;
                brightness = _settings->brightness - ((millis() - timerEnd) * _settings->brightness / 5000 );

                if ( brightness != _brightness ) {

                    _brightness = brightness;
                    this->update();
                }
            }

            return;
        }
    }

    /* Check if lamp effect is enabled */
    if( _visualStepDelay == 0 ) {
        return;
    }

    /* Check if the visual effect next step delay is elapsed */
    if( ( millis() - _timerStart ) < _visualStepDelay ) {
        return;
    }


    switch( _mode ) {


        case LAMP_MODE_FLASHING:
            _visualStepReverse = !_visualStepReverse;
            _brightness = _visualStepReverse ? 0 : _settings->brightness;
            break;


        case LAMP_MODE_FADING:

            uint8_t maxStepValue;
            maxStepValue = max( _settings->brightness, LAMP_MIMIMUM_FADING_BRIGHTNESS );

            if( _visualStepValue >= maxStepValue ) {
                _visualStepValue = maxStepValue;
                _visualStepReverse = true;
            }

            if( _visualStepValue <= 5 ) {
                _visualStepReverse = false;
            }


            _visualStepValue += ( _visualStepReverse ? -5 : 5 );
            _brightness = _visualStepValue;
            break;

        case LAMP_MODE_RAINBOW:
            _visualStepValue += 5;

            if( _visualStepValue < 85 ) {

                _r = _visualStepValue * 3;
                _g = 255 - _visualStepValue * 3;
                _b = 0;

            } else if( _visualStepValue < 170 ) {

                _r = 255 - ( _visualStepValue - 85 ) * 3;
                _g = 0;
                _b = ( _visualStepValue - 85 ) * 3;

            } else {

                _r = 0;
                _g = ( _visualStepValue - 170 ) * 3;
                _b = 255 - ( _visualStepValue - 170 ) * 3;
            }

            break;
    }

    this->update();

    _timerStart = millis();
}


/*******************************************************************************
 *
 * @brief   Refresh the lamp NeoPixel data.
 * 
 */
void Lamp::update() {
    uint8_t pixmap[] = {
        (uint8_t)(( _mode == LAMP_MODE_OFF || g_power.getPowerMode() != POWER_MODE_NORMAL ) ? 0x00 : 0xFF )
    };

    this->show( pixmap, 3 );
}