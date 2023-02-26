//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/cmd_juliette.cpp
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

#include "console_base.h"
#include "ansi_art.h"


/*******************************************************************************
 * 
 * @brief   Starts the print ANSI art task.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool ConsoleBase::beginPrintJulietteANSI() {
    _taskIndex = 0;
    
    this->startTask( TASK_CONSOLE_PRINT_JULIETTE_ANSI );
    this->println();

    return true;
}


/*******************************************************************************
 * 
 * @brief   Print the next line of the ANSI art file to the console
 * 
 */
void ConsoleBase::runTaskPrintJulietteANSI() {

    this->print("\033[38;2;211;215;207m");

    uint8_t nchar = 0;
    uint16_t chr;
    while( (uint16_t)_taskIndex < (uint16_t)sizeof( juliette_bitmap )) { 

        if( nchar > 32 ) {
            return;
        }
        
        uint8_t repeat = 1;
        chr = pgm_read_byte( &juliette_bitmap[ _taskIndex++ ]);

        /* End of line character */
        if( chr == 0x0A ) {
            break;
        }

        /* If the read character is a repeat marker, read the repeat count */
        if( chr == 0x00 ) {
            repeat = pgm_read_byte( &juliette_bitmap[ _taskIndex++ ]);
            chr = pgm_read_byte( &juliette_bitmap[ _taskIndex++ ]);
        }

        /* If the read character is a unicode marker (0x01), read the 16-bit unicode character */
        if( chr == 0x01 ) {
            chr = pgm_read_byte( &juliette_bitmap[ _taskIndex++ ]) << 8;
            chr += pgm_read_byte( &juliette_bitmap[ _taskIndex++ ]);
        }

        /* read the RGB565 color value */
        uint16_t color;
        color = pgm_read_byte( &juliette_bitmap[ _taskIndex++ ]) << 8;
        color += pgm_read_byte( &juliette_bitmap[ _taskIndex++ ]);

        uint8_t r = (( color & 0xF800 ) >> 11) * 255 / 31;
        uint8_t g = (( color & 0x7E0 ) >> 5 ) * 255 / 63;
        uint8_t b = ( color & 0x1F ) * 255 / 31;

        /* Print the color escape sequence */
        nchar += this->printf("\033[48;2;%d;%d;%dm", r, g, b);

        while( repeat-- > 0 ) {

            /* Print Unicode character, translate from utf-16 to utf-8 (0x) */
            if( chr > 0xFF ) {

                nchar += this->print( 0xE0 | ( chr >> 12 ));
                nchar += this->print( 0x80 | (( chr >> 6 ) & 0x3F ));
                nchar += this->print( 0x80 | ( chr & 0x3F ));

            /* Regular ASCII */
            } else {
                nchar += this->print(chr);
            }
        }

        nchar++;
    }

    this->print("\033[0m\r\n");

    if( (uint16_t)_taskIndex >= (uint16_t)sizeof( juliette_bitmap )) {

        this->endTask( TASK_SUCCESS );
        return;
    }
}