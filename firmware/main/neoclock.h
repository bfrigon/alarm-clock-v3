//******************************************************************************
//
// Project : Alarm Clock V3
// File    : neoclock.h
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

#ifndef NEOCLOCK_H
#define NEOCLOCK_H

#include <Arduino.h>
#include <avr/pgmspace.h>



/* TODO: Could not figure out a way to pass pin number to inline asm */
#define NEOCLOCK_PORT   PORTC
#define NEOCLOCK_PIN    0




#define SEG_MINUS  0x0A
#define SEG_SPACE  0x0B

extern bool g_clockUpdate;


static const uint8_t PROGMEM _charmap[ 12 ] = {
    0b00111111,  /* Digit 0 */
    0b00000110,  /* Digit 1 */
    0b01011011,  /* Digit 2 */
    0b01001111,  /* Digit 3 */
    0b01100110,  /* Digit 4 */
    0b01101101,  /* Digit 5 */
    0b01111101,  /* Digit 6 */
    0b00000111,  /* Digit 7 */
    0b01111111,  /* Digit 8 */
    0b01101111,  /* Digit 9 */
    0b01000000,  /* Minus */
    0b00000000   /* Space */
};





class NeoClock {
  public:

    uint8_t hour = 0xFF;
    uint8_t minute = 0xFF;
    uint8_t flashRate = 20;
    bool status_set = false;
    uint8_t pixmap[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

    bool hourFlashing = false;
    bool minutesFlashing = false;

    



    /* Constructor */
    NeoClock( uint8_t pin_leds );

    void begin();
    void update();

    void setColorRGB( uint8_t r, uint8_t g, uint8_t b );
    void setColorFromTable( uint8_t id );
    void setBrightness( uint8_t id );

    void processUpdateEvents();



  private:
    bool _init = false;

    uint8_t _pin_leds;
    uint8_t _brightness = 10;
    uint8_t _g = 0x00;
    uint8_t _b = 0x00;
    uint8_t _r = 0xFF;

    uint32_t _flashTimerStart = 0;
    bool     _flashState = false;

    
    

    void show( uint8_t *pixmap, uint8_t *colors );
    void setPixel( uint8_t *pixmap, uint8_t id, bool state );
    void setDigitPixels( uint8_t *pixmap, uint8_t pos, uint8_t value );

};

extern NeoClock g_clock;

#endif /* NEOCLOCK_H */
