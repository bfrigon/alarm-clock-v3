//******************************************************************************
//
// Project : Alarm Clock V3
// File    : lib/asprintf/asprintf.cpp
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

#include "asprintf.h"



int vasprintf( char **buffer, const char *format, va_list args ) {

    /* Duplicate list of arguments for length measuring */   
    va_list args_cpy;
    va_copy( args_cpy, args );

    /* Measure required buffer length */
    int length;
    length = vsnprintf( NULL, 0, format, args_cpy );
    va_end( args_cpy );

    if( length < 0 ) {
        return length;
    }

    /* Allocate buffer */
    char *tempbuf;
    tempbuf = ( char* )malloc( length + 1 );

    if( tempbuf == nullptr ) {
        return -1;
    }

    length = vsnprintf( tempbuf, length + 1, format, args );

    if( length < 0 )
        free( tempbuf );
    else
        *buffer = tempbuf;

    return length;
}



int asprintf( char **buffer, const char *format, ... ) {
    int length;
    va_list args;

    va_start( args, format );
    length = vasprintf( buffer, format, args );
    va_end( args );

    return length;
}


int vasprintf_P( char **buffer, const char *format, va_list args ) {

    /* Duplicate list of arguments for length measuring */   
    va_list args_cpy;
    va_copy( args_cpy, args );

    /* Measure required buffer length */
    int length;
    length = vsnprintf_P( NULL, 0, format, args_cpy );
    va_end( args_cpy );

    if( length < 0 ) {
        return length;
    }

    /* Allocate buffer */
    char *tempbuf;
    tempbuf = ( char* )malloc( length + 1 );

    if( tempbuf == nullptr ) {
        return -1;
    }

    length = vsnprintf_P( tempbuf, length + 1, format, args );

    if( length < 0 )
        free( tempbuf );
    else
        *buffer = tempbuf;

    return length;
}


int asprintf_P( char **buffer, const char *format, ... ) {
    int length;
    va_list args;

    va_start( args, format );
    length = vasprintf_P( buffer, format, args );
    va_end( args );

    return length;
}