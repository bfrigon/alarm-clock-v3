//******************************************************************************
//
// Project : Alarm Clock V3
// File    : lib/freemem/freemem.h
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
#ifndef FREEMEM_H
#define FREEMEM_H

#include <Arduino.h>

extern int g_freeMemory;

int freeMemory();

#endif /* FREEMEM_H */