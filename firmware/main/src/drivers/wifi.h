//******************************************************************************
//
// Project : Alarm Clock V3
// File    : wifi.h
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

#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <winc1500_async.h>


void updateWifiConfig();
void enableWifi();
void disableWifi();


#endif /* WIFI_H */