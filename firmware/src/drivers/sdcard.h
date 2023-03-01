//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/sdcard.h
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
#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SdFat.h>
#include "power.h"


class SDCardManager : public SdFat {

  public:
    SDCardManager( int8_t pin_sd_detect, int8_t pin_sd_cs );
    bool isCardPresent();
    bool detectCardPresence();
    void onPowerStateChange( uint8_t state );

  private:

    int8_t _pin_sd_detect;
    int8_t _pin_sd_cs;
    bool _card_present;
    uint32_t _debouceTimer;
    uint8_t _powerState;

};

/* SD Card manager */
extern SDCardManager g_sdcard;

#endif  /* SDCARD_H */