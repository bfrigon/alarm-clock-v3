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

#include <resources.h>

#include "neopixel.h"
#include "power.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 * @param   pin_leds    Pin ID connected to the neopixel data line.
 * @param   pin_shdn    Pin ID connected to the neopixel power MOSFET.
 * 
 */
NeoPixel::NeoPixel( int8_t pin_leds, int8_t pin_shdn ) {
    _pin_leds = pin_leds;
    _pin_shdn = pin_shdn;

    _ambientDimming = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize pins for the LED string.
 * 
 */
void NeoPixel::begin() {
    if( _init == true ) {
        return;
    }

    _init = true;

    pinMode( _pin_leds, OUTPUT );

    if( _pin_shdn >= 0 ) {
        pinMode( _pin_shdn, OUTPUT );
    }

    this->onPowerStateChange( g_power.getPowerMode() );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Disable power for the LED string.
 * 
 */
void NeoPixel::end() {
    if( _init == false ) {
        return;
    }

    _init = false;

    if( _pin_shdn >= 0 ) {
        digitalWrite( _pin_shdn, HIGH );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Disable/enable power for the leds based on the current system 
 *          power state
 *
 * @param   state    Current power state
 * 
 */
void NeoPixel::onPowerStateChange( uint8_t state ) {

    if( _init == false ) {
        return;
    }

    if( _pin_shdn >= 0 ) {
        digitalWrite( _pin_shdn, ( state == POWER_MODE_SUSPEND ? HIGH : LOW ) );
    }

    this->update();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Calculate the pixel brightness and apply gamma correction for an 
 *          individual color channel.
 *
 * @param   color    Color channel value.
 *
 * @return  The corrected brightness value.
 * 
 */
inline uint8_t NeoPixel::getColorBrigthness( uint8_t color ) {

    uint8_t brightness = _brightness;

    /* Apply ambiant light dimming percentage */
    brightness = brightness * ( 100 - _ambientDimming ) / 100;

    /* Keep the ambient dimming from turning the pixel off completely */
    if( _brightness > 0 && brightness < 1 ) {
        brightness = 1;
    }

    if( g_power.getPowerMode() == POWER_MODE_LOW_POWER ) {

        /* limit brightness in low power mode */
        if( brightness > 25 ) {
            brightness = 25;
        }
    }

    color = ( ( uint16_t )color * ( uint16_t )brightness ) / 100;

    return color;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Send the data to each pixels.
 *
 * @param   pixmap        Pointer to the pixel data buffer ( 1 bit per pixel )
 * @param   num_pixels    Number of pixels contained in the pixel map.
 * 
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
        this->getColorBrigthness( g_power.getPowerMode() == POWER_MODE_LOW_POWER ? 0 : _g ),
        this->getColorBrigthness( g_power.getPowerMode() == POWER_MODE_LOW_POWER ? 255 : _r ),
        this->getColorBrigthness( g_power.getPowerMode() == POWER_MODE_LOW_POWER ? 0 : _b ),
    };


    /* Get the port address and port bit from pin number */
    volatile uint8_t *port = portOutputRegister( digitalPinToPort( _pin_leds ) );
    uint8_t pinMask = digitalPinToBitMask( _pin_leds );

    volatile uint8_t high = ( *port ) | pinMask;     /* Port data with neopixel data pin set high */
    volatile uint8_t low = ( *port ) & ~pinMask;     /* Port data with neopixel data pin set low */


    /* Disable interrupts to prevent timing errors */
    noInterrupts();

    digitalWrite( _pin_leds, LOW );
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Set/clear pixel within the pixel map.
 *
 * @param   pixmap    Pointer to the pixel map data.
 * @param   pos       Pixel position to modify.
 * @param   state     State to assign to the pixel. 
 *                    (TRUE for ON, FALSE otherwise)
 * 
 */
void NeoPixel::setPixel( uint8_t *pixmap, uint8_t pos, bool state ) {

    if( state == true ) {
        *( pixmap + ( pos / 8 ) ) |= ( 1 << ( pos % 8 ) );

    } else {

        *( pixmap + ( pos / 8 ) ) &= ~( 1 << ( pos % 8 ) );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the brightness of the pixel string.
 *
 * @param   brightness    Brighness value 0-100 %
 * 
 */
void NeoPixel::setBrightness( uint8_t brightness ) {
    if( brightness > 100 ) {
        brightness = 100;
    }

    _brightness = brightness;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the ambient dimming percentage
 *
 * @param   dimming    Ambient dimming percentage ( 0-100 )
 * 
 */
void NeoPixel::setAmbientDimming( uint8_t dimming ) {

    if( dimming > 100 ) {
        dimming = 100;
    }

    if( dimming != _ambientDimming ) {

        _ambientDimming = dimming;
        this->update();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the pixel string color using an RGB value.
 *
 * @param   r    Red component
 * @param   g    Green component
 * @param   b    Blue component
 * 
 */
void NeoPixel::setColorRGB( uint8_t r, uint8_t g, uint8_t b ) {
    _r = r;
    _g = g;
    _b = b;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the pixel string color using the RGB table color ID.
 *
 * @param   id    value 0-12 (use color table in ressource.h)
 * 
 */
void NeoPixel::setColorFromTable( uint8_t id ) {

    if( id > COLOR_TABLE_MAX_COLORS - 1 ) {
        id = COLOR_TABLE_MAX_COLORS - 1;
    }

    _r = pgm_read_byte( &_COLOR_TABLE[ id ][ 0 ] );
    _g = pgm_read_byte( &_COLOR_TABLE[ id ][ 1 ] );
    _b = pgm_read_byte( &_COLOR_TABLE[ id ][ 2 ] );
}