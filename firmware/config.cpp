//******************************************************************************
//
// Project : Alarm Clock V3
// File    : config.cpp
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

#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "resources.h"


struct Config g_config;


void loadConfig() {
    uint8_t c;
    uint8_t byte;

    strcpy( g_config.ssid, "287744230" );
    strcpy( g_config.wkey, "75fvcx-46820ab+vx12kc" );


    uint16_t magic;
    EEPROM.get( EEPROM_ADDR_MAGIC, magic);

    if (magic != 0xBEEF) {
        restoreDefaultConfig();
        return;
    }

    for ( c = 0; c < sizeof( Config ); c++ ) {
        byte = EEPROM.read( EEPROM_ADDR_CONFIG + c );

        *((( uint8_t* )&g_config ) + c ) = byte;
    }

}

void saveConfig() {
    uint8_t c;
    uint8_t byte;


    /* Put valid config magic number (0xBEEF) */
    EEPROM.update( EEPROM_ADDR_MAGIC + 0, 0xEF );
    EEPROM.update( EEPROM_ADDR_MAGIC + 1, 0xBE );


    for ( c = 0; c < sizeof( Config ); c++ ) {
        byte = *((( uint8_t* )&g_config) + c );

        EEPROM.update( EEPROM_ADDR_CONFIG + c, byte );
    }

    Serial.println("Written EEPROM");
}

void restoreDefaultConfig() {

    initAlarmProfiles();

    /* Store default config */
    saveConfig();
}



void initAlarmProfiles() {

    AlarmProfile profile;

    profile.snoozeDelay = 10;
    profile.volume = 70;
    profile.filename[0] = 0;

    for ( uint8_t i = 0; i < MAX_ALARM_PROFILES; i++ ) {

        strcpy_P( profile.name, S_PROFILE_PREFIX );

        char cnum[3];
        itoa( i + 1, cnum, 10 );
        strcat( profile.name, cnum );

        g_alarm.saveProfile( &profile, i );
    }
}


