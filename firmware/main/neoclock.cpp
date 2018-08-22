//******************************************************************************
//
// Project : Alarm Clock V3
// File    : neoclock.cpp
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

#include <Arduino.h>

#include "neoclock.h"
#include "config.h"
#include "time.h"
#include "resources.h"





NeoClock::NeoClock( uint8_t pin_leds ) {
    this->_pin_leds = pin_leds;

}

void NeoClock::begin() {
    this->_init = true;

    pinMode( this->_pin_leds, OUTPUT );

    this->_flashTimerStart = millis();
}

void NeoClock::update() {

    if ( this->_init == false ) {
        this->begin();
    }

    bool is_pm = false;
    uint8_t hour = this->hour;
    if (( g_config.clock_24h == false ) && ( this->hour != 0xFF )) {

        if ( hour > 12 )
            hour -= 12;

        if ( hour == 0 )
            hour = 12;

        is_pm = ( this->hour > 11 );
    }

    uint8_t pixmap[5];
    uint8_t colors[3] = {
        pgm_read_byte( &_GAMMA_TABLE[ this->_g * this->_brightness / 100 ]),
        pgm_read_byte( &_GAMMA_TABLE[ this->_r * this->_brightness / 100 ]),
        pgm_read_byte( &_GAMMA_TABLE[ this->_b * this->_brightness / 100 ])
    };


    this->setPixel( pixmap, 0, is_pm );
    this->setPixel( pixmap, 1, this->status_set );

    if ( this->hourFlashing == true && this->_flashState == false ) {
        this->setDigitPixels( pixmap, 2, SEG_SPACE );
        this->setDigitPixels( pixmap, 9, SEG_SPACE );
    } else if ( this->hour == 0xFF ) {
        this->setDigitPixels( pixmap, 2, SEG_MINUS );
        this->setDigitPixels( pixmap, 9, SEG_MINUS );
    } else {
        this->setDigitPixels( pixmap, 2, ( hour > 9 ) ? hour / 10 : SEG_SPACE );
        this->setDigitPixels( pixmap, 9, hour % 10 );
    }

    this->setPixel( pixmap, 16, true );
    this->setPixel( pixmap, 17, true );

    if ( this->minutesFlashing == true && this->_flashState == false ) {
        this->setDigitPixels( pixmap, 18, SEG_SPACE );
        this->setDigitPixels( pixmap, 25, SEG_SPACE );
    } else if ( this->minute == 0xFF ) {
        this->setDigitPixels( pixmap, 18, SEG_MINUS );
        this->setDigitPixels( pixmap, 25, SEG_MINUS );
    } else {
        this->setDigitPixels( pixmap, 18, this->minute / 10 );
        this->setDigitPixels( pixmap, 25, this->minute % 10 );
    }

    this->setPixel( pixmap, 32, g_config.alarm_on[0] );
    this->setPixel( pixmap, 33, g_config.alarm_on[1] );


    this->show( pixmap, colors );
}


void NeoClock::processUpdateEvents() {

    if ( millis() - this->_flashTimerStart > ( this->flashRate * 10 )) {
        this->_flashTimerStart = millis();
        this->_flashState = !( this->_flashState );

        if ( this->hourFlashing == true || this->minutesFlashing == true ) {
            g_clockUpdate = true;
        }
    }


    if ( g_clockUpdate == true ) {

        g_clock.update();
        g_clockUpdate = false;
    }
}

void NeoClock::show( uint8_t *pixmap, uint8_t *color ) {

    volatile uint8_t comp = 0;
    volatile uint8_t pixels = 0;
    volatile uint8_t i_comp = 0;
    volatile uint8_t i_bit = 0;
    uint8_t i_pixel = 0;
    uint8_t i_pixel_grp = 0;


    /* Disable interrupts to prevent timing errors */
    noInterrupts();

    for ( i_pixel_grp = 0; i_pixel_grp < 5; i_pixel_grp++ ) {
        pixels = *pixmap++;

        for ( i_pixel = 0; i_pixel < 8; i_pixel++ ) {

            asm volatile (
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
                "   sbi %[pin_port], %[pin_bit]     \n\t"
                "   .rept 6                         \n\t"   // 750-1050ns high
                "       rjmp .+0                    \n\t"
                "   .endr                           \n\t"
                "   nop                             \n\t"
                "                                   \n\t"
                "   cbi %[pin_port], %[pin_bit]     \n\t"   // 200-500ns low
                "   rjmp next_bit                   \n\t"   // No need for extra delay here,
                "                                   \n\t"   // loop overhead is enough

                /* Send OFF bit */
                "bit_off:                           \n\t"   // Send
                "   sbi %[pin_port], %[pin_bit]     \n\t"
                "   rjmp .+0                        \n\t"   // 200-500ns high
                "   nop                             \n\t"
                "                                   \n\t"
                "   cbi %[pin_port], %[pin_bit]     \n\t"
                "   .rept 3                         \n\t"   // 750-1050ns low
                "       rjmp .+0                    \n\t"
                "   .endr                           \n\t"
                "   rjmp next_bit                   \n\t"
                "                                   \n\t"
                "done:                              \n\t"
                "   subi %[color], 3                \n\t"   // Reset color component pointer

            ::
                [pin_port]	    "I" ( _SFR_IO_ADDR( PORTC)  ),
                [pin_bit]       "I" ( 0 ),
                [comp]          "r" ( comp ),
                [i_bit]         "r" ( i_bit ),
                [i_comp]        "r" ( i_comp ),
                [pixels]        "r" ( pixels ),
                [color]         "e" ( color )
            );

            pixels >>= 1;
         }
    }

    delayMicroseconds( 50 );

    interrupts();
}


void NeoClock::setPixel( uint8_t *pixmap, uint8_t id, bool state ) {

    if ( state ) {
        *(pixmap + ( id / 8 )) |= ( 1 << ( id % 8 ));

    } else {

        *(pixmap + ( id / 8 )) &= ~( 1 << ( id % 8 ));
    }
}

void NeoClock::setDigitPixels( uint8_t *pixmap, uint8_t pos, uint8_t value ) {

    uint8_t chr = pgm_read_byte( &_charmap[ value ] );

    for ( uint8_t i = 0; i < 8; i++ ) {
        this->setPixel( pixmap, pos + i, (chr & ( 1 << i )));
    }
}

void NeoClock::setBrightness( uint8_t brightness ) {
    if ( brightness > 100 ) {
        brightness = 100;
    }

    this->_brightness = brightness;
}


void NeoClock::setColorRGB( uint8_t r, uint8_t g, uint8_t b ) {

    this->_r = r;
    this->_g = g;
    this->_b = b;
}


void NeoClock::setColorFromTable( uint8_t id ) {

    if ( id > COLOR_TABLE_MAX_COLORS - 1 ) {
        id = COLOR_TABLE_MAX_COLORS - 1;
    }

    this->_r = pgm_read_byte( &_COLOR_TABLE[ id ][ 0 ] );
    this->_g = pgm_read_byte( &_COLOR_TABLE[ id ][ 1 ] );
    this->_b = pgm_read_byte( &_COLOR_TABLE[ id ][ 2 ] );
}
