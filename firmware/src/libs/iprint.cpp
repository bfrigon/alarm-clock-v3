//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/iprint.cpp
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
#include "iprint.h"


/*--------------------------------------------------------------------------
 *
 * User stream callback for sending a character.
 *
 * Arguments
 * ---------
 *  - c      : Character to send
 *  - stream : Stream object from which the call originate from.
 *
 * Returns : Number of bytes written
 */
int IPrint::_cb_putchar( char c, FILE *stream ) {

    /* Get the pointer to the IPrint class */
    IPrint *obj;
    obj = ( IPrint* )fdev_get_udata( stream );
    
    return obj->print( c );
}


/*--------------------------------------------------------------------------
 *
 * Initialize the IPrint interface
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void IPrint::_initPrint() {

    fdev_setup_stream( &_stream, this->_cb_putchar, NULL, _FDEV_SETUP_WRITE );

    /* Store the pointer to this class */
    fdev_set_udata( &_stream, ( void * )this );
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array
 *
 * Arguments
 * ---------
 *  - str           : Pointer to the string to print.
 *  - ptr_pgm_space : True if 'str' points to a program memory location, false otherwize
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::_print( const char *str, bool ptr_pgm_space ) {
    uint8_t num = 0;

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

        if( this->_print( c ) == 0 ) {
            return num;
        }

        num++;
    }

    return num;
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array within a fixed length. Adds padding before or after
 * the character array if it is less than the fixed length
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
uint8_t IPrint::_print( const char *str, uint8_t length, uint8_t align, bool ptr_pgm_space ) {
    uint8_t res;
    uint8_t num = 0;
    uint8_t slen;
    uint8_t pre_padding;
    uint8_t post_padding;

    slen = ( ptr_pgm_space == true ? strlen_P( str ) : strlen( str ));

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
            if( this->_print( 0x20 ) == 0 ) {
                return num;
            }

            pre_padding--;
            num++;
            continue;
        }

        if( slen > 0 ) {

            if( ptr_pgm_space == true ) {
                res = this->_print( pgm_read_byte( str++ ) );

            } else {
                res = this->_print( *str++ );
            }


            if( res == 0 ) {
                return num;
            }

            slen--;
            num++;
            continue;
        }

        if( post_padding > 0 ) {
            if( this->_print( 0x20 ) == 0 ) {
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
 * Prints a single character
 *
 * Arguments
 * ---------
 *  - c : Character to print
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::print( char c ) {

    /* Call the print function implemented in the parent class */
    return this->_print( c );
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in SRAM
 *
 * Arguments
 * ---------
 *  - str : Pointer to the string to print.
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::print( const char *str ) {
    return this->_print( str, false );
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in SRAM within a fixed length. 
 * Adds padding before or after the character array if it is less than the 
 * fixed length
 *
 * Arguments
 * ---------
 *  - str    : Pointer to the string to print.
 *  - length : Maximum number of characters to print, including padding
 *  - align  : Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT )
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::print( const char *str, uint8_t length, uint8_t align ) {
    return this->_print( str, length, align, false );
}


/*--------------------------------------------------------------------------
 *
 * Sends a carriage return character
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::println() {
    return this->_print( "\r\n", false );
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in SRAM and sends a carriage return 
 * character
 *
 * Arguments
 * ---------
 *  - str : Pointer to the string to print.
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::println( const char *str ) {
    uint8_t n;

    n = this->_print( str, false );
    n += this->println();

    return n;
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in SRAM within a fixed length and sends
 * a carriage return. Adds padding before or after the character array if it 
 * is less than the fixed length
 *
 * Arguments
 * ---------
 *  - str    : Pointer to the string to print.
 *  - length : Maximum number of characters to print, including padding
 *  - align  : Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT )
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::println( const char *str, uint8_t length, uint8_t align ) {
    uint8_t n;

    n = this->_print( str, length, align, false );
    n += this->println();

    return n;
}


/*--------------------------------------------------------------------------
 *
 * Prints a formated string using a format string contained in SRAM.
 *
 * Arguments
 * ---------
 *  - format : Pointer to the string to print.
 *  - ...    : Additional arguments.
 *
 * Returns : Number of characters printed.
 */
uint8_t IPrint::printf( const char *format, ... ) {
    va_list args;
    va_start( args, format );

    uint8_t length;
    length = vfprintf( &_stream, format, args );

    va_end( args );

    return length;
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in program memory
 *
 * Arguments
 * ---------
 *  - str : Pointer to the string to print.
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::print_P( const char *str ) {
    return this->_print( str, true );
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in program memory within a fixed length. 
 * Adds padding before or after the character array if it is less than the 
 * fixed length
 *
 * Arguments
 * ---------
 *  - str    : Pointer to the string to print.
 *  - length : Maximum number of characters to print, including padding
 *  - align  : Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT )
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::print_P( const char *str, uint8_t length, uint8_t align ) {
    return this->_print( str, length, align, true );
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in program memory and sends a carriage 
 * return character
 *
 * Arguments
 * ---------
 *  - str : Pointer to the string to print.
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::println_P( const char *str ) {
    uint8_t n;

    n = this->_print( str, true );
    n += this->println();

    return n;
}


/*--------------------------------------------------------------------------
 *
 * Prints a character array contained in program memory within a fixed length 
 * and sends a carriage return. Adds padding before or after the character 
 * array if it is less than the fixed length
 *
 * Arguments
 * ---------
 *  - str    : Pointer to the string to print.
 *  - length : Maximum number of characters to print, including padding
 *  - align  : Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT )
 *
 * Returns : Number of characters written
 */
uint8_t IPrint::println_P( const char *str, uint8_t length, uint8_t align ) {
    uint8_t n;

    n = this->_print( str, length, align, true );
    n += this->println();

    return n;
}


/*--------------------------------------------------------------------------
 *
 * Prints a formated string using a format string contained in program memory.
 *
 * Arguments
 * ---------
 *  - format : Pointer to the string to print.
 *  - ...    : Additional arguments.
 *
 * Returns : Number of characters printed.
 */
uint8_t IPrint::printf_P( const char *format, ... ) {
    va_list args;
    va_start( args, format );

    uint8_t length;
    length = vfprintf_P( &_stream, format, args );

    va_end( args );

    return length;
}