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

#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "libs/time.h"
#include "resources.h"

#define MAX_ALARM_PROFILES          2
#define ALARM_FILENAME_LENGTH       40
#define ALARM_MESSAGE_LENGTH        16


#define EEPROM_ADDR_MAGIC           0
#define EEPROM_ADDR_FIRMWARE_VER    4
#define EEPROM_ADDR_CONFIG          10
#define EEPROM_ADDR_PROFILES        EEPROM_ADDR_CONFIG + ( sizeof( Config ) )


struct NightLampSettings {
    uint8_t color;
    uint8_t brightness;
    uint8_t delay_off;
    uint8_t speed;
    uint8_t mode;
};

struct AlarmProfile {
    char filename[ ALARM_FILENAME_LENGTH + 1 ];
    char message[ ALARM_MESSAGE_LENGTH + 1 ];
    uint8_t snoozeDelay;
    uint8_t volume;
    bool gradual;
    uint8_t visualMode;
    uint8_t effectSpeed = 5;
    struct Time time;
    uint8_t dow = 0x7F;
    struct NightLampSettings lamp;
};

struct Config {
    bool clock_24h = false;
    bool alarm_on[2] = { false, false };

    bool net_dhcp = true;
    uint8_t net_ip[4] = { 10, 0, 0, 125 };
    uint8_t net_mask[4] = { 255, 255, 192, 0 };
    uint8_t net_gateway[4] = { 10, 0, 0, 1 };
    uint8_t net_dns[4] = { 10, 0, 0, 1 };

    uint8_t clock_color = 1;
    uint8_t clock_brightness = 40;
    uint8_t lcd_contrast = 50;
    uint8_t date_format = 0;


    struct NightLampSettings lamp;

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
