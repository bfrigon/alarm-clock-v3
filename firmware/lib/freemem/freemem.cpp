//******************************************************************************
//
// Project : Alarm Clock V3
// File    : lib/freemem/freemem.cpp
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

#include "freemem.h"

extern char *__brkval;
int g_freeMemory;


/*! ------------------------------------------------------------------------
 *
 * @brief   Calculate the free SRAM memory
 * 
 * @return  Free memory amount in bytes
 */
int freeMemory() {
  char top;

  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
}

