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


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - address   : I2C address of the OLED module
 *  - pin_reset : Pin connected to the OLED reset line
 */
US2066::US2066( uint8_t address, uint8_t pin_reset ) {
    this->_address = address;
    this->_pin_reset = pin_reset;
}


/*--------------------------------------------------------------------------
 *
 * Perform proper power-up sequence and initialize the LCD module.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void US2066::begin() {
    if( this->_init == true ) {
        return;
    }

    this->_init = true;

    delay( 1 );
    pinMode( this->_pin_reset, OUTPUT );

    /* Assert the reset pin the LCD module */
    digitalWrite( this->_pin_reset, LOW );
    delay( 1 );
    digitalWrite( this->_pin_reset, HIGH );
    delay( 1 );


    /* Setup stream for printf function */
    fdev_setup_stream( &this->_lcdout, this->_putchar, NULL, _FDEV_SETUP_WRITE );
    fdev_set_udata( &this->_lcdout, ( void * )this );


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


/*--------------------------------------------------------------------------
 *
 * Shutdown LCD module.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void US2066::end() {
    if( this->_init == false ) {
        return;
    }

    this->_init = false;

    pinMode( this->_pin_reset, OUTPUT );
    digitalWrite( this->_pin_reset, LOW );


}


/*--------------------------------------------------------------------------
 *
 * Selects instruction set.
 *
 * Arguments
 * ---------
 *  - iset : Instruction set (US2066_ISET_STANDARD, US2066_ISET_SPECIAL,
 *           US2066_ISET_EXTENDED or US2066_ISET_OLED).
 *
 * Returns : Nothing
 */
void US2066::selectInstructions( uint8_t iset ) {
    uint8_t fset;

    switch( iset ) {


        /* RE=1, IS=X, SD=0 */
        case US2066_ISET_EXTENDED:

            /* If current set is oled, go back to extended set */
            if( this->_current_iset == US2066_ISET_OLED ) {
                this->sendCommand( US2066_CMD_OLED_CHAR );
            }

        /* Fall through */

        /* RE=1, IS=X, SD=1 */
        case US2066_ISET_OLED:

            fset = US2066_DEF_FSET | US2066_FUNC_BE;

            if( this->_state.cgramBlink == true ) {
                fset |= US2066_FUNC_BE;
            }

            if( this->_state.reverseDisplay == true ) {
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

            /* If current set is oled, go back to extended set first */
            if( this->_current_iset == US2066_ISET_OLED ) {
                this->sendCommand( US2066_CMD_OLED_CHAR );
            }

            fset = US2066_DEF_FSET | ( ( iset == US2066_ISET_SPECIAL ) ? US2066_OP_IS : 0x00 );
            this->sendCommand( US2066_CMD_FUNCTSET | fset );
            break;
    }

    this->_current_iset = iset;
}


/*--------------------------------------------------------------------------
 *
 * Sends a single byte command to the LCD module.
 *
 * Arguments
 * ---------
 *  - cmd : Command to send
 *
 * Returns : Status of the transmission
 *   0: Success
 *   1: Data too long to fit in transmit buffer
 *   2: Received NACK on transmit of address
 *   3: Received NACK on transmit of data
 *   4: Other error
 */
uint8_t US2066::sendCommand( uint8_t cmd ) {


    Wire.beginTransmission( this->_address );

    Wire.write( US2066_MODE_CMD );
    Wire.write( cmd );

    return Wire.endTransmission();
}


/*--------------------------------------------------------------------------
 *
 * Send a two bytes command to the LCD module (cmd + data)
 *
 * Arguments
 * ---------
 *  - cmd  : Command to send
 *  - data : Data (single byte)
 *
 * Returns : Status of the transmission
 *   0: Success
 *   1: Data too long to fit in transmit buffer
 *   2: Received NACK on transmit of address
 *   3: Received NACK on transmit of data
 *   4: Other error
 */
uint8_t US2066::sendCommand( uint8_t cmd, uint8_t data ) {
    Wire.beginTransmission( this->_address );

    Wire.write( US2066_MODE_CMD | US2066_MODE_CONTINUE );
    Wire.write( cmd );

    /* Data following commands in extended mode require D/C#=1 */
    Wire.write( this->_current_iset == US2066_ISET_EXTENDED ? US2066_MODE_DATA : US2066_MODE_CMD );
    Wire.write( data );

    return Wire.endTransmission();
}


/*--------------------------------------------------------------------------
 *
 * Write custom characters to the LCD CGRAM.
 *
 * Arguments
 * ---------
 *  - pchrmap  : Pointer to the array containing the characters (8x8 bytes).
 *
 * Returns : Status of the transmission
 *   0: Success
 *   1: Data too long to fit in transmit buffer
 *   2: Received NACK on transmit of address
 *   3: Received NACK on transmit of data
 *   4: Other error
 */
uint8_t US2066::setCustomCharacters( const char *pchrmap ) {

    if( this->_init == false ) {
        this->begin();
    }

    this->selectInstructions( US2066_ISET_STANDARD ); /* RE=0, SD=0, SI=0 */

    uint8_t ch;

    for( ch = 0; ch < 8; ch++ ) {

        Wire.beginTransmission( this->_address );
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


/*--------------------------------------------------------------------------
 *
 * Set the DDRAM address corresponding to the specified row and column.
 *
 * Arguments
 * ---------
 *  - row : Row number (0 based)
 *  - col : Column number (0 based)
 *
 * Returns : Nothing
 */
void US2066::setPosition( uint8_t row, uint8_t col ) {
    if( this->_init == false ) {
        this->begin();
    }

    uint8_t dram_address = ( col + ( row * 0x40 ) );

    this->sendCommand( US2066_CMD_DDRAM | ( dram_address & 0x7F ) );
}


/*--------------------------------------------------------------------------
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void US2066::clear() {
    if( this->_init == false ) {
        this->begin();
    }

    this->sendCommand( US2066_CMD_CLEAR );
}


/*--------------------------------------------------------------------------
 *
 * Arguments
 * ---------
 *  - contrast : Contrast value (0-255)
 *
 * Returns : Nothing
 */
void US2066::setContrast( uint8_t contrast ) {
    if( this->_init == false ) {
        this->begin();
    }

    contrast = pgm_read_byte( &_GAMMA_TABLE[ 255 * contrast / 100 ] ),


    /* Select OLED instruction set */
    this->selectInstructions( US2066_ISET_OLED );   /* RE=1, SD=1 */

    this->sendCommand( US2066_CMD_CONTRAST, contrast );

    /* Return to standard instruction set */
    this->selectInstructions( US2066_ISET_STANDARD ); /* RE=0, SD=0, SI=0 */
}


/*--------------------------------------------------------------------------
 *
 * Sets the cursor state of the LCD module.
 *
 * Arguments
 * ---------
 *  - underline : Underline cursor ON or OFF.
 *  - blinking  : Blinking cursor (block).
 *
 * Returns : Nothing
 */
void US2066::setCursor( bool underline, bool blinking ) {
    if( this->_init == false ) {
        this->begin();
    }

    this->_state.cursor = underline;
    this->_state.blink = blinking;

    this->updateDisplayState();
}


/*--------------------------------------------------------------------------
 *
 * Turn the display ON or OFF.
 *
 * Arguments
 * ---------
 *  - on      : Turn display On or OFF.
 *  - reverse : Reverse all pixels on the display.
 *
 * Returns : Nothing
 */
void US2066::setDisplay( bool on, bool reverse ) {
    if( this->_init == false ) {
        this->begin();
    }

    this->_state.display = on;
    this->_state.reverseDisplay = reverse;

    this->updateDisplayState();

}


/*--------------------------------------------------------------------------
 *
 * Update the display control register on the LCD. (display ON/OFF, cursor)
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void US2066::updateDisplayState() {
    uint8_t state = this->_state.blink |
                    this->_state.cursor << 1 |
                    this->_state.display << 2;

    this->sendCommand( US2066_CMD_DISPLAY | state );


    /* Select extended instruction set, reverse display is set while
       selecting instruction set */
    this->selectInstructions( US2066_ISET_EXTENDED ); /* RE=1, SD=0 */


    /* Return to standard instruction set */
    this->selectInstructions( US2066_ISET_STANDARD ); /* RE=0, SD=0, SI=0 */

}


/*--------------------------------------------------------------------------
 *
 * Fills the LCD with the specified number of characters
 *
 * Arguments
 * ---------
 *  - c   : Character to print
 *  - num : Number of times to print the character
 *
 * Returns : Status of the transmission
 *   0: Success
 *   1: Data too long to fit in transmit buffer
 *   2: Received NACK on transmit of address
 *   3: Received NACK on transmit of data
 *   4: Other error
 */
void US2066::fill( char c, uint8_t num ) {
    if( this->_init == false ) {
        this->begin();
    }

    if( num == 0 ) {
        return;
    }

    if( this->_init == false ) {
        this->begin();
    }

    while( num-- ) {
        this->print( c );
    }
}


/*--------------------------------------------------------------------------
 *
 * Prints a single character on the LCD module at the current coordinates.
 *
 * Arguments
 * ---------
 *  - c : Character to print
 *
 * Returns : Status of the transmission
 *   0: Success
 *   1: Data too long to fit in transmit buffer
 *   2: Received NACK on transmit of address
 *   3: Received NACK on transmit of data
 *   4: Other error
 */
uint8_t US2066::print( char c ) {
    if( this->_init == false ) {
        this->begin();
    }

    Wire.beginTransmission( this->_address );

    Wire.write( US2066_MODE_DATA );
    Wire.write( c );

    return Wire.endTransmission();
}


/*--------------------------------------------------------------------------
 *
 * Prints a string on the LCD module at the current coordinates.
 *
 * Arguments
 * ---------
 *  - str           : Pointer to the string to print.
 *  - ptr_pgm_space : True if 'str' points to a program memory location, false otherwize
 *
 * Returns : Number of characters written
 */
uint8_t US2066::print( const char *str, bool ptr_pgm_space ) {
    if( this->_init == false ) {
        this->begin();
    }

    uint8_t num = 0;

    if( this->_init == false ) {
        this->begin();
    }


    while( true ) {

        char c;

        if( ptr_pgm_space == true ) {
            c = pgm_read_byte( str++ );

        } else {
            c = *str++;
        }

        if( c == 0x00 ) {
            break;
        }

        if( this->print( c ) != 0 ) {
            return num;
        }

        num++;
    }

    return num;
}


/*--------------------------------------------------------------------------
 *
 * Prints a string with padding on the LCD module at the current coordinates.
 *
 * Arguments
 * ---------
 *  - str           : Pointer to the string to print.
 *  - length        : Maximum number of characters to print, including padding
 *  - align         : Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT )
 *  - ptr_pgm_space : True if 'str' points to a program memory location, false otherwize
 *
 * Returns : Number of characters written
 */
uint8_t US2066::print( const char *str, uint8_t length, uint8_t align, bool ptr_pgm_space ) {
    uint8_t res;
    uint8_t num = 0;
    uint8_t slen;
    uint8_t pre_padding;
    uint8_t post_padding;

    if( this->_init == false ) {
        this->begin();
    }

    if( ptr_pgm_space == true ) {
        slen = strlen_P( str );

    } else {
        slen = strlen( str );
    }

    if( slen < length ) {
        switch( align ) {
            case TEXT_ALIGN_CENTER:
                pre_padding = ( length - slen ) / 2 ;
                post_padding = length - slen - pre_padding;
                break;

            case TEXT_ALIGN_RIGHT:
                pre_padding = length - slen;
                post_padding = 0;
                break;

            /* TEXT_ALIGN_LEFT */
            default:
                pre_padding = 0;
                post_padding = length - slen;
                break;
        }

    } else {
        pre_padding = 0;
        post_padding = 0;
    }


    uint8_t i;

    for( i = 0; i < length; i++ ) {

        if( pre_padding > 0 ) {
            if( this->print( CHAR_SPACE ) != 0 ) {
                return num;
            }

            pre_padding--;
            num++;
            continue;
        }

        if( slen > 0 ) {

            if( ptr_pgm_space == true ) {
                res = this->print( pgm_read_byte( str++ ) );

            } else {
                res = this->print( *str++ );
            }


            if( res != 0 ) {
                return num;
            }

            slen--;
            num++;
            continue;
        }

        if( post_padding > 0 ) {
            if( this->print( CHAR_SPACE ) != 0 ) {
                return num;
            }

            post_padding--;
            num++;
            continue;
        }
    }

    return num;
}


/*--------------------------------------------------------------------------
 *
 * Prints a formated string on the LCD module at the current coordinates.
 *
 * Arguments
 * ---------
 *  - format : Pointer to the string to print.
 *  - ...    : Additional arguments.
 *
 * Returns : Number of characters printed.
 */
uint8_t US2066::printf( const char *format, ... ) {

    if( this->_init == false ) {
        this->begin();
    }

    va_list args;
    va_start( args, format );

    uint8_t length;
    length = vfprintf( &this->_lcdout, format, args );

    va_end( args );

    return length;
}


/*--------------------------------------------------------------------------
 *
 * Prints a formated string on the LCD module at the current coordinates
 * using format string stored in program memory.
 *
 * Arguments
 * ---------
 *  - format : Pointer to the string to print.
 *  - ...    : Additional arguments.
 *
 * Returns : Number of characters printed.
 */
uint8_t US2066::printf_P( const char *format, ... ) {

    if( this->_init == false ) {
        this->begin();
    }

    va_list args;
    va_start( args, format );

    uint8_t length;
    length = vfprintf_P( &this->_lcdout, format, args );

    va_end( args );

    return length;
}


/*--------------------------------------------------------------------------
 *
 * Write callback function for the output stream.
 *
 * Arguments
 * ---------
 *  - format : Pointer to the string to print.
 *  - args   : Variable argument list.
 *
 * Returns : Number of characters printed.
 */
int US2066::_putchar( char ch, FILE *stream ) {

    US2066 *lcd;
    lcd = ( US2066* )fdev_get_udata( stream );
    
    return lcd->print( ch );
}