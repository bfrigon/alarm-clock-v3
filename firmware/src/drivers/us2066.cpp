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


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 * @param   address      I2C address of the OLED module
 * @param   pin_reset    Pin connected to the OLED reset line
 * 
 */
US2066::US2066( uint8_t address, uint8_t pin_reset ) {
    this->_address = address;
    this->_pin_reset = pin_reset;

    /* Initialize IPrint interface */
    this->_initPrint();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Perform proper power-up sequence and initialize the LCD module.
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Shutdown the LCD module.
 * 
 */
void US2066::end() {
    if( this->_init == false ) {
        return;
    }

    this->_init = false;

    pinMode( this->_pin_reset, OUTPUT );
    digitalWrite( this->_pin_reset, LOW );


}


/*! ------------------------------------------------------------------------
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


/*! ------------------------------------------------------------------------
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


    Wire.beginTransmission( this->_address );

    Wire.write( US2066_MODE_CMD );
    Wire.write( cmd );

    return Wire.endTransmission();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Send a two bytes command to the LCD module (cmd + data)
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
    Wire.beginTransmission( this->_address );

    Wire.write( US2066_MODE_CMD | US2066_MODE_CONTINUE );
    Wire.write( cmd );

    /* Data following commands in extended mode require D/C#=1 */
    Wire.write( this->_current_iset == US2066_ISET_EXTENDED ? US2066_MODE_DATA : US2066_MODE_CMD );
    Wire.write( data );

    return Wire.endTransmission();
}


/*! ------------------------------------------------------------------------
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the DDRAM address corresponding to the specified 
 *          row and column.
 *
 * @param   row     Row number (0 based)
 * @param   col     Column number (0 based)
 */
void US2066::setPosition( uint8_t row, uint8_t col ) {
    if( this->_init == false ) {
        this->begin();
    }

    uint8_t dram_address = ( col + ( row * 0x40 ) );

    this->sendCommand( US2066_CMD_DDRAM | ( dram_address & 0x7F ) );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Clear the display
 * 
 */
void US2066::clear() {
    if( this->_init == false ) {
        this->begin();
    }

    this->sendCommand( US2066_CMD_CLEAR );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the display contrast
 * 
 * @param   contrast    Contrast value (0-255)
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the cursor state of the LCD module.
 *
 * @param   underline    Underline cursor ON or OFF.
 * @param   blinking     Blinking cursor (block).
 */
void US2066::setCursor( bool underline, bool blinking ) {
    if( this->_init == false ) {
        this->begin();
    }

    this->_state.cursor = underline;
    this->_state.blink = blinking;

    this->updateDisplayState();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Turn the display ON or OFF.
 *
 * @param   on          Turn display On or OFF.
 * @param   reverse     Reverse all pixels on the display.
 */
void US2066::setDisplay( bool on, bool reverse ) {
    if( this->_init == false ) {
        this->begin();
    }

    this->_state.display = on;
    this->_state.reverseDisplay = reverse;

    this->updateDisplayState();

}


/*! ------------------------------------------------------------------------
 *
 * @brief   Update the display control register on the LCD. 
 *          (display ON/OFF, cursor)
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Fills the LCD with the specified number of characters
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
    if( this->_init == false ) {
        this->begin();
    }

    if( num == 0 ) {
        return;
    }

    while( num-- ) {
        this->print( c );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   IPrint interface callback for printing a single character. 
 *          Sends the output to the LCD module at the current coordinates.
 *
 * @param   c    Character to print
 *
 * @return  Number of bytes written
 * 
 */
uint8_t US2066::_print( char c ) {
    if( this->_init == false ) {
        this->begin();
    }

    Wire.beginTransmission( this->_address );

    Wire.write( US2066_MODE_DATA );
    Wire.write( c );

    return ( Wire.endTransmission() == 0 ? 1 : 0 );
}