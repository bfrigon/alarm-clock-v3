//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/tsl2591.cpp
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
#include "tsl2591.h"
#include "power.h"


TSL2591::TSL2591() {

}

void TSL2591::begin() {
    this->resume();
    
}


void TSL2591::suspend() {
    this->sendCommand( TSL2591_REG_ENABLE, TSL2591_ENABLE_OFF );
}

void TSL2591::resume() {
    this->sendCommand( TSL2591_REG_ENABLE, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN );

    this->_lastIntegrationStart = millis();
}


void TSL2591::onPowerStateChange( uint8_t state ) {

    if( state == POWER_MODE_SUSPEND ) {
        this->suspend();

    } else {
        this->resume();
    }
}


void TSL2591::runTask() {




    if( millis() - this->_lastIntegrationStart < 1000 ) {
        return;
    }

    this->_lastIntegrationStart = millis();

    uint16_t vis = this->readWord( TSL2591_REG_C0DATAL );
    uint16_t ir = this->readWord( TSL2591_REG_C1DATAL );

    //Serial.println( vis );

}


void TSL2591::configure( uint8_t gain, uint8_t integration ) {

    uint8_t cfg = ( ( gain & 0x03 ) << 4 ) | ( integration & 0x07 ) ;

    this->sendCommand( TSL2591_REG_CONFIG, cfg );
}





void TSL2591::sendCommand( uint8_t command, uint8_t value ) {

    Wire.beginTransmission( TSL2591_I2C_ADDR );
    Wire.write( TSL2591_COMMAND_SELECT | TSL2591_TRANSACTION_NORMAL | ( command & 0x1F ) );
    Wire.write( value );
    Wire.endTransmission();
}

uint8_t TSL2591::readByte( uint8_t address ) {

    Wire.beginTransmission( TSL2591_I2C_ADDR );
    Wire.write( TSL2591_COMMAND_SELECT | TSL2591_TRANSACTION_NORMAL | ( address & 0x1F ) );
    Wire.endTransmission();

    Wire.requestFrom( TSL2591_I2C_ADDR, 1 );

    return Wire.read();
}


uint16_t TSL2591::readWord( uint8_t address ) {
    Wire.beginTransmission( TSL2591_I2C_ADDR );
    Wire.write( TSL2591_COMMAND_SELECT | TSL2591_TRANSACTION_NORMAL | ( address & 0x1F ) );
    Wire.endTransmission();

    Wire.requestFrom( TSL2591_I2C_ADDR, 2 );

    uint8_t buffer[2];

    Wire.readBytes( buffer, sizeof( buffer ) );

    return buffer[1] << 8 | buffer[0];
}

