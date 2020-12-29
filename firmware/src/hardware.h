//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/hardware.h
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

#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "libs/time.h"
#include "libs/timezone.h"

#include "drivers/tpa2016.h"
#include "drivers/vs1053.h"
#include "drivers/rtc.h"
#include "drivers/lamp.h"
#include "drivers/qt1070.h"
#include "drivers/us2066.h"
#include "drivers/neoclock.h"
#include "drivers/neopixel.h"
#include "drivers/wifimanager.h"
#include "drivers/bq27441.h"
#include "drivers/power.h"
#include "drivers/tsl2591.h"



// ----------------------------------------
// Pins
// ----------------------------------------
#define PIN_INT_RTC         2
#define PIN_INT_KEYPAD      3
#define PIN_OLED_RESET      5
#define PIN_WIFI_CS         13
#define PIN_WIFI_IRQ        19
#define PIN_WIFI_RESET      11
#define PIN_WIFI_ENABLE     12
#define PIN_FACTORY_RESET   6
#define PIN_NEOCLOCK        37
#define PIN_PIX_LAMP        35
#define PIN_PIX_SHDN        30
#define PIN_ALARM_SW        25
#define PIN_VS1053_RESET    9
#define PIN_VS1053_DREQ     18
#define PIN_VS1053_CS       16
#define PIN_VS1053_SDCS     4
#define PIN_VS1053_XDCS     29
#define PIN_SD_DETECT       33
#define PIN_AMP_SHDN        28
#define PIN_ON_BATTERY      A14
#define PIN_SYSOFF          A8


// ----------------------------------------
// Devices I2C addresses
// ----------------------------------------
#define I2C_ADDR_OLED       0x3c
#define I2C_ADDR_TLS2561    0x29


#endif /* HARDWARE_H */