//******************************************************************************
//
// Project : Alarm Clock V3
// File    : lib/asprintf/asprintf.h
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
#ifndef ASPRINTF_H
#define ASPRINTF_H


#include <Arduino.h>


int vasprintf( char **buffer, const char *format, va_list args );
int asprintf( char **buffer, const char *format, ... );
int vasprintf_P( char **buffer, const char *format, va_list args );
int asprintf_P( char **buffer, const char *format, ... );




#endif /* ASPRINTF_H */