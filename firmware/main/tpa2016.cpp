#include "tpa2016.h"


TPA2016::TPA2016( uint8_t pin_shutdown ) {

    this->_pin_shutdown = pin_shutdown;

    this->_control = TPA2016_CTRL_NG;


}

void TPA2016::begin() {

    this->_init = true;


    pinMode( this->_pin_shutdown, OUTPUT );

    /* Disable shutdown mode */
    digitalWrite( this->_pin_shutdown, LOW );
    delay( 10 );


    this->write( 0x01, 0b00000000 );
    this->write( 0x02, 0 );
    this->write( 0x03, 0 );
    this->write( 0x04, 0 );
    this->write( 0x06, 0b00110101 );
    this->write( 0x07, 0b11000000 );


}


void TPA2016::enableOutputs() {

    this->_control |= TPA2016_CTRL_LSPK;
    this->_control |= TPA2016_CTRL_RSPK;

    this->write( TPA2016_REG_CONTROL, this->_control );
    this->write( 0x07, 0b11000000 );

}

void TPA2016::disableOutputs() {

    this->_control &= ~TPA2016_CTRL_LSPK;
    this->_control &= ~TPA2016_CTRL_RSPK;

    this->write( TPA2016_REG_CONTROL, this->_control );
}


void TPA2016::setFixedGain( int8_t db ) {

    if ( db < -28 ) db = -28;
    if ( db > 30 ) db = 30;

    this->write( TPA2016_REG_FIXED_GAIN , db );
}

void TPA2016::setAttackTime( int8_t time ) {


}

void TPA2016::setReleaseTime( int8_t time ) {

}

void TPA2016::setHoldTime( int8_t time ) {

}

void TPA2016::enableAGC( bool enabled ) {

}

void TPA2016::setMaxGain( int8_t db) {

}

void TPA2016::setCompression( uint8_t compression ) {


}

void TPA2016::dumpRegs() {

     for ( uint8_t i = 1; i < 8l;  i++ ) {

        Wire.beginTransmission( TPA2016_I2C_ADDR );
        Wire.write( i );
        Wire.endTransmission();

        Wire.requestFrom( TPA2016_I2C_ADDR, true );
        while( Wire.available() == false );

        Serial.print("REG: 0x");
        Serial.print( i, HEX );
        Serial.print(" = 0x");
        Serial.println( Wire.read(), HEX );
    }

}




void TPA2016::write( uint8_t reg, uint8_t data ) {

    Wire.beginTransmission( TPA2016_I2C_ADDR );

    Wire.write( reg );
    Wire.write( data );

    Wire.endTransmission();
}
