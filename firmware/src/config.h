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
#include <SdFat.h>
#include <avr/pgmspace.h>
#include <IPAddress.h>

#include "libs/time.h"
#include "libs/task.h"
#include "resources.h"



#define CONFIG_BACKUP_FILENAME          "config.txt"

/* Limits */
#define MAX_LENGTH_SETTING_NAME         32
#define MAX_LENGTH_SETTING_VALUE        96
#define MAX_NUM_PROFILES                2
#define MAX_LENGTH_ALARM_FILENAME       12
#define MAX_LENGTH_ALARM_MESSAGE        16
#define MAX_SSID_LENGTH                 32
#define MAX_WKEY_LENGTH                 63
#define MIN_CLOCK_BRIGHTNESS            15
#define MAX_CLOCK_BRIGHTNESS            80
#define MIN_LAMP_BRIGHTNESS             5
#define MAX_LAMP_BRIGHTNESS             100
#define MIN_LAMP_DELAY_OFF              0
#define MAX_LAMP_DELAY_OFF              90
#define MIN_LCD_CONTRAST                0
#define MAX_LCD_CONTRAST                100
#define MIN_ALARM_LAMP_BRIGHTNESS       25
#define MAX_ALARM_LAMP_BRIGHTNESS       100
#define MIN_ALARM_LAMP_EFFECT_SPEED     1
#define MAX_ALARM_LAMP_EFFECT_SPEED     10
#define MIN_ALARM_SNOOZE_TIME           0
#define MAX_ALARM_SNOOZE_TIME           30
#define MIN_ALARM_VOLUME                20
#define MAX_ALARM_VOLUME                100
#define MIN_ALARM_VISUAL_EFFECT_SPEED   1
#define MAX_ALARM_VISUAL_EFFECT_SPEED   10

/* EEPROM addresses */
#define EEPROM_ADDR_MAGIC               0
#define EEPROM_ADDR_FIRMWARE_VER        4
#define EEPROM_ADDR_CONFIG              10
#define EEPROM_ADDR_PROFILES            EEPROM_ADDR_CONFIG + ( sizeof( GlobalSettings ) )

/* Settings type */
#define SETTING_TYPE_UNKNOWN        0
#define SETTING_TYPE_BOOL           1
#define SETTING_TYPE_STRING         2
#define SETTING_TYPE_SECTION        3
#define SETTING_TYPE_INTEGER        4
#define SETTING_TYPE_IP             5
#define SETTING_TYPE_COMMENT        6
#define SETTING_TYPE_TIME           7
#define SETTING_TYPE_DOW            8

/* Settings names */
#define SETTING_VALUE_FALSE         "off"
#define SETTING_VALUE_TRUE          "on"

PROG_STR( COMMENT_FILE_HEADER,
          "; -----------------------\r\n"
          "; Alarm clock V3 settings\r\n"
          "; -----------------------\r\n" );

PROG_STR( SETTING_NAME_SECTION_CLOCK,       "clock" );
PROG_STR( SETTING_NAME_SECTION_LCD,         "lcd" );
PROG_STR( SETTING_NAME_SECTION_LAMP,        "lamp" );
PROG_STR( SETTING_NAME_SECTION_NETWORK,     "network" );
PROG_STR( SETTING_NAME_SECTION_ALARM,       "alarm" );
PROG_STR( SETTING_NAME_24H,                 "24h" );
PROG_STR( SETTING_NAME_COLOR,               "color" );
PROG_STR( SETTING_NAME_BRIGHTNESS,          "brightness" );
PROG_STR( SETTING_NAME_DELAY,               "delay" );
PROG_STR( SETTING_NAME_DATEFMT,             "datefmt" );
PROG_STR( SETTING_NAME_CONTRAST,            "contrast" );
PROG_STR( SETTING_NAME_VOLUME,              "volume" );
PROG_STR( SETTING_NAME_TIME,                "time" );
PROG_STR( SETTING_NAME_DOW,                 "dow" );
PROG_STR( SETTING_NAME_FILENAME,            "filename" );
PROG_STR( SETTING_NAME_MESSAGE,             "message" );
PROG_STR( SETTING_NAME_GRADUAL,             "gradual" );
PROG_STR( SETTING_NAME_VISUAL_MODE,         "effect-mode" );
PROG_STR( SETTING_NAME_VISUAL_SPEED,        "effect-speed" );
PROG_STR( SETTING_NAME_DHCP,                "dhcp" );
PROG_STR( SETTING_NAME_ADDRESS,             "address" );
PROG_STR( SETTING_NAME_MASK,                "mask" );
PROG_STR( SETTING_NAME_GATEWAY,             "gateway" );
PROG_STR( SETTING_NAME_DNS,                 "dns" );
PROG_STR( SETTING_NAME_SSID,                "ssid" );
PROG_STR( SETTING_NAME_WKEY,                "passphrase" );
PROG_STR( SETTING_NAME_ENABLED,             "enabled" );
PROG_STR( SETTING_NAME_SNOOZE,              "snooze" );
PROG_STR( SETTING_NAME_LAMP_COLOR,          "lamp-color" );
PROG_STR( SETTING_NAME_LAMP_MODE,           "lamp-mode" );
PROG_STR( SETTING_NAME_LAMP_SPEED,          "lamp-speed" );
PROG_STR( SETTING_NAME_LAMP_BRIGHTNESS,     "lamp-brightness" );

/* Settings ID's */
#define SETTING_ID_CLOCK_24H                1   /* Begin clock section */
#define SETTING_ID_CLOCK_COLOR              2
#define SETTING_ID_CLOCK_BRIGHTNESS         3
#define SETTING_ID_LCD_DATEFMT              4   /* Begin LCD section */
#define SETTING_ID_LCD_CONTRAST             5
#define SETTING_ID_LAMP_COLOR               6
#define SETTING_ID_LAMP_BRIGHTNESS          7
#define SETTING_ID_LAMP_DELAY               8
#define SETTING_ID_NETWORK_DHCP             9   /* Begin network section */
#define SETTING_ID_NETWORK_IP               10
#define SETTING_ID_NETWORK_MASK             11
#define SETTING_ID_NETWORK_GATEWAY          12
#define SETTING_ID_NETWORK_DNS              13
#define SETTING_ID_NETWORK_SSID             14
#define SETTING_ID_NETWORK_WKEY             15
#define SETTING_ID_ALARM_BEGIN              16  /* Begin alarm section */
#define SETTING_ID_ALARM_ENABLED            16
#define SETTING_ID_ALARM_FILENAME           17
#define SETTING_ID_ALARM_TIME               18
#define SETTING_ID_ALARM_SNOOZE             19
#define SETTING_ID_ALARM_VOLUME             20
#define SETTING_ID_ALARM_GRADUAL            21
#define SETTING_ID_ALARM_DOW                22
#define SETTING_ID_ALARM_MESSAGE            23
#define SETTING_ID_ALARM_VISUAL             24
#define SETTING_ID_ALARM_VISUAL_SPEED       25
#define SETTING_ID_ALARM_LAMP_MODE          26
#define SETTING_ID_ALARM_LAMP_SPEED         27
#define SETTING_ID_ALARM_LAMP_COLOR         28
#define SETTING_ID_ALARM_LAMP_BRIGHTNESS    29
#define SETTING_ID_END                      29

/* Section ID's */
#define SECTION_ID_UNKNOWN                  0
#define SECTION_ID_ANY                      0
#define SECTION_ID_CLOCK                    1
#define SECTION_ID_LCD                      2
#define SECTION_ID_LAMP                     3
#define SECTION_ID_NETWORK                  4
#define SECTION_ID_ALARM                    5

/* Setting parser token types */
#define TOKEN_NAME                          0
#define TOKEN_VALUE                         1
#define TOKEN_WHITESPACE                    2
#define TOKEN_COMMENT                       3
#define TOKEN_LIST                          4

/* Tasks */
#define TASK_BACKUP_CONFIG                  1
#define TASK_RESTORE_CONFIG                 2

/* Task error status */
#define TASK_ERROR_NO_SDCARD                1
#define TASK_ERROR_WRITE                    2
#define TASK_ERROR_NOT_FOUND                3
#define TASK_ERROR_READ                     4
#define TASK_ERROR_CANT_OPEN                5


//**************************************************************************
//
// Memory writable structure containing night lamp settings */
//
//**************************************************************************
struct NightLampSettings {
    uint8_t color;
    uint8_t brightness;
    uint8_t delay_off;
    uint8_t speed;
    uint8_t mode;
};


//**************************************************************************
//
// Memory writable structure containing alarm profile settings */
//
//**************************************************************************
struct AlarmProfile {
    char filename[ MAX_LENGTH_ALARM_FILENAME + 1 ];
    char message[ MAX_LENGTH_ALARM_MESSAGE + 1 ];
    uint8_t snoozeDelay;
    uint8_t volume;
    bool gradual;
    uint8_t visualMode;
    uint8_t effectSpeed = 5;
    struct Time time;
    uint8_t dow = 0x7F;
    struct NightLampSettings lamp;
};


//**************************************************************************
//
// Memory writable structure containing all settings */
//
//**************************************************************************
struct GlobalSettings {
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

    char ssid[ MAX_SSID_LENGTH + 1 ];
    char wkey[ MAX_WKEY_LENGTH + 1 ];
};



//**************************************************************************
//
// Configuration manager class
//
//**************************************************************************
class ConfigManager : public Task {
  public:

    void reset();
    void load();
    void save();
    void apply();
    bool isEepromValid();
    void formatEeprom();
    bool startRestore();
    bool startBackup( bool overwrite = true );
    void endBackup( int error = TASK_SUCCESS );
    void endRestore( int error = TASK_SUCCESS );
    void runTask();

    GlobalSettings settings;

  private:

    bool writeNextLine();
    void writeConfigLine( const char* name, uint8_t type, void* value );
    bool readNextLine();
    uint8_t parseConfigLine( char* name, char* value );
    void parseSettingValue( char* src, void* dest, uint8_t type, uint8_t min = 0, uint8_t max = 255 );
    bool matchSettingName( char* testName, const char* name, uint8_t section );


    uint8_t _currentSettingID = 0;
    uint8_t _currentSectionID = 0;
    int8_t _currentAlarmID = -1;

    FatFile _sd_file;
};

extern ConfigManager g_config;

#endif /* CONFIG_H */
