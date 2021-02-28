//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/als.cpp
// Author  : Benoit Frigon <www.frigon.info>
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

#include <config.h>

#include "als.h"
#include "power.h"
#include "neoclock.h"
#include "us2066.h"



/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 * 
 */
ALS::ALS() {
    _init = false;
    _lastIntegrationStart = millis();
    _lastValueChange = millis();    
    _currentAmbientDimming = 0;
    _targetAmbientDimming = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the TSL2591 IC
 * 
 */
void ALS::begin() {
    this->resume();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Power down the TSL2591 IC
 * 
 */
void ALS::suspend() {
    if( _init == false ) {
        return;
    }

    /* Power off the TSL2591 oscillator and timers */
    this->writeByte( TSL2591_REG_ENABLE, TSL2591_ENABLE_OFF );

    _init = false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Re-initialize the TSL2591 IC
 * 
 */
void ALS::resume() {

    /* Check device ID to confirm that the device is present */
    if( this->readByte( TSL2591_REG_ID ) != 0x50 ) {

        _init = false;
        return;
    }

    /* Sets the gain and integration time */
    this->configure( TSL2591_GAIN_HIGH, TSL2591_INTEGRATION_500MS );

    /* Enable TSL2591 oscillator and timers */
    this->writeByte( TSL2591_REG_ENABLE, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN );

    _lastIntegrationStart = millis();
    _lastValueChange = millis();
    
    _init = true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the sensor gain and integration time
 * 
 * @param   gain           ADC gain
 * @param   integration    Integration time
 * 
 */
bool ALS::configure( uint8_t gain, uint8_t integration ) {

    if( integration > TSL2591_INTEGRATION_MAX ) {
        integration = TSL2591_INTEGRATION_MAX;
    }

    if( gain > TSL2591_GAIN_MAXIMUM ) {
        gain = TSL2591_GAIN_MAXIMUM;
    }

    uint8_t cfg = ( ( gain & 0x03 ) << 4 ) | ( integration & 0x07 ) ;

    return this->writeByte( TSL2591_REG_CONFIG, cfg );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Called when system power state changes 
 * 
 * @param   state   New power state
 * 
 */
void ALS::onPowerStateChange( uint8_t state ) {

    if( state == POWER_MODE_SUSPEND ) {
        this->suspend();

    } else {
        this->resume();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Monitor ambient lighting for changes and set the display 
 *          brightness accordingly.
 * 
 */
void ALS::processEvents() {
    if( _init == false ) {
        return;
    }
    
    if( millis() - _lastIntegrationStart >= 500 ) {

        _lastIntegrationStart = millis();

        /* Calculate required dimming */
        uint8_t dimming = this->calculateAmbientDimming();

        /* Check if the dimming value is outside stable range */
        if( _targetAmbientDimming < ( dimming < ALS_STABLE_RANGE ? 0 : dimming - ALS_STABLE_RANGE ) || 
            _targetAmbientDimming > min( dimming + ALS_STABLE_RANGE, 100 )) {

            _targetAmbientDimming = dimming;
            _lastValueChange = millis();
        }
    }

    /* Only change the brightness when the target dimming value has 
       been stable for the required amount of time */
    if( millis() - _lastValueChange > ALS_MINIMUM_STABLE_DELAY && ( millis() - _lastValueChange ) % ALS_FADE_STEPS_MS == 0 ) {

        /* gradually bring the current dimming value to the target value  */ 
        if( _targetAmbientDimming > _currentAmbientDimming ) {
            _currentAmbientDimming++;

        } else if( _targetAmbientDimming < _currentAmbientDimming ) {
            _currentAmbientDimming--;
        }

        if( _currentAmbientDimming != _targetAmbientDimming ) {

            /* Update clock and lcd ambient dimming values */
            g_clock.setAmbientDimming( _currentAmbientDimming );
            g_lcd.setAmbientDimming( _currentAmbientDimming );
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Calculate the required dimming percentage relative to the 
 *          perceived ambient light. 
 * 
 * @details The maximum dimming value is determined by the choosen ALS
 *          preset. The preset also determine the threshold at which it 
 *          begins dimming. 
 *
 * @return  Percentage of dimming required (0 - preset maximum)
 * 
 */
uint8_t ALS::calculateAmbientDimming() {

    if( _init == false ) {
        return 0;
    }

    /* Read the count from both channels (visible+ir and ir ) */
    uint16_t vis_ir = this->readWord( TSL2591_REG_C0DATAL );
    uint16_t ir = this->readWord( TSL2591_REG_C1DATAL );

    /* If either one of the channel overflow, no dimming required */
    if(( vis_ir == 0xFFFF ) | ( ir == 0xFFFF )) {
        return 0;
    }

    /* Estimate perceived room brightness */
    uint16_t perceived;
    perceived = ( vis_ir > ir ) ? vis_ir - ir : 0;

    /* Set the dark threshold and maximum dimming value
       from the selected preset */
    uint16_t dark_threshold;
    uint8_t max_dimming;
    uint8_t min_perceived;

    switch( g_config.clock.als_preset ) {

        case ALS_PRESET_LOW:
            dark_threshold = 150;
            min_perceived = 15;
            max_dimming = 60;
            break;

        case ALS_PRESET_MEDIUM:
            dark_threshold = 300;
            min_perceived = 15;
            max_dimming = 80;
            break;

        case ALS_PRESET_HIGH:
            dark_threshold = 600;
            min_perceived = 30;
            max_dimming = 95;
            break;

        /* ALS_PRESET_OFF */
        default:
            return 0;
    }


    if( perceived < min_perceived ) {
        perceived = min_perceived;
    }

    /* Calculate the dimming value relative to the perceived brightness bellow the
       defined dark threshold */
    if( perceived < dark_threshold ) {
        return map( perceived, min_perceived, dark_threshold, max_dimming, 0 );
    } else {
        return 0;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Write a single byte at the specified address
 * 
 * @param   address    Address to write to
 * @param   value      The value to write
 * 
 * @return  TRUE if successful, FALSE otherwise.
 */
bool ALS::writeByte( uint8_t address, uint8_t value ) {

    Wire.beginTransmission( TSL2591_I2C_ADDR );

    Wire.write( TSL2591_COMMAND_SELECT | TSL2591_TRANSACTION_NORMAL | ( address & 0x1F ) );
    Wire.write( value );

    return ( Wire.endTransmission() == 0 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Read a single byte from given address
 * 
 * @param   address    Register address to read from
 * 
 * @return  Value at the specified address
 * 
 */
uint8_t ALS::readByte( uint8_t address ) {

    Wire.beginTransmission( TSL2591_I2C_ADDR );
    Wire.write( TSL2591_COMMAND_SELECT | TSL2591_TRANSACTION_NORMAL | ( address & 0x1F ) );
    Wire.endTransmission();

    Wire.requestFrom( TSL2591_I2C_ADDR, 1 );

    return Wire.read();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Read a 16 bits(word) value starting at the given address
 * 
 * @param   address    Register address to read from
 * 
 * @return  16 bits value at the specified address.
 * 
 */
uint16_t ALS::readWord( uint8_t address ) {
    Wire.beginTransmission( TSL2591_I2C_ADDR );
    Wire.write( TSL2591_COMMAND_SELECT | TSL2591_TRANSACTION_NORMAL | ( address & 0x1F ) );
    Wire.endTransmission();

    Wire.requestFrom( TSL2591_I2C_ADDR, 2 );

    uint8_t buffer[2];

    Wire.readBytes( buffer, sizeof( buffer ) );

    return buffer[1] << 8 | buffer[0];
}