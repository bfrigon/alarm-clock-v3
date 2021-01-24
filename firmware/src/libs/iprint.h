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
#include "time.h"

#define TEXT_ALIGN_LEFT     0
#define TEXT_ALIGN_CENTER   1
#define TEXT_ALIGN_RIGHT    2





class IPrint {

  public: 

    size_t print( char c );
    size_t print( const char *str );
    size_t print( const char *str, uint8_t length, uint8_t align );
    size_t println();
    size_t println( const char *str );
    size_t println( const char *str, uint8_t length, uint8_t align );
    size_t printf( const char *format, ... );
    size_t printfln( const char *format, ... );

    size_t print_P( const char *str );
    size_t print_P( const char *str, uint8_t length, uint8_t align );
    size_t println_P( const char *str );
    size_t println_P( const char *str, uint8_t length, uint8_t align );
    size_t printf_P( const char *format, ... );
    size_t printfln_P( const char *format, ... );

    size_t printTimeInterval( unsigned long seconds, const char *separator = S_DATETIME_SEPARATOR_SPACE, bool compact = false );
    
  private:
    FILE _stream = {0};
    static int _cb_putchar( char ch, FILE *stream );

    virtual size_t _print( char c ) = 0;
    size_t _print( const char *str, bool ptr_pgm_space = false );
    size_t _print( const char *str, uint8_t length, uint8_t align, bool ptr_pgm_space = false );

  protected:
    void _initPrint();
    
};


#endif /* I_PRINT_H */