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
#include "time.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   User stream callback for sending a character.
 *
 * @param   c         Character to send
 * @param   stream    Stream object from which the call originate from.
 *
 * @return  Number of bytes written
 * 
 */
int IPrint::_cb_putchar( char c, FILE *stream ) {

    /* Get the pointer to the IPrint class */
    IPrint *obj;
    obj = ( IPrint* )fdev_get_udata( stream );
    
    return obj->print( c );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the IPrint interface
 * 
 */
void IPrint::_initPrint() {

    fdev_setup_stream( &_stream, this->_cb_putchar, NULL, _FDEV_SETUP_WRITE );

    /* Store the pointer to this class */
    fdev_set_udata( &_stream, ( void * )this );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array
 *
 * @param   str              Pointer to the string to print.
 * @param   ptr_pgm_space    TRUE if 'str' points to a program memory 
 *                           location, FALSE otherwize
 *
 * @return  Number of characters written
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array within a fixed length. Adds padding before 
 *          or after the character array if it is less than the fixed length
 *
 * @param   str              Pointer to the string to print.
 * @param   length           Maximum number of characters to print, including 
 *                           padding
 * @param   align            Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, 
 *                           TEXT_ALIGN_RIGHT )
 * @param   ptr_pgm_space    True if 'str' points to a program memory location, 
 *                           FALSE otherwize
 *
 * @return  Number of characters written
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a single character
 *
 * @param   c    Character to print
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::print( char c ) {

    /* Call the print function implemented in the parent class */
    return this->_print( c );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in SRAM
 *
 * @param   str    Pointer to the string to print.
 *
 * @return  Number of characters written
 */
uint8_t IPrint::print( const char *str ) {
    return this->_print( str, false );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in SRAM within a fixed length. 
 * 
 * @details Adds padding before or after the character array if it's width 
 *          is less than the fixed length.
 *
 * @param   str       Pointer to the string to print.
 * @param   length    Maximum number of characters to print, including padding
 * @param   align     Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, 
 *                    TEXT_ALIGN_RIGHT )
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::print( const char *str, uint8_t length, uint8_t align ) {
    return this->_print( str, length, align, false );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sends a carriage return character
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::println() {
    return this->_print( "\r\n", false );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in SRAM and sends a 
 *          carriage return character
 *
 * @param   str    Pointer to the string to print.
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::println( const char *str ) {
    uint8_t n;

    n = this->_print( str, false );
    n += this->println();

    return n;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in SRAM within a fixed length 
 *          and sends a carriage return. 
 * 
 * @details Adds padding before or after the character array if it's width 
 *          is less than the fixed length.
 *
 * @param   str       Pointer to the string to print.
 * @param   length    Maximum number of characters to print, including padding
 * @param   align     Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, 
 *                    TEXT_ALIGN_RIGHT )
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::println( const char *str, uint8_t length, uint8_t align ) {
    uint8_t n;

    n = this->_print( str, length, align, false );
    n += this->println();

    return n;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a formated string using a format string contained in SRAM.
 *
 * @param   format    Pointer to the string to print.
 * @param   ...       Additional arguments.
 *
 * @return  Number of characters printed.
 * 
 */
uint8_t IPrint::printf( const char *format, ... ) {
    va_list args;
    va_start( args, format );

    uint8_t length;
    length = vfprintf( &_stream, format, args );

    va_end( args );

    return length;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in program memory
 *
 * @param   str    Pointer to the string to print.
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::print_P( const char *str ) {
    return this->_print( str, true );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in program memory within 
 *          a fixed length. 
 * 
 * @details Adds padding before or after the character array if it's width 
 *          is less than the fixed length.
 *
 * @param   str       Pointer to the string to print.
 * @param   length    Maximum number of characters to print, including padding
 * @param   align     Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, 
 *                    TEXT_ALIGN_RIGHT )
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::print_P( const char *str, uint8_t length, uint8_t align ) {
    return this->_print( str, length, align, true );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in program memory and sends 
 *          a carriage return character
 *
 * @param   str    Pointer to the string to print.
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::println_P( const char *str ) {
    uint8_t n;

    n = this->_print( str, true );
    n += this->println();

    return n;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a character array contained in program memory within a 
 *          fixed length and sends a carriage return. 
 * 
 * @details Adds padding before or after the character array if it's width 
 *          is less than the fixed length.
 *
 * @param   str       Pointer to the string to print.
 * @param   length    Maximum number of characters to print, including padding
 * @param   align     Text alignment ( TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, 
 *                    TEXT_ALIGN_RIGHT )
 *
 * @return  Number of characters written
 * 
 */
uint8_t IPrint::println_P( const char *str, uint8_t length, uint8_t align ) {
    uint8_t n;

    n = this->_print( str, length, align, true );
    n += this->println();

    return n;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a formated string using a format string contained in 
 *          program memory.
 *
 * @param   format    Pointer to the string to print.
 * @param   ...       Additional arguments.
 *
 * @return  Number of characters printed.
 * 
 */
uint8_t IPrint::printf_P( const char *format, ... ) {
    va_list args;
    va_start( args, format );

    uint8_t length;
    length = vfprintf_P( &_stream, format, args );

    va_end( args );

    return length;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a formated time interval
 *
 * @param   time         Number of seconds
 * @param   separator    String in program memory to use to separate components
 * @param   compact      TRUE prints a compact representation of the inteval, 
 *                       FALSE otherwise
 *
 * @return  Number of characters printed.
 * 
 */
uint8_t IPrint::printTimeInterval( unsigned long time, const char *separator, bool compact ) {

    uint8_t seconds, minutes, hours;
    seconds = ( time % 60L );
    minutes = (( time % 3600 ) / 60L );
    hours = (( time % 86400 ) / 3600L );

    uint16_t days;
    days = ( time / 86400L );

    /* Prints the number of days, if any */
    if( days == 1 ) {
        this->print_P( (compact == true) ? S_DATETIME_1D : S_DATETIME_1DD );
    } else if ( days > 1 && compact == true ) {
        this->printf_P( S_DATETIME_D, days );
    } else if ( days > 1 && compact == false ) {
        this->printf_P( S_DATETIME_DD, days );
    }

    if( days > 0 && ( hours > 0 || minutes > 0 || seconds > 0 )) {
        this->print_P( separator );
    }

    /* Prints the number of hours, if any */
    if( hours == 1 ) {
        this->print_P( (compact == true) ? S_DATETIME_1H : S_DATETIME_1HH );
    } else if ( hours > 1 && compact == true ) {
        this->printf_P( S_DATETIME_H, hours );
    } else if ( hours > 1 && compact == false ) {
        this->printf_P( S_DATETIME_HH, hours );
    }

    if( hours > 0 && ( minutes > 0 || seconds > 0 )) {
        this->print_P( separator );
    }

    /* Prints the number of minutes, if any */
    if( minutes == 1 ) {
        this->print_P( (compact == true) ? S_DATETIME_1M : S_DATETIME_1MM );
    } else if ( minutes > 1 && compact == true ) {
        this->printf_P( S_DATETIME_M, minutes );
    } else if ( minutes > 1 && compact == false ) {
        this->printf_P( S_DATETIME_MM, minutes );
    }

    if( minutes > 0 && ( seconds > 0 )) {
        this->print_P( separator );
    }

    /* Prints the number of seconds, if any */
    if( seconds == 1 ) {
        this->print_P( (compact == true) ? S_DATETIME_1S : S_DATETIME_1SS );
    } else if ( seconds > 1 && compact == true ) {
        this->printf_P( S_DATETIME_S, seconds );
    } else if ( seconds > 1 && compact == false ) {
        this->printf_P( S_DATETIME_SS, seconds );
    }
}