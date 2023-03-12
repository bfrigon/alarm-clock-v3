//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/sdcard.cpp
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

#include "sdcard.h"
#include <services/logger.h>
#include <drivers/rtc.h>
#include <timezone.h>


/*******************************************************************************
 *
 * @brief   Class constructor
 *
 * @param   pin_sd_detect    SD card detect pin.
 * @param   pin_sd_cs        SD card chip select pin.
 * 
 */
SDCardManager::SDCardManager( int8_t pin_sd_detect, int8_t pin_sd_cs ) : SdFat() {

    _pin_sd_cs = pin_sd_cs;
    _pin_sd_detect = pin_sd_detect;
    _card_present = false;
    _debouceTimer = 0;
    _powerState = POWER_MODE_NORMAL;

    pinMode( pin_sd_detect, INPUT );


    FsDateTime::setCallback( sdCardDateTimeCallback );
}


/*******************************************************************************
 *
 * @brief   Return whether the SD card was detected or not.
 *
 * @return  TRUE if detected, FALSE otherwise.
 * 
 */
bool SDCardManager::isCardPresent() {
    return _card_present;
}


/*******************************************************************************
 *
 * @brief   Check the current state of the SD card detect pin. If a card 
 *          is found, initialize the SdFat library.
 *
 * @return  TRUE if detected, FALSE otherwise.
 * 
 */
bool SDCardManager::detectCardPresence() {

    /* Card detected */
    if( digitalRead( _pin_sd_detect ) == LOW ) {

        /* Already detected, no need to re-initialize. */
        if( _card_present == true ) {
            return true;
        }

        if( _powerState == POWER_MODE_SUSPEND ) {
            if( _debouceTimer == 0 ) {

                /* millis() not usable in suspend mode, debounce using wake-up counter. */
                _debouceTimer = 1;
                return false;
            }

        } else {

            /* Start the debouce timer. */
            if( _debouceTimer == 0 ) {
                _debouceTimer = millis();
                return false;
            }

            /* Failed to detect SD card. */
            if( ( millis() - _debouceTimer ) < 1000 ) {
                return false;
            }   
        }     

        /* The card was detected, now we can initialize or re-initalize it. */
        _card_present = true;

    } else {

        /* Already not present, do nothing. */
        if( _card_present == false ) {
            return false;
        }

        this->end();

        g_log.add( EVENT_SD_REMOVED );

        _debouceTimer = 0;
        _card_present = false;
        return false;
    }

    _debouceTimer = 0;
    _card_present = false;    

    if( this->begin( _pin_sd_cs ) == false ) {

        /* Failed to initialize SD card */
        g_log.add( EVENT_SD_INIT_FAIL, this->sdErrorCode() );
        return false;
    }

    g_log.add( EVENT_SD_READY );
    
    _card_present = true;
    return true;
}


/*******************************************************************************
 *
 * @brief   Called when system power state changes.
 * 
 * @param   state   New power state
 * 
 */
void SDCardManager::onPowerStateChange( uint8_t state ) {

    _powerState = state;
}


/*******************************************************************************
 *
 * @brief   Callback function used by the SdFat library to update the timestamp 
 *          when a file is created or written. Return the current date/time
 * 
 * @param   date    Pointer to the variable to store the compacted time value.
 * @param   time    Pointer to the variable to store the compacted date value.
 * @param   ms10    Pointer to the variable to store the low time bits in 
 *                  units of 10ms
 * 
 */
void sdCardDateTimeCallback( uint16_t* date, uint16_t* time, uint8_t* ms10 ) {

    DateTime now = g_rtc.now();
    
    /* Return date using FS_DATE macro to format fields. */
    *date = FS_DATE( now.year(), now.month(), now.day() );

    /* Return time using FS_TIME macro to format fields. */
    *time = FS_TIME( now.hour(), now.minute(), now.second() );

    /* Return low time bits in units of 10 ms, 0 <= ms10 <= 199. */
    *ms10 = now.second() & 1 ? 100 : 0;
}