//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/us2066.h
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

#ifndef US2066_H
#define US2066_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include "../resources.h"
#include "../libs/iprint.h"
#include "power.h"


#define CHAR_SPACE              0x20



/* LCD properties */
#ifndef US2066_DISPLAY_LINES
    #define US2066_DISPLAY_LINES    2
#endif




#define US2066_DEF_I2C_ADDR     0x3c



#define US2066_MODE_CONTINUE    0x80
#define US2066_MODE_CMD         0x00
#define US2066_MODE_DATA        0x40

/* Instruction sets */
#define US2066_ISET_STANDARD    0x00
#define US2066_ISET_SPECIAL     0x01
#define US2066_ISET_EXTENDED    0x02
#define US2066_ISET_OLED        0x03

/* Fundamental instruction set (RE=0, IS=0, SD=0) */
#define US2066_CMD_CLEAR        0x01
#define US2066_CMD_HOME         0x02
#define US2066_CMD_ENTRY        0x04
#define US2066_CMD_DISPLAY      0x08
#define US2066_CMD_CURSOR       0x10
#define US2066_CMD_FUNCTSET     0x20
#define US2066_CMD_CGRAM        0x40
#define US2066_CMD_DDRAM        0x80

/* Extended instruction set (RE=1, IS=0, SD=0) */
#define US2066_CMD_FUNC_A       0x71
#define US2066_CMD_FUNC_B       0x72
#define US2066_CMD_OLED_CHAR    0x78
#define US2066_CMD_FUNCTSET_EXT 0x08
#define US2066_CMD_MAP          0x04

/* OLED instruction set (RE=1, IS=0, SD=1) */
#define US2066_CMD_CONTRAST     0x81
#define US2066_CMD_CLOCK        0xD5
#define US2066_CMD_SEG_HW       0xDA
#define US2066_CMD_PHASE        0xD9
#define US2066_CMD_VCOMH        0xDB




#define US2066_INTERNAL_VDD_OFF     0x00
#define US2066_INTERNAL_VDD_ON      0x5C


#define US2066_OP_RE        0x02
#define US2066_OP_IS        0x01
#define US2066_OP_SD        0x01

#define US2066_BDC_INC      0x02    /* COM0 -> COM31 */
#define US2066_BDC_DEC      0x00    /* COM31 -> COM0 */

#define US2066_BDS_INC      0x01    /* SEG0 -> SEG99 */
#define US2066_BDS_DEC      0x00    /* SEG99 -> SEG0 */


#define US2066_ROM_A        0x00
#define US2066_ROM_B        0x04
#define US2066_ROM_C        0x08

#define US2066_CG_240       0x00
#define US2066_CG_248       0x01
#define US2066_CG_250       0x02
#define US2066_CG_256       0x03

#define US2066_SEG_SEQ      0x00
#define US2066_SEG_ALT      0x10
#define US2066_LR_ENABLED   0x20
#define US2066_LR_DISABLED  0x00


#define US2066_DISPLAY_OFF  0x00
#define US2066_DISPLAY_ON   0x04

#define US2066_CURSOR_OFF   0x00
#define US2066_CURSOR_ON    0x02

#define US2066_BLINK_OFF    0x00
#define US2066_BLINK_ON     0x01

#define US2066_FUNC_BE      0x04
#define US2066_FUNC_REV     0x01



#if US2066_DISPLAY_LINE == 4
    #define US2066_DEF_FSET       0x08
    #define US2066_DEF_FSET_EXT   0x01

#elif US2066_DISPLAY_LINE == 3
    #define US2066_DEF_FSET       0x00
    #define US2066_DEF_FSET_EXT   0x01

#elif US2066_DISPLAY_LINE == 1
    #define US2066_DEF_FSET       0x00
    #define US2066_DEF_FSET_EXT   0x00

#else
    #define US2066_DEF_FSET       0x08
    #define US2066_DEF_FSET_EXT   0x00
#endif






typedef struct {
    bool blink = false;
    bool cursor = false;
    bool display = false;

    bool reverseDisplay = false;
    bool cgramBlink = true;

} US2066_STATE;






class US2066 : public IPrint {

  public:

    US2066( uint8_t address, uint8_t pin_reset );

    void begin();
    void end();

    void clear();
    void setPosition( uint8_t row, uint8_t col );
    void setContrast( uint8_t contrast );
    void setCursor( bool underline, bool blinking );
    void setDisplay( bool on, bool reverse );
    uint8_t setCustomCharacters( const char *pchrmap );
    void fill( char c, uint8_t num );

  private:
    void selectInstructions( uint8_t iset );
    void updateDisplayState();
    uint8_t sendCommand( uint8_t cmd );
    uint8_t sendCommand( uint8_t cmd, uint8_t data );
    uint8_t _print( char c );

    bool _init = false;
    uint8_t _address = US2066_DEF_I2C_ADDR;
    uint8_t _pin_reset;

    uint8_t _current_iset = US2066_ISET_STANDARD;
    FILE _lcdout = {0};
    US2066_STATE _state;
};

extern US2066 g_lcd ;

#endif /* US2066_H */