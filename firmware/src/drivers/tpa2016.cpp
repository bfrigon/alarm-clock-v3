//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/tpa2016.cpp
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
#include "tpa2016.h"


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  None
 *
 */
TPA2016::TPA2016() {
    _control = TPA2016_CTRL_NG_ON | TPA2016_CTRL_LSPK_OFF | TPA2016_CTRL_RSPK_OFF;
    _attackTime = 5;
    _releaseTime = 11;
    _holdTime = 0;
    _fixedGain = 6;
    _outputLimiterDisabled = false;
    _noiseGateThreshold = TPA2016_AGC_NG_THRESHOLD_4;
    _outputLimiterLevel = 26;
    _maxGain = 30;
    _compression = TPA2016_COMPRESSION_4_1;

}


/*--------------------------------------------------------------------------
 *
 * Set hardware pins
 *
 * Arguments
 * ---------
 *  - pin_shutdown
 *
 * Returns : Nothing
 */
void TPA2016::setPins( int8_t pin_shutdown ) {
    this->_pin_shutdown = pin_shutdown;
}


/*--------------------------------------------------------------------------
 *
 * Initialize the amplifier IC
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void TPA2016::begin() {
    _init = true;

    /* Disable shutdown mode */
    if( this->_pin_shutdown >= 0 ) {
        pinMode( this->_pin_shutdown, OUTPUT );

        digitalWrite( this->_pin_shutdown, LOW );
    }

    /* Sets all config registers with default values */
    this->write( TPA2016_REG_CONTROL, _control );
    this->setAttackTime( _attackTime );
    this->setReleaseTime( _releaseTime );
    this->setHoldTime( _holdTime );
    this->setFixedGain( _fixedGain );
    this->setLimiter( _outputLimiterDisabled, _outputLimiterLevel );
    this->setCompression( _compression );
}


/*--------------------------------------------------------------------------
 *
 * Deinitialize the amplifier IC
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::end() {

    if( _init == false ) {
        return;
    }

    this->_init = false;

    /* Shutdown mode */
    if( this->_pin_shutdown >= 0 ) {
        digitalWrite( this->_pin_shutdown, HIGH );
    }
}


/*--------------------------------------------------------------------------
 *
 * Enable left and right speaker outputs.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::enableOutputs() {

    if( _init == false ) {
        return false;
    }

    this->_control |= TPA2016_CTRL_LSPK_ON;
    this->_control |= TPA2016_CTRL_RSPK_ON;

    return this->write( TPA2016_REG_CONTROL, this->_control );
}


/*--------------------------------------------------------------------------
 *
 * Disable left and right speaker outputs.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::disableOutputs() {

    if( _init == false ) {
        return false;
    }

    this->_control &= ~TPA2016_CTRL_LSPK_ON;
    this->_control &= ~TPA2016_CTRL_RSPK_ON;

    return this->write( TPA2016_REG_CONTROL, this->_control );
}


/*--------------------------------------------------------------------------
 *
 * Sets the fixed gain of the amplifier.
 *
 * Arguments
 * ---------
 *  - db : Gain value in decibel. Valid range -28dB to +30dB
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::setFixedGain( int8_t db ) {

    if( _init == false ) {
        return false;
    }

    if( db < -28 ) {
        db = -28;
    }

    if( db > 30 ) {
        db = 30;
    }

    _fixedGain = db;

    return this->write( TPA2016_REG_FIXED_GAIN, db );
}


/*--------------------------------------------------------------------------
 *
 * Sets the auto gain control attack time.
 *
 * Arguments
 * ---------
 *  - time : Number of steps 0-63. Each steps represents 0.1067 milliseconds.
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::setAttackTime( int8_t time ) {

    if( _init == false ) {
        return false;
    }

    if( time > 63 ) {
        time = 63;
    }

    _attackTime = time;

    return this->write( TPA2016_REG_AGC_ATTACK, time );
}


/*--------------------------------------------------------------------------
 *
 * Sets the auto gain control release time.
 *
 * Arguments
 * ---------
 *  - time : Number of steps 0-63. Each steps represents 0.0137 seconds.
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::setReleaseTime( int8_t time ) {

    if( _init == false ) {
        return false;
    }

    if( time > 63 ) {
        time = 63;
    }

    _releaseTime = time;

    return this->write( TPA2016_REG_AGC_RELEASE, time );
}


/*--------------------------------------------------------------------------
 *
 * Sets the auto gain control hold time.
 *
 * Arguments
 * ---------
 *  - time : Number of steps 0-63. Each steps represents 0.0137 seconds.
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::setHoldTime( int8_t time ) {

    if( _init == false ) {
        return false;
    }

    if( time > 63 ) {
        time = 63;
    }

    _holdTime = time;

    return this->write( TPA2016_REG_AGC_HOLD, time );
}



/*--------------------------------------------------------------------------
 *
 * Sets the maximum gain the auto gain control can acheive.
 *
 * Arguments
 * ---------
 - db : Value in decibel. Valid range from 18 dB to 30 dB
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::setMaxGain( int8_t db ) {

    if( _init == false ) {
        return false;
    }

    if( db > 30 ) {
        db = 30;
    }

    if ( db < 18 ) {
        db = 18;
    }

    _maxGain = db;

    /* Stored in the same register as compression ratio */
    return this->setCompression( _compression );

}


/*--------------------------------------------------------------------------
 *
 * Sets the AGC compression ratio
 *
 * Arguments
 * ---------
 *  - compression : 0=Off, 1=2:1, 2=4:1, 3=8:1
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::setCompression( uint8_t compression ) {

    if( _init == false ) {
        return false;
    }

    if( compression > 3 ) {
        compression = 3;
    }

    _compression = compression;

    

    int8_t reg;
    reg = _compression | ((( _maxGain - 18 ) & 0xF) << 4 );

    return this->write( TPA2016_REG_AGC_CTRL2, reg );
}


/*--------------------------------------------------------------------------
 *
 * Sets the output limiter level. 
 *
 * Arguments
 * ---------
 *  - disabled : TRUE to disable the output limiter function, otherwise
 *               enabled by default.
 *  - level    : Limiter level in 0.5 dB steps from -6.5 dBV. 
 *               0=-6.5dBV, 31=+9dBV
 * 
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::setLimiter( bool disabled, int8_t level ) {

    if( _init == false ) {
        return false;
    }

    if( level > 31 ) {
        level = 31;
    }

    int8_t reg;
    reg = level | (( _noiseGateThreshold & 0x3 ) << 5 );

    if( disabled ) {
        reg |= TPA2016_AGC_LIMITER_DISABLED;
    }

    return this->write( TPA2016_REG_AGC_CTRL1, reg );
}


/*--------------------------------------------------------------------------
 *
 * Prints the contents of the config register to the serial port
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void TPA2016::dumpRegs() {

    for( uint8_t i = 1; i < 8l;  i++ ) {

        Wire.beginTransmission( TPA2016_I2C_ADDR );
        Wire.write( i );
        Wire.endTransmission();

        Wire.requestFrom( TPA2016_I2C_ADDR, true );

        while( Wire.available() == false );

        // Serial.print( "REG: 0x" );
        // Serial.print( i, HEX );
        // Serial.print( " = 0x" );
        // Serial.println( Wire.read(), HEX );
    }
}


/*--------------------------------------------------------------------------
 *
 * Write data to a config register
 *
 * Arguments
 * ---------
 *  - reg  : Register address
 *  - data : Data to write
 *
 * Returns : TRUE if successful, FALSE otherwise
 */
bool TPA2016::write( uint8_t reg, uint8_t data ) {

    Wire.beginTransmission( TPA2016_I2C_ADDR );

    uint8_t ret;
    ret = Wire.write( reg );
    if( ret < 1 ) {
        return false;
    }

    ret = Wire.write( data );
    if( ret < 1 ) {
        return false;
    }

    Wire.endTransmission();

    return true;
}