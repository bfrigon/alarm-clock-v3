//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/neopixel.h
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
#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <Arduino.h>
#include <avr/pgmspace.h>



/*******************************************************************************
 *
 * @brief   NeoPixel driver class
 * 
 *******************************************************************************/
class NeoPixel {
  
  public:
    NeoPixel( int8_t pin_leds, int8_t pin_shdn );
    void begin();
    void end();
    void onPowerStateChange( uint8_t state );
    void setColorRGB( uint8_t r, uint8_t g, uint8_t b );
    void setColorFromTable( uint8_t id );
    void setBrightness( uint8_t brightness );
    void setAmbientDimming( uint8_t dimming );
    virtual void update() = 0;


  protected:
    void show( uint8_t *pixmap, uint8_t num_leds );
    void setPixel( uint8_t *pixmap, uint8_t id, bool state );
    inline uint8_t getColorBrigthness( uint8_t color );

    int8_t _pin_leds;
    int8_t _pin_shdn;
    uint8_t _brightness = 10;
    uint8_t _g = 0x00;
    uint8_t _b = 0x00;
    uint8_t _r = 0xFF;
    uint8_t _ambientDimming = 0;
    bool _init = false;
};

#endif /* NEOPIXEL_H */