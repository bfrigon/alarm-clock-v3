//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/neopixel.cpp
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
#include "neopixel.h"
#include "power.h"
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
NeoPixel::NeoPixel( int8_t pin_leds, int8_t pin_shdn ) {
    this->_pin_leds = pin_leds;
    this->_pin_shdn = pin_shdn;
}


/*--------------------------------------------------------------------------
 *
 * Initialize pins for the LED string.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void NeoPixel::begin() {
    if( this->_init == true ) {
        return;
    }

    this->_init = true;

    pinMode( this->_pin_leds, OUTPUT );

    if( this->_pin_shdn >= 0 ) {
        pinMode( this->_pin_shdn, OUTPUT );
    }

    this->onPowerStateChange( g_power.getPowerMode() );
}


/*--------------------------------------------------------------------------
 *
 * Disable power for the LED string.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void NeoPixel::end() {
    if( this->_init == false ) {
        return;
    }

    this->_init = false;

    if( this->_pin_shdn >= 0 ) {
        digitalWrite( this->_pin_shdn, HIGH );
    }
}


/*--------------------------------------------------------------------------
 *
 * Disable/enable power for the leds based on the current system power state
 *
 * Arguments
 * ---------
 *  - state : Current power state
 *
 * Returns : Nothing
 */
void NeoPixel::onPowerStateChange( uint8_t state ) {

    if( this->_init == false ) {
        return;
    }

    if( this->_pin_shdn >= 0 ) {
        digitalWrite( this->_pin_shdn, ( state == POWER_MODE_SUSPEND ? HIGH : LOW ) );
    }

    this->update();
}


/*--------------------------------------------------------------------------
 *
 * Calculate the pixel brightness and apply gamma correction for an 
 * individual color channel.
 *
 * Arguments
 * ---------
 *  - color : Color channel value.
 *
 * Returns : The corrected brightness value.
 */
inline uint8_t NeoPixel::getColorBrigthness( uint8_t color ) {

    uint8_t brightness = this->_brightness;

    if( g_power.getPowerMode() == POWER_MODE_LOW_POWER ) {
        /* limit brightness in low power mode */
        brightness = ( this->_brightness > 25 ) ? 25 : this->_brightness;
    }

    color = ( ( uint16_t )color * ( uint16_t )brightness ) / 100;

    if( this->_gammaCorrection == true ) {
        color = pgm_read_byte( &_GAMMA_TABLE[ color ] );
    }

    return color;
}


/*--------------------------------------------------------------------------
 *
 * Send the data to each pixels.
 *
 * Arguments
 * ---------
 *  - pixmap     : Pointer to the pixel data buffer ( 1 bit per pixel )
 *  - num_pixels : Number of pixels contained in the pixel map.
 * Returns : Nothing
 */
void NeoPixel::show( uint8_t *pixmap, uint8_t num_pixels ) {

    volatile uint8_t comp = 0;
    volatile uint8_t pixels = 0;
    volatile uint8_t i_comp = 0;
    volatile uint8_t i_bit = 0;
    uint8_t i_pixel = 0;
    uint8_t i_pixel_grp = 0;


    /* Don't send anything if currently in suspend mode */
    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        return;
    }


    /* Build gamma corrected color table in GRB order */
    volatile uint8_t colorTable[3] = {
        this->getColorBrigthness( g_power.getPowerMode() == POWER_MODE_LOW_POWER ? 0 : this->_g ),
        this->getColorBrigthness( g_power.getPowerMode() == POWER_MODE_LOW_POWER ? 255 : this->_r ),
        this->getColorBrigthness( g_power.getPowerMode() == POWER_MODE_LOW_POWER ? 0 : this->_b ),
    };


    /* Get the port address and port bit from pin number */
    volatile uint8_t *port = portOutputRegister( digitalPinToPort( this->_pin_leds ) );
    uint8_t pinMask = digitalPinToBitMask( this->_pin_leds );

    volatile uint8_t high = ( *port ) | pinMask;     /* Port data with neopixel data pin set high */
    volatile uint8_t low = ( *port ) & ~pinMask;     /* Port data with neopixel data pin set low */


    /* Disable interrupts to prevent timing errors */
    noInterrupts();

    digitalWrite( this->_pin_leds, LOW );
    delayMicroseconds( 60 );


    for( i_pixel_grp = 0; i_pixel_grp <= ( ( num_pixels - 1 ) / 8 ); i_pixel_grp++ ) {
        pixels = *pixmap++;

        for( i_pixel = 0; i_pixel < 8; i_pixel++ ) {

            asm volatile(
                "next_comp:                         \n\t"
                "   inc %[i_comp]                   \n\t"
                "   cpi %[i_comp], 4                \n\t"
                "   brsh done                       \n\t"

                "   ld %[comp], %a[color]+          \n\t"

                "   clr %[i_bit]                    \n\t"

                "next_bit:                          \n\t"
                "   inc %[i_bit]                    \n\t"
                "   cpi %[i_bit], 9                 \n\t"
                "   brsh next_comp                  \n\t"

                "   rol %[comp]                     \n\t"
                "   sbrc %[pixels], 0               \n\t"
                "   brcs bit_on                     \n\t"
                "   rjmp bit_off                    \n\t"


                /* Send ON bit */
                "bit_on:                            \n\t"
                "   st %a[port], %[high]            \n\t"
                "   .rept 6                         \n\t"   // 750-1050ns high
                "       rjmp .+0                    \n\t"
                "   .endr                           \n\t"
                "   nop                             \n\t"
                "                                   \n\t"
                "   st %a[port], %[low]             \n\t"   // 200-500ns low
                "   rjmp next_bit                   \n\t"   // No need for extra delay here,
                "                                   \n\t"   // loop overhead is enough

                /* Send OFF bit */
                "bit_off:                           \n\t"   // Send
                "   st %a[port], %[high]            \n\t"
                "   rjmp .+0                        \n\t"   // 200-500ns high
                "   nop                             \n\t"
                "                                   \n\t"
                "   st %a[port], %[low]             \n\t"
                "   .rept 3                         \n\t"   // 750-1050ns low
                "       rjmp .+0                    \n\t"
                "   .endr                           \n\t"
                "   rjmp next_bit                   \n\t"
                "                                   \n\t"
                "done:                              \n\t"
                "   subi %[color], 3                \n\t"   // Reset color table pointer
                ::
                [port]	        "e"( port ),
                [high]          "d"( high ),
                [low]           "d"( low ),
                [comp]          "d"( comp ),
                [i_bit]         "d"( i_bit ),
                [i_comp]        "d"( i_comp ),
                [pixels]        "d"( pixels ),
                [color]         "e"( &colorTable )
            );

            pixels >>= 1;
        }
    }

    delayMicroseconds( 50 );

    interrupts();
}


/*--------------------------------------------------------------------------
 *
 * Set/clear pixel within the pixel map.
 *
 * Arguments
 * ---------
 *  - pixmap : Pointer to the pixel map data.
 *  - pos    : Pixel position to modify.
 *  - state  : State to assign to the pixel. (True for ON, false otherwise)
 * 
 * Returns : Nothing
 */
void NeoPixel::setPixel( uint8_t *pixmap, uint8_t pos, bool state ) {

    if( state == true ) {
        *( pixmap + ( pos / 8 ) ) |= ( 1 << ( pos % 8 ) );

    } else {

        *( pixmap + ( pos / 8 ) ) &= ~( 1 << ( pos % 8 ) );
    }
}


/*--------------------------------------------------------------------------
 *
 * Sets the brightness of the pixel string.
 *
 * Arguments
 * ---------
 *  - brightness : Brighness value 0-100 %
 *
 * Returns : Nothing
 */
void NeoPixel::setBrightness( uint8_t brightness ) {
    if( brightness > 100 ) {
        brightness = 100;
    }

    this->_brightness = brightness;
}


/*--------------------------------------------------------------------------
 *
 * Sets the pixel string color using an RGB value.
 *
 * Arguments
 * ---------
 *  - r: Red component
 *  - g: Green component
 *  - b: Blue component
 *
 * Returns : Nothing
 */
void NeoPixel::setColorRGB( uint8_t r, uint8_t g, uint8_t b ) {
    this->_r = r;
    this->_g = g;
    this->_b = b;
}


/*--------------------------------------------------------------------------
 *
 * Sets the pixel string color using the RGB table color ID.
 *
 * Arguments
 * ---------
 *  - id : value 0-12 (use color table in ressource.h)
 *
 * Returns : Nothing
 */
void NeoPixel::setColorFromTable( uint8_t id ) {

    if( id > COLOR_TABLE_MAX_COLORS - 1 ) {
        id = COLOR_TABLE_MAX_COLORS - 1;
    }

    this->_r = pgm_read_byte( &_COLOR_TABLE[ id ][ 0 ] );
    this->_g = pgm_read_byte( &_COLOR_TABLE[ id ][ 1 ] );
    this->_b = pgm_read_byte( &_COLOR_TABLE[ id ][ 2 ] );
}