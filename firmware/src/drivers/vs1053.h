//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/vs1053.h
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
#ifndef VS1053_H
#define VS1053_H

#include <Arduino.h>
#include <SPI.h>



#define VS1053_SCI_READ         0x03
#define VS1053_SCI_WRITE        0x02

#define VS1053_REG_MODE         0x00
#define VS1053_REG_STATUS       0x01
#define VS1053_REG_BASS         0x02
#define VS1053_REG_CLOCKF       0x03
#define VS1053_REG_DECODETIME   0x04
#define VS1053_REG_AUDATA       0x05
#define VS1053_REG_WRAM         0x06
#define VS1053_REG_WRAMADDR     0x07
#define VS1053_REG_HDAT0        0x08
#define VS1053_REG_HDAT1        0x09
#define VS1053_REG_VOLUME       0x0B

#define VS1053_GPIO_DDR         0xC017
#define VS1053_GPIO_IDATA       0xC018
#define VS1053_GPIO_ODATA       0xC019

#define VS1053_INT_ENABLE       0xC01A

#define VS1053_MODE_SM_DIFF     0x0001
#define VS1053_MODE_SM_LAYER12  0x0002
#define VS1053_MODE_SM_RESET    0x0004
#define VS1053_MODE_SM_CANCEL   0x0008
#define VS1053_MODE_SM_EARSPKLO 0x0010
#define VS1053_MODE_SM_TESTS    0x0020
#define VS1053_MODE_SM_STREAM   0x0040
#define VS1053_MODE_SM_SDINEW   0x0800
#define VS1053_MODE_SM_ADPCM    0x1000
#define VS1053_MODE_SM_LINE1    0x4000
#define VS1053_MODE_SM_CLKRANGE 0x8000


#define VS1053_SCI_AIADDR       0x0A
#define VS1053_SCI_AICTRL0      0x0C
#define VS1053_SCI_AICTRL1      0x0D
#define VS1053_SCI_AICTRL2      0x0E
#define VS1053_SCI_AICTRL3      0x0F




#define VS1053_CONTROL_SPI_SETTING      SPISettings( 250000,  MSBFIRST, SPI_MODE0 )
#define VS1053_DATA_SPI_SETTING         SPISettings( 8000000, MSBFIRST, SPI_MODE0 )


/*******************************************************************************
 *
 * @brief   Audio codec (VS1053) driver class
 * 
 *******************************************************************************/
class VS1053 {

  public:
    VS1053( int8_t pin_cs, int8_t pin_xdcs, int8_t pin_dreq, int8_t pin_rst );
    int8_t begin();
    void end();
    bool readyForData();
    void playData( uint8_t *buffer, size_t buffsiz );
    void setVolume( uint8_t left, uint8_t right );
    void softReset();
    void reset();


  protected:
    uint16_t sciRead( uint8_t addr );
    void sciWrite( uint8_t addr, uint16_t data );
    inline void spiwrite( uint8_t c );
    void spiwrite( uint8_t *buffer, size_t num );


  private:
    bool _init = false;
    int8_t _pin_xdcs;
    int8_t _pin_cs;
    int8_t _pin_dreq;
    int8_t _pin_reset;
};

#endif /* VS1053_H */