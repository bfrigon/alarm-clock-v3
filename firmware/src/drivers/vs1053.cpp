//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/vs1053.cpp
// Author  : Benoit Frigon <www.bfrigon.com>
// Credits : Based on Adafruit_VS1053 library
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
#include "vs1053.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 * @param   pin_cs       Chip select pin
 * @param   pin_xdcs     Chip data select pin
 * @param   pin_dreq     Data request pin.
 * @param   pin_reset    Chip reset pin.
 * 
 */
VS1053::VS1053( int8_t pin_cs, int8_t pin_xdcs, int8_t pin_dreq, int8_t pin_reset ) {

    this->_pin_cs = pin_cs;
    this->_pin_xdcs = pin_xdcs;
    this->_pin_dreq = pin_dreq;
    this->_pin_reset = pin_reset;

}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the codec IC.
 *
 * @return  Returns the chip version if successful, -1 otherwise.
 * 
 */
int8_t VS1053::begin() {
    if( this->_init == true ) {
        return -1;
    }

    this->_init = true;

    pinMode( this->_pin_reset, OUTPUT );
    digitalWrite( this->_pin_reset, LOW );

    pinMode( this->_pin_cs, OUTPUT );
    digitalWrite( this->_pin_cs, HIGH );

    pinMode( this->_pin_xdcs, OUTPUT );
    digitalWrite( this->_pin_xdcs, HIGH );

    pinMode( this->_pin_dreq, INPUT );

    SPI.begin();

    #ifndef SPI_HAS_TRANSACTION
    SPI.setDataMode( SPI_MODE0 );
    SPI.setBitOrder( MSBFIRST );
    SPI.setClockDivider( SPI_CLOCK_DIV128 );
    #endif

    this->reset();

    return ( this->sciRead( VS1053_REG_STATUS ) >> 4 ) & 0x0F;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Shut down the codec IC.
 * 
 */
void VS1053::end() {
    if( this->_init == false ) {
        return;
    }

    digitalWrite( this->_pin_reset, LOW );

    this->_init = false;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check the state of the DREQ pin to see if the codec is ready 
 *          for more data.
 *
 * @return  TRUE if ready, FALSE otherwise.
 * 
 */
bool VS1053::readyForData() {

    if( this->_init == false ) {
        this->begin();
    }

    return digitalRead( this->_pin_dreq );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Send a data block to the codec buffer.
 *
 * @param   buffer     Pointer to the data block
 * @param   bufsize    Size of the data block
 * 
 */
void VS1053::playData( uint8_t *buffer, uint8_t buffsiz ) {
    if( this->_init == false ) {
        this->begin();
    }

    #ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction( VS1053_DATA_SPI_SETTING );
    #endif

    digitalWrite( this->_pin_xdcs, LOW );

    this->spiwrite( buffer, buffsiz );

    digitalWrite( this->_pin_xdcs, HIGH );

    #ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
    #endif
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the volume attenuation in 0.5 DB increment for each channels. 
 *          Value range from 0-255, 0 being full volume and 255 total silence.
 *
 * @param   left     Volume for the left channel.
 * @param   right    Volume value for the right channel.
 * 
 */
void VS1053::setVolume( uint8_t left, uint8_t right ) {
    if( this->_init == false ) {
        this->begin();
    }

    uint16_t volume;
    volume = left;
    volume <<= 8;
    volume |= right;

    noInterrupts();

    this->sciWrite( VS1053_REG_VOLUME, volume );

    interrupts();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Performs a software reset
 * 
 */
void VS1053::softReset() {
    if( this->_init == false ) {
        this->begin();
    }

    this->sciWrite( VS1053_REG_MODE, VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_RESET );
    delay( 3 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Performs a hardware reset
 * 
 */
void VS1053::reset() {
    if( this->_init == false ) {
        this->begin();
    }

    /* hardware reset */
    digitalWrite( this->_pin_reset, LOW );
    delay( 3 );
    digitalWrite( this->_pin_reset, HIGH );

    digitalWrite( this->_pin_cs, HIGH );
    digitalWrite( this->_pin_xdcs, HIGH );
    delay( 3 );

    this->softReset();

    this->sciWrite( VS1053_REG_CLOCKF, 0x6000 );
    delay( 50 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Read data from specified register.
 *
 * @param   addr    Address of the register to write to.
 *
 * @return  Value of the register
 * 
 */
uint16_t VS1053::sciRead( uint8_t addr ) {
    uint16_t data;

    #ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction( VS1053_CONTROL_SPI_SETTING );
    #endif

    digitalWrite( this->_pin_cs, LOW );
    spiwrite( VS1053_SCI_READ );
    spiwrite( addr );
    delayMicroseconds( 10 );

    data = SPI.transfer( 0x00 );
    data <<= 8;
    data |= SPI.transfer( 0x00 );

    digitalWrite( this->_pin_cs, HIGH );

    #ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
    #endif

    return data;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Write data the specified register.
 *
 * @param   addr    Address of the register to write to.
 * @param   data    Data to write to the register.
 * 
 */
void VS1053::sciWrite( uint8_t addr, uint16_t data ) {

    #ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction( VS1053_CONTROL_SPI_SETTING );
    #endif

    digitalWrite( this->_pin_cs, LOW );

    this->spiwrite( VS1053_SCI_WRITE );
    this->spiwrite( addr );
    this->spiwrite( data >> 8 );
    this->spiwrite( data & 0xFF );

    digitalWrite( this->_pin_cs, HIGH );

    #ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
    #endif
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sends a single byte to the codec over SPO.
 *
 * @param   c    Byte to send
 * 
 */
inline void VS1053::spiwrite( uint8_t c ) {
    SPI.transfer( c );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sends a data block to the codec over SPI.
 *
 * @param   buffer    Pointer to the data block.
 * @param   size      Size of the data block.
 * 
 */
void VS1053::spiwrite( uint8_t *buffer, uint16_t size ) {
    while( size-- ) {
        SPI.transfer( *buffer++ );
    }
}