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


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - pin_leds : Pin ID connected to the nepoxel data line.
 */
Lamp::Lamp( int8_t pin_leds ) : NeoPixel( pin_leds, -1 ) {

}


/*--------------------------------------------------------------------------
 *
 * Sets the brightness of the lamp
 *
 * Arguments
 * ---------
 *  - brightness : Brighness value 0-100 %
 *
 * Returns : Nothing
 */
void Lamp::setBrightness( uint8_t brightness ) {

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


/*--------------------------------------------------------------------------
 *
 * Sets the lamp color using the RGB table color ID.
 *
 * Arguments
 * ---------
 *  - id : value 0-12 (color table in ressource.h)
 *
 * Returns : Nothing
 */
void Lamp::setColorFromTable( uint8_t id ) {

    if( this->_mode == LAMP_MODE_RAINBOW ) {
        return;
    }

    NeoPixel::setColorFromTable( id );
    this->update();
}


/*--------------------------------------------------------------------------
 *
 * Sets the lamp color using an RGB value.
 *
 * Arguments
 * ---------
 *  - r: Red component
 *  - g: Green component
 *  - b: Blue component
 *
 * Returns : Nothing
 */
void Lamp::setColorRGB( uint8_t r, uint8_t g, uint8_t b ) {

    if( this->_mode == LAMP_MODE_RAINBOW ) {
        return;
    }

    NeoPixel::setColorRGB( r, g, b );
    this->update();
}


/*--------------------------------------------------------------------------
 *
 * Sets the visual effect animation speed.
 *
 * Arguments
 * ---------
 *  - speed : 1 (slowest), 10 (fastest)

 * Returns : Nothing
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


/*--------------------------------------------------------------------------
 *
 * Turn on the lamp
 *
 * Arguments
 * ---------
 *  - settings  : Structure containing the lamp settings.
 *  - test_mode : True to force the lamp to remain on.
 *
 * Returns : Nothing
 */
void Lamp::activate( struct NightLampSettings *settings, bool test_mode = false ) {

    uint8_t mode;
    mode = settings->mode;

    if( test_mode == true && mode == LAMP_MODE_OFF ) {
        mode = LAMP_MODE_ON;
    }

    if( mode == this->_mode ) {
        /* Already active */
        return;
    }

    this->_mode = mode;
    this->_settings = settings;
    this->setColorFromTable( settings->color );
    this->setBrightness( settings->brightness );
    this->_delay_off = ( test_mode == true ) ? 0 : settings->delay_off;
    this->_visualStepSpeed = settings->speed;
    this->_visualStepReverse = false;
    this->_timerStart = millis();

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


/*--------------------------------------------------------------------------
 *
 * Update the visual effect animation next step delay when speed settings
 * changes.
 *
 * Arguments
 * ---------
 *  None
  *
 * Returns : Nothing
 */
void Lamp::updateVisualStepDelay() {

    switch( this->_mode ) {
        case LAMP_MODE_OFF:
        case LAMP_MODE_ON:
            this->_visualStepDelay = 0;
            break;

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
    }
}


/*--------------------------------------------------------------------------
 *
 * Turn off the lamp
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Lamp::deactivate() {

    if( this->_mode == LAMP_MODE_OFF ) {
        /* Alread off */
        return;
    }

    this->_mode = LAMP_MODE_OFF;
    this->update();
}


/*--------------------------------------------------------------------------
 *
 * Check if the turn-off delay timer has elapsed and execute the visual effect
 * animation next step.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Lamp::processEvents() {

    if( this->_mode == LAMP_MODE_OFF ) {
        return;
    }

    /* Check if the OFF delay is elapsed */
    if( this->_delay_off > 0 ) {

        if( millis() >= this->_timerStart + ( this->_delay_off * 60000 ) ) {
            this->deactivate();
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
void Lamp::update() {
    uint8_t pixmap[] = {
        ( ( this->_mode == LAMP_MODE_OFF || g_power.getPowerMode() != POWER_MODE_NORMAL ) ? 0x00 : 0xFF )
    };

    this->show( pixmap, 3 );
}