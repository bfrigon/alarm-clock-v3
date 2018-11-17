//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/lamp.h
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
#ifndef LAMP_H
#define LAMP_H

#include <Arduino.h>

#include "../config.h"
#include "neopixel.h"



#define LAMP_MODE_OFF       0
#define LAMP_MODE_ON        1
#define LAMP_MODE_FLASHING  2
#define LAMP_MODE_FADING    3
#define LAMP_MODE_RAINBOW   4
#define LAMP_MODE_FADEOUT   100


#define LAMP_MIMIMUM_FADING_BRIGHTNESS  20



class Lamp : public NeoPixel {
  public:

    /* Constructor */
    Lamp( int8_t pin_leds );


    void setBrightness( uint8_t brightness );
    void setColorFromTable( uint8_t id );
    void setColorRGB( uint8_t r, uint8_t g, uint8_t b );
    void setEffectSpeed( uint8_t speed );
    void activate( struct NightLampSettings *settings, bool test_mode = false );
    void deactivate();
    void processEvents();
    void update();

  private:
    uint8_t _delay_off = 0;
    uint8_t _mode = LAMP_MODE_OFF;

    uint32_t _timerStart = 0;

    uint16_t _visualStepDelay = 0;
    uint8_t _visualStepValue = 0;
    uint8_t _visualStepSpeed = 5;
    bool _visualStepReverse = false;

    struct NightLampSettings *_settings;


    void updateVisualStepDelay();
};

extern Lamp g_lamp;

#endif /* LAMP_H */