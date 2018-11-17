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
    this->_control = TPA2016_CTRL_NG;
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::setPins( int8_t pin_shutdown ) {
    this->_pin_shutdown = pin_shutdown;
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::begin() {
    if( this->_init == true ) {
        return;
    }

    this->_init = true;

    /* Disable shutdown mode */
    if( this->_pin_shutdown >= 0 ) {
        pinMode( this->_pin_shutdown, OUTPUT );

        digitalWrite( this->_pin_shutdown, LOW );
    }


    this->write( 0x01, 0b00000000 );
    this->write( 0x02, 0 );
    this->write( 0x03, 0 );
    this->write( 0x04, 0 );
    this->write( 0x06, 0b00110101 );
    this->write( 0x07, 0b11000000 );
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::end() {

    if( this->_init == false ) {
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
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::enableOutputs() {

    this->_control |= TPA2016_CTRL_LSPK;
    this->_control |= TPA2016_CTRL_RSPK;

    this->write( TPA2016_REG_CONTROL, this->_control );
    this->write( 0x07, 0b11000000 );

}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::disableOutputs() {

    this->_control &= ~TPA2016_CTRL_LSPK;
    this->_control &= ~TPA2016_CTRL_RSPK;

    this->write( TPA2016_REG_CONTROL, this->_control );
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::setFixedGain( int8_t db ) {

    if( db < -28 ) {
        db = -28;
    }

    if( db > 30 ) {
        db = 30;
    }

    this->write( TPA2016_REG_FIXED_GAIN, db );
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::setAttackTime( int8_t time ) {


}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::setReleaseTime( int8_t time ) {

}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::setHoldTime( int8_t time ) {

}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::enableAGC( bool enabled ) {

}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::setMaxGain( int8_t db ) {

}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::setCompression( uint8_t compression ) {


}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::dumpRegs() {

    for( uint8_t i = 1; i < 8l;  i++ ) {

        Wire.beginTransmission( TPA2016_I2C_ADDR );
        Wire.write( i );
        Wire.endTransmission();

        Wire.requestFrom( TPA2016_I2C_ADDR, true );

        while( Wire.available() == false );

        Serial.print( "REG: 0x" );
        Serial.print( i, HEX );
        Serial.print( " = 0x" );
        Serial.println( Wire.read(), HEX );
    }
}


/*--------------------------------------------------------------------------
 *
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : 
 */
void TPA2016::write( uint8_t reg, uint8_t data ) {

    Wire.beginTransmission( TPA2016_I2C_ADDR );

    Wire.write( reg );
    Wire.write( data );

    Wire.endTransmission();
}