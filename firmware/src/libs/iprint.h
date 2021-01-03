//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/libs/iprint.h
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
#ifndef I_PRINT_H
#define I_PRINT_H

#include <Arduino.h>
#include "../resources.h"

#define TEXT_ALIGN_LEFT     0
#define TEXT_ALIGN_CENTER   1
#define TEXT_ALIGN_RIGHT    2


class IPrint {

  public: 

    uint8_t print( char c );
    uint8_t print( const char *str );
    uint8_t print( const char *str, uint8_t length, uint8_t align );
    uint8_t println();
    uint8_t println( const char *str );
    uint8_t println( const char *str, uint8_t length, uint8_t align );
    uint8_t printf( const char *format, ... );

    uint8_t print_P( const char *str );
    uint8_t print_P( const char *str, uint8_t length, uint8_t align );
    uint8_t println_P( const char *str );
    uint8_t println_P( const char *str, uint8_t length, uint8_t align );
    uint8_t printf_P( const char *format, ... );

    uint8_t printTimeInterval( long seconds, const char *separator = S_DATETIME_SEPARATOR_SPACE, bool compact = false );
    
  private:
    FILE _stream = {0};
    static int _cb_putchar( char ch, FILE *stream );

    virtual uint8_t _print( char c ) = 0;
    uint8_t _print( const char *str, bool ptr_pgm_space = false );
    uint8_t _print( const char *str, uint8_t length, uint8_t align, bool ptr_pgm_space = false );

  protected:
    void _initPrint();
    
};


#endif /* I_PRINT_H */