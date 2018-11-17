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
#include "config.h"
#include "alarm.h"
#include "drivers/lamp.h"



struct Config g_config;


/*--------------------------------------------------------------------------
 *
 * Load settings from EEPROM
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
void loadConfig() {
    uint8_t c;
    uint8_t byte;

    uint16_t magic;
    EEPROM.get( EEPROM_ADDR_MAGIC, magic );

    /* If magic code is not found, assumes the EEPROM is empty or corrupted and
       restore default settings */
    if( magic != 0xBEEF ) {
        restoreDefaultConfig();
        return;
    }

    for( c = 0; c < sizeof( Config ); c++ ) {
        byte = EEPROM.read( EEPROM_ADDR_CONFIG + c );

        *( ( ( uint8_t * )&g_config ) + c ) = byte;
    }
}


/*--------------------------------------------------------------------------
 *
 * Save settings to EEPROM
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
void saveConfig() {
    uint8_t c;
    uint8_t byte;

    /* Save valid config magic number (0xBEEF) */
    EEPROM.update( EEPROM_ADDR_MAGIC + 0, 0xEF );
    EEPROM.update( EEPROM_ADDR_MAGIC + 1, 0xBE );

    for( c = 0; c < sizeof( Config ); c++ ) {
        byte = *( ( ( uint8_t * )&g_config ) + c );

        EEPROM.update( EEPROM_ADDR_CONFIG + c, byte );
    }

    Serial.println( "Written EEPROM" );
}


/*--------------------------------------------------------------------------
 *
 * Restore default settings or initialize EEPROM contents.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
void restoreDefaultConfig() {

    strcpy( g_config.ssid, "f287744230" );
    strcpy( g_config.wkey, "75fvcx-46820ab+vx12kc" );

    g_config.lamp.brightness = 60;
    g_config.lamp.mode = LAMP_MODE_ON;
    g_config.lamp.color = COLOR_WHITE;


    /* Store default config */
    saveConfig();


    /* Save default alarm profiles */
    struct AlarmProfile profile;
    profile.snoozeDelay = 10;
    profile.volume = 30;
    profile.filename[0] = 0;
    profile.time.hour = 0;
    profile.time.minute = 0;
    profile.visualMode = ALARM_VISUAL_NONE;
    profile.effectSpeed = 5;
    profile.gradual = false;
    profile.dow = 0x7F;

    profile.lamp.brightness = 60;
    profile.lamp.color = COLOR_WHITE;
    profile.lamp.mode = LAMP_MODE_OFF;
    profile.lamp.speed = 5;

    for( uint8_t i = 0; i < MAX_ALARM_PROFILES; i++ ) {

        g_alarm.saveProfile( &profile, i );
    }
}