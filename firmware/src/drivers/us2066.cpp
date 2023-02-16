//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/us2066.cpp
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

#include "us2066.h"



/*******************************************************************************
 *
 * @brief   Class constructor
 *
 * @param   address      I2C address of the OLED module
 * @param   pin_reset    Pin connected to the OLED reset line
 * 
 */
US2066::US2066( uint8_t address, uint8_t pin_reset ) {
    _address = address;
    _pin_reset = pin_reset;
    _ambientDimming = 0;

    /* Initialize IPrint interface */
    _initPrint();
}


/*******************************************************************************
 *
 * @brief   Perform proper power-up sequence and initialize the LCD module.
 * 
 */
void US2066::begin() {
    if( _init == true ) {
        return;
    }

    _init = true;

    delay( 1 );
    pinMode( _pin_reset, OUTPUT );

    /* Assert the reset pin the LCD module */
    digitalWrite( _pin_reset, LOW );
    delay( 1 );
    digitalWrite( _pin_reset, HIGH );
    delay( 1 );

    /* Disable internal regulator */
    this->selectInstructions( US2066_ISET_EXTENDED );   /* RE=1, SD=0 */
    this->sendCommand( US2066_CMD_FUNC_A, US2066_INTERNAL_VDD_OFF );

    /* Set display OFF, no cursor */
    this->selectInstructions( US2066_ISET_STANDARD );    /* RE=0 */
    this->sendCommand( US2066_CMD_DISPLAY | US2066_DISPLAY_OFF | US2066_CURSOR_OFF );

    /* Set display clock devide ratio, oscillator freq */
    this->selectInstructions( US2066_ISET_OLED );       /* RE=1, SD=1 */
    this->sendCommand( US2066_CMD_CLOCK, 0x50 );
    this->selectInstructions( US2066_ISET_EXTENDED );   /* RE=1, SD=0 */

    /* Set display mode */
    this->sendCommand( US2066_CMD_FUNCTSET_EXT | US2066_DEF_FSET_EXT );

    /* Set display common and segment direction */
    this->sendCommand( US2066_CMD_MAP | US2066_BDC_INC | US2066_BDS_DEC );

    /* Set ROM selection */
    this->sendCommand( US2066_CMD_FUNC_B, US2066_ROM_A | US2066_CG_248 );

    /* Set SEG pins HW config */
    this->selectInstructions( US2066_ISET_OLED );       /* RE=1, SD=1 */
    this->sendCommand( US2066_CMD_SEG_HW, US2066_SEG_ALT | US2066_LR_DISABLED );

    /* Set contrast */
    this->sendCommand( US2066_CMD_CONTRAST, 128 );

    /* Set phase length */
    this->sendCommand( US2066_CMD_PHASE, 0x2F );

    //Set VCOMH Deselect level
    this->sendCommand( US2066_CMD_VCOMH, 0x30 );

    /* Return to standard instruction set */
    this->selectInstructions( US2066_ISET_STANDARD );   /* RE=0 */

    /* Clear display */
    this->clear();

    /* Set cursor home */
    this->setPosition( 0, 0 );

    /* Turn display ON */
    this->setDisplay( true, false );

    /* Give time for SEG/COM to turn on */
    delay( 200 );
    
}


/*******************************************************************************
 *
 * @brief   Shutdown the LCD module.
 * 
 */
void US2066::end() {
    if( _init == false ) {
        return;
    }

    _init = false;

    pinMode( _pin_reset, OUTPUT );
    digitalWrite( _pin_reset, LOW );


}


/*******************************************************************************
 *
 * @brief   Selects the display instruction set.
 *
 * @param   iset    Instruction set (US2066_ISET_STANDARD, US2066_ISET_SPECIAL,
 *                  US2066_ISET_EXTENDED or US2066_ISET_OLED).
 * 
 */
void US2066::selectInstructions( uint8_t iset ) {
    uint8_t fset;

    switch( iset ) {


        /* RE=1, IS=X, SD=0 */
        case US2066_ISET_EXTENDED:

            /* If current set is oled, go back to extended set */
            if( _current_iset == US2066_ISET_OLED ) {
                this->sendCommand( US2066_CMD_OLED_CHAR );
            }

        /* Fall through */

        /* RE=1, IS=X, SD=1 */
        case US2066_ISET_OLED:

            fset = US2066_DEF_FSET | US2066_FUNC_BE;

            if( _state.cgramBlink == true ) {
                fset |= US2066_FUNC_BE;
            }

            if( _state.reverseDisplay == true ) {
                fset |= US2066_FUNC_REV;
            }

            this->sendCommand( US2066_CMD_FUNCTSET | US2066_OP_RE | fset );


            if( iset == US2066_ISET_OLED ) {
                this->sendCommand( US2066_CMD_OLED_CHAR | US2066_OP_SD );
            }

            break;


        /* US2066_ISET_STANDARD : RE=0, SD=0, IS=0 */
        /* US2066_ISET_SPECIAL : RE=0, SD=0, IS=1 */
        default:

            /* If current set is oled, go back to extended set first. */
            if( _current_iset == US2066_ISET_OLED ) {
                this->sendCommand( US2066_CMD_OLED_CHAR );
            }

            fset = US2066_DEF_FSET | ( ( iset == US2066_ISET_SPECIAL ) ? US2066_OP_IS : 0x00 );
            this->sendCommand( US2066_CMD_FUNCTSET | fset );
            break;
    }

    _current_iset = iset;
}


/*******************************************************************************
 *
 * @brief   Sends a single byte command to the LCD module.
 *
 * @param   cmd     Command to send
 *
 * @return  Status of the transmission
 * 
 * @retval  0   Success
 * @retval  1   Data too long to fit in transmit buffer
 * @retval  2   Received NACK on transmit of address
 * @retval  3   Received NACK on transmit of data
 * @retval  4   Other error
 * 
 */
uint8_t US2066::sendCommand( uint8_t cmd ) {


    Wire.beginTransmission( _address );

    Wire.write( US2066_MODE_CMD );
    Wire.write( cmd );

    return Wire.endTransmission();
}


/*******************************************************************************
 *
 * @brief   Send a two bytes command to the LCD module (cmd + data).
 *
 * @param   cmd     Command to send
 * @param   data    Data (single byte)
 *
 * @return  Status of the transmission
 * 
 * @retval  0   Success
 * @retval  1   Data too long to fit in transmit buffer
 * @retval  2   Received NACK on transmit of address
 * @retval  3   Received NACK on transmit of data
 * @retval  4   Other error
 * 
 */
uint8_t US2066::sendCommand( uint8_t cmd, uint8_t data ) {
    Wire.beginTransmission( _address );

    Wire.write( US2066_MODE_CMD | US2066_MODE_CONTINUE );
    Wire.write( cmd );

    /* Data following commands in extended mode require D/C#=1. */
    Wire.write( _current_iset == US2066_ISET_EXTENDED ? US2066_MODE_DATA : US2066_MODE_CMD );
    Wire.write( data );

    return Wire.endTransmission();
}


/*******************************************************************************
 *
 * @brief   Write custom characters to the LCD CGRAM.
 *
 * @param   pchrmap    Pointer to the array containing the 
 *                     characters (8x8 bytes).
 *
 * @return  Status of the transmission
 * 
 * @retval  0   Success
 * @retval  1   Data too long to fit in transmit buffer
 * @retval  2   Received NACK on transmit of address
 * @retval  3   Received NACK on transmit of data
 * @retval  4   Other error
 * 
 */
uint8_t US2066::setCustomCharacters( const unsigned char *pchrmap ) {

    if( _init == false ) {
        this->begin();
    }

    this->selectInstructions( US2066_ISET_STANDARD ); /* RE=0, SD=0, SI=0 */

    uint8_t ch;

    for( ch = 0; ch < 8; ch++ ) {

        Wire.beginTransmission( _address );
        Wire.write( US2066_MODE_CMD | US2066_MODE_CONTINUE );
        Wire.write( US2066_CMD_CGRAM | ( ch << 3 ) );

        uint8_t i;

        for( i = 0; i < 8; i++ ) {

            Wire.write( US2066_MODE_DATA | US2066_MODE_CONTINUE );
            Wire.write( pgm_read_byte( pchrmap++ ) );
        }

        uint8_t res;
        res = Wire.endTransmission();

        if( res != 0 ) {
            return res;
        }
    }

    return 0;
}


/*******************************************************************************
 *
 * @brief   Set the DDRAM address corresponding to the specified 
 *          row and column.
 *
 * @param   row     Row number (0 based)
 * @param   col     Column number (0 based)
 */
void US2066::setPosition( uint8_t row, uint8_t col ) {
    if( _init == false ) {
        this->begin();
    }

    uint8_t dram_address = ( col + ( row * 0x40 ) );

    this->sendCommand( US2066_CMD_DDRAM | ( dram_address & 0x7F ) );
}


/*******************************************************************************
 *
 * @brief   Clear the display
 * 
 */
void US2066::clear() {
    if( _init == false ) {
        this->begin();
    }

    this->sendCommand( US2066_CMD_CLEAR );
}


/*******************************************************************************
 *
 * @brief   Set the display contrast
 * 
 * @param   contrast    Contrast value (0-100)
 */
void US2066::setContrast( uint8_t contrast ) {
    if( _init == false ) {
        this->begin();
    }

    if( contrast > 100 ) {
        contrast = 100;
    }

    _contrast = contrast;

    /* Apply ambiant light dimming percentage. */
    contrast = contrast * ( 100 - _ambientDimming ) / 100;

    /* Select OLED instruction set */
    this->selectInstructions( US2066_ISET_OLED );   /* RE=1, SD=1 */

    this->sendCommand( US2066_CMD_CONTRAST, contrast );

    /* Return to standard instruction set */
    this->selectInstructions( US2066_ISET_STANDARD ); /* RE=0, SD=0, SI=0 */
}


/*******************************************************************************
 *
 * @brief   Sets the ambient dimming percentage.
 *
 * @param   dimming    Dimming percentage ( 0-100 )
 * 
 */
void US2066::setAmbientDimming( uint8_t dimming ) {

    if( dimming > 100 ) {
        dimming = 100;
    }

    if( dimming != _ambientDimming ) {
        _ambientDimming = dimming;

        this->setContrast( _contrast );
    }
}


/*******************************************************************************
 *
 * @brief   Sets the cursor state of the LCD module.
 *
 * @param   underline    Underline cursor ON or OFF.
 * @param   blinking     Blinking cursor (block).
 */
void US2066::setCursor( bool underline, bool blinking ) {
    if( _init == false ) {
        this->begin();
    }

    _state.cursor = underline;
    _state.blink = blinking;

    this->updateDisplayState();
}


/*******************************************************************************
 *
 * @brief   Turn the display ON or OFF.
 *
 * @param   on          Turn display On or OFF.
 * @param   reverse     Reverse all pixels on the display.
 */
void US2066::setDisplay( bool on, bool reverse ) {
    if( _init == false ) {
        this->begin();
    }

    _state.display = on;
    _state.reverseDisplay = reverse;

    this->updateDisplayState();

}


/*******************************************************************************
 *
 * @brief   Update the display control register on the LCD. 
 *          (display ON/OFF, cursor)
 * 
 */
void US2066::updateDisplayState() {
    uint8_t state = _state.blink |
                    _state.cursor << 1 |
                    _state.display << 2;

    this->sendCommand( US2066_CMD_DISPLAY | state );


    /* Select extended instruction set, reverse display is set while
       selecting instruction set. */
    this->selectInstructions( US2066_ISET_EXTENDED ); /* RE=1, SD=0 */


    /* Return to standard instruction set. */
    this->selectInstructions( US2066_ISET_STANDARD ); /* RE=0, SD=0, SI=0 */

}


/*******************************************************************************
 *
 * @brief   Fills the LCD with the specified number of characters.
 *
 * @param   c       Character to print
 * @param   num     Number of times to print the character
 *
 * @return  Status of the transmission
 * 
 * @retval  0   Success
 * @retval  1   Data too long to fit in transmit buffer
 * @retval  2   Received NACK on transmit of address
 * @retval  3   Received NACK on transmit of data
 * @retval  4   Other error
 * 
 */
void US2066::fill( char c, uint8_t num ) {
    if( _init == false ) {
        this->begin();
    }

    if( num == 0 ) {
        return;
    }

    while( num-- ) {
        this->print( c );
    }
}


/*******************************************************************************
 *
 * @brief   IPrint interface callback for printing a single character. 
 *          Sends the output to the LCD module at the current coordinates.
 *
 * @param   c    Character to print
 *
 * @return  Number of bytes written
 * 
 */
size_t US2066::_print( char c ) {
    if( _init == false ) {
        this->begin();
    }

    Wire.beginTransmission( _address );

    Wire.write( US2066_MODE_DATA );
    Wire.write( c );

    return ( Wire.endTransmission() == 0 ? 1 : 0 );
}


/*******************************************************************************
 *
 * @brief   Convert a buffer containing UTF-8 formated characters to the
 *          equivalent characters supported by the LCD module (US2066, Rom A).
 *
 * @param   buffer  Pointer to the UTF-8 string to convert.
 * @param   length  Number of character contained in the buffer.    
 * 
 */
void utf8ToLcdCharset( char* buffer, size_t length ) {

    uint32_t unicode = 0;

    while( length-- ) {

        uint8_t c = *(( uint8_t* )buffer );
        uint8_t replace = 0;

        /* End of string (NULL character) */
        if( c == 0 ) {
            break;

        /* Single byte code point (U+0000 - U+007F) */
        } else if ( c < 0x80 ) {
            unicode = c;

        /* Multi-byte code point */
        } else {
            unicode <<= 8;
            unicode += c;
        }
            
        /* Convert unicode characters to LCD characters */
        switch( unicode ) {
            case 0xC2B0:    replace = 0x07; break;      /* U+00B0 (Degree symbol) */
            case 0xC2AB:    replace = 0x14; break;      /* U+00AB (Left-pointing double angle quotation mark) */
            case 0xC2BB:    replace = 0x15; break;      /* U+00BB (Right-pointing double angle quotation mark) */
            case 0x5E:      replace = 0x1D; break;      /* U+005E (Circumflex accent) */
            case 0xC2A4:    replace = 0x24; break;      /* U+00A4 (Currency sign) */
            case 0xC2A1:    replace = 0x40; break;      /* U+00A1 (Inverted exclamation mark) */
            case 0xC384:    replace = 0x5B; break;      /* U+00C4 (Latin capital letter A with diaeresis) */
            case 0xC396:    replace = 0x5C; break;      /* U+00D6 (Latin capital letter O with diaeresis) */
            case 0xC391:    replace = 0x5D; break;      /* U+00D1 (Latin capital letter N with tilde) */
            case 0xC39C:    replace = 0x5E; break;      /* U+00DC (Latin capital letter U with diaeresis) */
            case 0xC2A7:    replace = 0x5F; break;      /* U+00A7 (Section sign)*/
            case 0xC2BF:    replace = 0x60; break;      /* U+00BF (Inverted question mark) */
            case 0xC3A4:    replace = 0x7B; break;      /* U+00E4 (Latin small letter A with diaeresis) */
            case 0xC3B6:    replace = 0x7C; break;      /* U+00F6 (Latin small letter O with diaeresis) */
            case 0xC3B1:    replace = 0x7D; break;      /* U+00F1 (Latin small letter N with tilde) */
            case 0xC3BC:    replace = 0x7E; break;      /* U+00FC (Latin small letter U with diaeresis) */
            case 0xC3A0:    replace = 0x7F; break;      /* U+00E0 (Latin small letter A with grave) */
            case 0xC2B9:    replace = 0x81; break;      /* U+00B9 (Superscript 1) */
            case 0xC2B2:    replace = 0x82; break;      /* U+00B2 (Superscript 2) */
            case 0xC2B3:    replace = 0x83; break;      /* U+00B3 (Superscript 3) */
            case 0xC2BD:    replace = 0x8A; break;      /* U+00BD (Fraction one half) */
            case 0xC2BC:    replace = 0x8B; break;      /* U+00BC (Fraction one quarter) */
            case 0xC2B1:    replace = 0x8C; break;      /* U+00B1 (Plus-minus sign) */
            case 0xE289A5:  replace = 0x8D; break;      /* U+2265 (Greater-that or equal to) */
            case 0xE289A4:  replace = 0x8E; break;      /* U+2264 (Less-that or equal to) */
            case 0xC2B5:    replace = 0x8F; break;      /* U+00B5 (Micro sign) */
            case 0xE299AA:  replace = 0x90; break;      /* U+266A (Eighth note) */
            case 0xE299AB:  replace = 0x91; break;      /* U+266B (Beamed eighth notes) */
            case 0xE299A5:  replace = 0x93; break;      /* U+2665 (Black hearth) */
            case 0xE299A6:  replace = 0x94; break;      /* U+2666 (Black diamond) */
            case 0xE2809C:  replace = 0x98; break;      /* U+201C (Left double quotation mark) */
            case 0xE2809D:  replace = 0x99; break;      /* U+201D (Right double quotation mark) */
            case 0xCEB1:    replace = 0x9C; break;      /* U+03B1 (Greek small letter alpha) */
            case 0xCEB5:    replace = 0x9D; break;      /* U+03B5 (Greek small letter epsilon) */
            case 0x40:      replace = 0xA0; break;      /* U+0040 (Commercial AT) */
            case 0xC2A3:    replace = 0xA1; break;      /* U+00A3 (Pound sign) */
            case 0x24:      replace = 0xA2; break;      /* U+0024 (Dollar sign) */
            case 0xC2A5:    replace = 0xA3; break;      /* U+00A5 (Yen sign) */
            case 0xC3A8:    replace = 0xA4; break;      /* U+00E8 (Latin small letter E with grave) */
            case 0xC3A9:    replace = 0xA5; break;      /* U+00E9 (Latin small letter E with acute) */
            case 0xC3B9:    replace = 0xA6; break;      /* U+00F9 (Latin small letter U with grave) */
            case 0xC3AC:    replace = 0xA7; break;      /* U+00EC (Latin small letter I with grave) */
            case 0xC3B2:    replace = 0xA8; break;      /* U+00F2 (Latin small letter O with grave) */
            case 0xC387:    replace = 0xA9; break;      /* U+00C7 (Latin capital letter C with cedilla) */
            case 0xC398:    replace = 0xAB; break;      /* U+00D8 (Latin capital letter O with stroke) */
            case 0xC3B8:    replace = 0xAC; break;      /* U+00F8 (Latin small letter O with stroke) */
            case 0xC385:    replace = 0xAE; break;      /* U+00C5 (Latin capital letter A with ring above) */
            case 0xC3A5:    replace = 0xAF; break;      /* U+00E5 (Latin capital letter C with cedilla) */
            case 0xC2A2:    replace = 0xB1; break;      /* U+00A2 (Cent sign) */
            case 0xCEBB:    replace = 0xB4; break;      /* U+03BB (Greek small letter lambda) */
            case 0xCEA9:    replace = 0xB5; break;      /* U+03A9 (Greek capital letter omega) */
            case 0xCF80:    replace = 0xB6; break;      /* U+03C0 (Greek small letter pi) */
            case 0xCEA8:    replace = 0xB7; break;      /* U+03A8 (Greek capital letter psi) */
            case 0xCEA3:    replace = 0xB8; break;      /* U+03A3 (Greek capital letter sigma) */
            case 0xCEB8:    replace = 0xB9; break;      /* U+03B8 (Greek small letter theta) */
            case 0xCE9E:    replace = 0xBA; break;      /* U+039E (Greek capital letter xi) */
            case 0xE2978F:  replace = 0xBB; break;      /* U+25CF (Black circle) */
            case 0xC386:    replace = 0xBC; break;      /* U+00C6 (Latin capital letter AE) */
            case 0xC3A6:    replace = 0xBD; break;      /* U+00E6 (Latin small letter AE) */
            case 0xC39F:    replace = 0xBE; break;      /* U+00DF (Latin small letter sharp S) */
            case 0xC389:    replace = 0xBF; break;      /* U+00C9 (Latin capital letter E with acute) */
            case 0xCE93:    replace = 0xC0; break;      /* U+0393 (Greek capital letter gamma) */
            case 0xCE9B:    replace = 0xC1; break;      /* U+039B (Greek capital letter lambda) */
            case 0xCEA0:    replace = 0xC2; break;      /* U+03A0 (Greek capital letter pi) */
            case 0x5F:      replace = 0xC4; break;      /* U+005F (Low line) */
            case 0xC388:    replace = 0xC5; break;      /* U+00C8 (Latin capital letter E with grave) */
            case 0xC38A:    replace = 0xC6; break;      /* U+00CA (Latin capital letter E with circumflex) */
            case 0xC3AA:    replace = 0xC7; break;      /* U+00EA (Latin small letter E with circumflex) */
            case 0xC3A7:    replace = 0xC8; break;      /* U+00E7 (Latin small letter C with cedilla) */
            case 0x7E:      replace = 0xCE; break;      /* U+007E (Tilde) */
            case 0xC2B7:    replace = 0xDD; break;      /* U+00B7 (Middle dot) */
            case 0xC381:    replace = 0xE2; break;      /* U+00C1 (Latin capital letter A with acute) */
            case 0xC38D:    replace = 0xE3; break;      /* U+00CD (Latin capital letter I with acute) */
            case 0xC393:    replace = 0xE4; break;      /* U+00D3 (Latin capital letter O with acute) */
            case 0xC39A:    replace = 0xE5; break;      /* U+00DA (Latin capital letter U with acute) */
            case 0xC39D:    replace = 0xE6; break;      /* U+00DD (Latin capital letter Y with acute) */
            case 0xC3A1:    replace = 0xE7; break;      /* U+00E1 (Latin small letter A with acute) */
            case 0xC3AD:    replace = 0xE8; break;      /* U+00ED (Latin small letter I with acute) */
            case 0xC3B3:    replace = 0xE9; break;      /* U+00F3 (Latin small letter O with acute) */
            case 0xC3BA:    replace = 0xEA; break;      /* U+00FA (Latin small letter U with acute) */
            case 0xC3BD:    replace = 0xEB; break;      /* U+00FD (Latin small letter Y with acute) */
            case 0xC394:    replace = 0xEC; break;      /* U+00D4 (Latin capital letter O with circumflex) */
            case 0xC3B4:    replace = 0xED; break;      /* U+00F4 (Latin small letter O with circumflex) */
            case 0xC48C:    replace = 0xF0; break;      /* U+010C (Latin capital letter C with caron) */
            case 0xC49A:    replace = 0xF1; break;      /* U+011A (Latin capital letter E with caron) */
            case 0xC598:    replace = 0xF2; break;      /* U+0158 (Latin capital letter R with caron) */
            case 0xC5A0:    replace = 0xF3; break;      /* U+0160 (Latin capital letter S with caron) */
            case 0xC5BD:    replace = 0xF4; break;      /* U+017D (Latin capital letter Z with caron) */
            case 0xC48D:    replace = 0xF5; break;      /* U+010D (Latin small letter C with caron) */
            case 0xC49B:    replace = 0xF6; break;      /* U+011B (Latin small letter E with caron) */
            case 0xC599:    replace = 0xF7; break;      /* U+0159 (Latin small letter R with caron) */
            case 0xC5A1:    replace = 0xF8; break;      /* U+0161 (Latin small letter S with caron) */
            case 0xC5BE:    replace = 0xF9; break;      /* U+017E (Latin small letter Z with caron) */
            case 0x5B:      replace = 0xFA; break;      /* U+005B (Left square bracket) */
            case 0x5C:      replace = 0xFB; break;      /* U+005C (Reverse solidus) */
            case 0x5D:      replace = 0xFC; break;      /* U+005D (Right square bracket) */
            case 0x7B:      replace = 0xFD; break;      /* U+007B (Left curly bracket) */
            case 0x7C:      replace = 0xFE; break;      /* U+007C (Vertical line) */
            case 0xC2A6:    replace = 0xFE; break;      /* U+00A6 (Broken bar) */
            case 0x7D:      replace = 0xFF; break;      /* U+007D (Right curly bracket) */
        }

        /* Remove byte if unknown unicode character found at current position */
        if(( replace == 0 ) && ( unicode >= 0x80 )) {

            memmove( buffer, buffer + 1, length );
            memset( buffer + length, 0, 1 );

            continue;
        }

        /* If unicode replacement found, replace the current character */       
        if( replace > 0 ) {
            *(( uint8_t* )buffer ) = replace;
        }

        buffer++;
        unicode = 0;
    }
}