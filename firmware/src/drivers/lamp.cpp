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


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 * @param   pin_leds    Pin ID connected to the nepoxel data line.
 * 
 */
Lamp::Lamp( int8_t pin_leds ) : NeoPixel( pin_leds, -1 ) {

}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the brightness of the lamp
 *
 * @param   brightness      Brighness value 0-100 %
 * @param   force           Force set even if night light is active
 * 
 */
void Lamp::setBrightness( uint8_t brightness, bool force ) {

    if( this->_mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( brightness > 100 ) {
        brightness = 100;
    }

    /* Need to update the fading speed accordingly if the brightness changes. */
    if( this->_mode == LAMP_MODE_FADING ) {
        this->updateVisualStepDelay();
        return;
    }

    if( this->_mode == LAMP_MODE_FLASHING && this->_brightness == 0 ) {
        return;
    }

    NeoPixel::setBrightness( brightness );
    this->update();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the lamp color using the RGB table color ID.
 *
 * @param   id      value 0-12 (color table in ressource.h)
 * @param   force   Force set even if night light is active
 * 
 */
void Lamp::setColorFromTable( uint8_t id, bool force ) {

    if( this->_mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( this->_mode == LAMP_MODE_RAINBOW ) {
        return;
    }
    
    NeoPixel::setColorFromTable( id );
    this->update();
}


/*! ------------------------------------------------------------------------
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

    if( this->_mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( this->_mode == LAMP_MODE_RAINBOW ) {
        return;
    }

    NeoPixel::setColorRGB( r, g, b );
    this->update();
}


/*! ------------------------------------------------------------------------
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

    this->_visualStepSpeed = speed;

    this->updateVisualStepDelay();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the lamp off delay.
 *
 * @param   delay       Delay in minutes before the lamp turn off 
 * 
 */
void Lamp::setDelayOff( uint8_t delay ) {
    if( delay == this->_delay_off ) {
        return;
    }

    this->_delay_off = delay;

    if( this->_mode == LAMP_MODE_NIGHTLIGHT ) {

        /* Reset timer */
        this->_timerStart = millis();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Turn on the lamp
 *
 * @param   settings    Structure containing the lamp settings.
 * @param   test_mode   True to force the lamp to remain on.
 * @param   force       Force activate even if night light active
 * 
 */
void Lamp::activate( struct NightLampSettings *settings, bool test_mode, bool force, uint8_t mode ) {

    if( this->_mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    if( mode == LAMP_MODE_NOOVERRIDE ) {

        if( this->_mode == LAMP_MODE_NIGHTLIGHT ) {
            mode = LAMP_MODE_NIGHTLIGHT;
        } else {
            mode = settings->mode;
        } 
    }

    if( test_mode == true && mode == LAMP_MODE_OFF ) {
        mode = LAMP_MODE_ON;
    }
    
    this->_settings = settings;
    this->_delay_off = ( test_mode == true ) ? 0 : settings->delay_off;
    this->_visualStepSpeed = settings->speed;
    this->_visualStepReverse = false;
    this->_timerStart = millis();
    this->setColorFromTable( settings->color, force );
    this->setBrightness( settings->brightness, force );
    this->_mode = mode;

    this->update();

    switch( this->_mode ) {
        case LAMP_MODE_FADING:
            this->_visualStepValue = settings->brightness;
            break;

        default:
            this->_visualStepValue = 0;
            break;
    }

    this->updateVisualStepDelay();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns whether or not the lamp is on
 *
 * @return  TRUE if lamp is ON, FLASE otherwise
 * 
 */
bool Lamp::isActive() {
    return ( _mode != LAMP_MODE_OFF );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Update the visual effect animation next step delay when speed 
 *          settings changes.
 *
 */
void Lamp::updateVisualStepDelay() {

    switch( this->_mode ) {
        case LAMP_MODE_FLASHING:
            this->_visualStepDelay = 2500 / this->_visualStepSpeed;
            break;

        case LAMP_MODE_FADING:
            this->_visualStepDelay = ( 25000 / this->_visualStepSpeed ) /
                                     max( this->_settings->brightness, LAMP_MIMIMUM_FADING_BRIGHTNESS );
            break;

        case LAMP_MODE_RAINBOW:
            this->_visualStepDelay = 250 / this->_visualStepSpeed;
            break;

        default:
            this->_visualStepDelay = 0;
            break;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Turn off the lamp
 *
 * @param   force   Force deactivate even if night light is active
 * 
 */
void Lamp::deactivate( bool force ) {

    if( this->_mode == LAMP_MODE_NIGHTLIGHT && force == false ) {
        return;
    }

    this->_mode = LAMP_MODE_OFF;
    this->update();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check if the turn-off delay timer has elapsed and execute the 
 *          visual effect animation next step.
 * 
 */
void Lamp::processEvents() {

    if( this->_mode == LAMP_MODE_OFF ) {
        return;
    }

    /* Check if the OFF delay is elapsed */
    if( this->_delay_off > 0 ) {

        uint32_t timerEnd;
        timerEnd = this->_timerStart + ( this->_delay_off * 60000UL);

        if( millis() >= timerEnd ) {

            if( millis() >= timerEnd + 5000 ) {
                this->deactivate( true );
                return;

            } else {

                uint8_t brightness;
                brightness = this->_settings->brightness - ((millis() - timerEnd) * this->_settings->brightness / 5000 );

                if ( brightness != this->_brightness ) {

                    this->_brightness = brightness;
                    this->update();
                }
            }

            return;
        }
    }

    /* Check if lamp effect is enabled */
    if( this->_visualStepDelay == 0 ) {
        return;
    }

    /* Check if the visual effect next step delay is elapsed */
    if( ( millis() - this->_timerStart ) < this->_visualStepDelay ) {
        return;
    }


    switch( this->_mode ) {


        case LAMP_MODE_FLASHING:
            this->_visualStepReverse = !this->_visualStepReverse;
            this->_brightness = this->_visualStepReverse ? 0 : this->_settings->brightness;
            break;


        case LAMP_MODE_FADING:

            uint8_t maxStepValue;
            maxStepValue = max( this->_settings->brightness, LAMP_MIMIMUM_FADING_BRIGHTNESS );

            if( this->_visualStepValue >= maxStepValue ) {
                this->_visualStepValue = maxStepValue;
                this->_visualStepReverse = true;
            }

            if( this->_visualStepValue <= 5 ) {
                this->_visualStepReverse = false;
            }


            this->_visualStepValue += ( this->_visualStepReverse ? -5 : 5 );
            this->_brightness = this->_visualStepValue;
            break;

        case LAMP_MODE_RAINBOW:
            this->_visualStepValue += 5;

            if( this->_visualStepValue < 85 ) {

                this->_r = this->_visualStepValue * 3;
                this->_g = 255 - this->_visualStepValue * 3;
                this->_b = 0;

            } else if( this->_visualStepValue < 170 ) {

                this->_r = 255 - ( this->_visualStepValue - 85 ) * 3;
                this->_g = 0;
                this->_b = ( this->_visualStepValue - 85 ) * 3;

            } else {

                this->_r = 0;
                this->_g = ( this->_visualStepValue - 170 ) * 3;
                this->_b = 255 - ( this->_visualStepValue - 170 ) * 3;
            }

            break;
    }

    this->update();

    this->_timerStart = millis();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Refresh the lamp NeoPixel data.
 * 
 */
void Lamp::update() {
    uint8_t pixmap[] = {
        ( ( this->_mode == LAMP_MODE_OFF || g_power.getPowerMode() != POWER_MODE_NORMAL ) ? 0x00 : 0xFF )
    };

    this->show( pixmap, 3 );
}