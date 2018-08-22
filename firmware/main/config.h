//******************************************************************************
//
// Project : Alarm Clock V3
// File    : config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#include "time.h"
#include "alarm.h"

#define EEPROM_ADDR_MAGIC           0
#define EEPROM_ADDR_FIRMWARE_VER    4
#define EEPROM_ADDR_CONFIG          10
#define EEPROM_ADDR_PROFILES        EEPROM_ADDR_CONFIG + ( sizeof( Config ) )


struct Config {
    bool clock_24h = false;
    bool alarm_on[2] = { false, false };
    uint8_t alarm_profile_id[2] = { 0, 0 };


    bool net_dhcp = true;
    uint8_t net_ip[4] = { 10, 0, 0, 125 };
    uint8_t net_mask[4] = { 255, 255, 192, 0 };
    uint8_t net_gateway[4] = { 10, 0, 0, 1 };
    uint8_t net_dns[4] = { 10, 0, 0, 1 };

    uint8_t clock_color = 1;
    uint8_t clock_brightness = 40;
    uint8_t lcd_contrast = 50;
    uint8_t date_format = 0;
    bool tempunit_c = true;

    char ssid[32];
    char wkey[63];
};


void loadConfig();
void saveConfig();
void initAlarmProfiles();
bool loadAlarmProfile( uint8_t id, AlarmProfile &profile );
void saveAlarmProfile( uint8_t id, AlarmProfile &profile );
void restoreDefaultConfig();



extern struct Config g_config;

#endif /* CONFIG_H */
