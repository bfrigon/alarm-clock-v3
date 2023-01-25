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
#include <time.h>
#include <itask.h>
#include <resources.h>


#define IPADDRESS_TO_ARRAY( src, dest )   dest[ 0 ] = src[ 0 ]; \
                                          dest[ 1 ] = src[ 1 ]; \
                                          dest[ 2 ] = src[ 2 ]; \
                                          dest[ 3 ] = src[ 3 ]; 

#define BATTERY_DESIGN_CAPACITY         350


#define CONFIG_BACKUP_FILENAME          "config.txt"


/* Limits */
#define MAX_LENGTH_SETTING_NAME         32
#define MAX_LENGTH_SETTING_VALUE        96
#define MAX_NUM_PROFILES                2
#define MAX_LENGTH_ALARM_FILENAME       12
#define MAX_LENGTH_ALARM_MESSAGE        16
#define MAX_SSID_LENGTH                 32
#define MAX_HOSTNAME_LENGTH             32
#define MAX_NTPSERVER_LENGTH            64
#define MAX_WKEY_LENGTH                 63
#define MIN_CLOCK_BRIGHTNESS            10
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
#define MAX_TZ_NAME_LENGTH              40
#define MAX_MQTT_HOST_LENGTH            64
#define MAX_MQTT_USERNAME_LENGTH        32
#define MAX_MQTT_PASSWORD_LENGTH        32
#define MAX_DISCOVERY_PREFIX_LENGTH     32

/* EEPROM addresses */
#define EEPROM_ADDR_MAGIC               0
#define EEPROM_ADDR_FIRMWARE_VER        4
#define EEPROM_ADDR_CLOCK_CONFIG        10
#define EEPROM_ADDR_NETWORK_CONFIG      EEPROM_ADDR_CLOCK_CONFIG + ( sizeof( ClockSettings ) )
#define EEPROM_ADDR_PROFILES            EEPROM_ADDR_NETWORK_CONFIG + ( sizeof( NetworkSettings ) )

/* EEPROM settings sections */
#define EEPROM_SECTION_CLOCK            0x01
#define EEPROM_SECTION_NETWORK          0x02
#define EEPROM_SECTION_ALL              EEPROM_SECTION_CLOCK | EEPROM_SECTION_NETWORK

/* Settings type */
enum {
    SETTING_TYPE_UNKNOWN = 0,
    SETTING_TYPE_BOOL,
    SETTING_TYPE_STRING,
    SETTING_TYPE_SECTION,
    SETTING_TYPE_INTEGER,
    SETTING_TYPE_IP,
    SETTING_TYPE_COMMENT,
    SETTING_TYPE_TIME,
    SETTING_TYPE_DOW,
    SETTING_TYPE_SHORT,
    SETTING_TYPE_TIMEZONE,
};

#define SETTING_VALUE_FALSE                 "off"
#define SETTING_VALUE_TRUE                  "on"


PROG_STR( COMMENT_FILE_HEADER,              "; -----------------------\r\n"
                                            "; Alarm clock V3 settings\r\n"
                                            "; -----------------------\r\n" );

/* Config file settings labels */
PROG_STR( SETTING_NAME_SECTION_CLOCK,       "clock" );
PROG_STR( SETTING_NAME_SECTION_ALS,         "als" );
PROG_STR( SETTING_NAME_SECTION_LCD,         "lcd" );
PROG_STR( SETTING_NAME_SECTION_LAMP,        "lamp" );
PROG_STR( SETTING_NAME_SECTION_NETWORK,     "network" );
PROG_STR( SETTING_NAME_SECTION_ALARM,       "alarm" );
PROG_STR( SETTING_NAME_SECTION_MQTT,        "mqtt-client" );
PROG_STR( SETTING_NAME_SECTION_HA,          "home-assistant" );
PROG_STR( SETTING_NAME_24H,                 "24h" );
PROG_STR( SETTING_NAME_COLOR,               "color" );
PROG_STR( SETTING_NAME_BRIGHTNESS,          "brightness" );
PROG_STR( SETTING_NAME_DELAY,               "delay" );
PROG_STR( SETTING_NAME_DATEFMT,             "datefmt" );
PROG_STR( SETTING_NAME_USE_NTP,             "ntp" );
PROG_STR( SETTING_NAME_TIMEZONE,            "timezone" );
PROG_STR( SETTING_NAME_ALS_PRESET,          "preset" );
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
PROG_STR( SETTING_NAME_HOSTNAME,            "hostname" );
PROG_STR( SETTING_NAME_NTPSERVER,           "ntpserver" );
PROG_STR( SETTING_NAME_TELNET_ENABLED,      "telnet" );
PROG_STR( SETTING_NAME_WKEY,                "passphrase" );
PROG_STR( SETTING_NAME_ENABLED,             "enabled" );
PROG_STR( SETTING_NAME_SNOOZE,              "snooze" );
PROG_STR( SETTING_NAME_LAMP_COLOR,          "lamp-color" );
PROG_STR( SETTING_NAME_LAMP_MODE,           "lamp-mode" );
PROG_STR( SETTING_NAME_LAMP_SPEED,          "lamp-speed" );
PROG_STR( SETTING_NAME_LAMP_BRIGHTNESS,     "lamp-brightness" );
PROG_STR( SETTING_NAME_MQTT_HOST,           "host" );
PROG_STR( SETTING_NAME_MQTT_USERNAME,       "username" );
PROG_STR( SETTING_NAME_MQTT_PASSWORD,       "password" );
PROG_STR( SETTING_NAME_MQTT_PORT,           "port");
PROG_STR( SETTING_NAME_HA_DISCOVERY_PREFIX, "discovery-prefix");

/* Settings ID's */
enum {

    /* Clock section */
    SETTING_ID_CLOCK_24H = 1,
    SETTING_ID_CLOCK_COLOR,
    SETTING_ID_CLOCK_BRIGHTNESS,
    SETTING_ID_CLOCK_NTP,
    SETTING_ID_TIMEZONE,

    /* ALS section */
    SETTING_ID_ALS_PRESET, 
    SETTING_ID_LCD_DATEFMT,

    /* Begin LCD section */
    SETTING_ID_LCD_CONTRAST,
    SETTING_ID_LAMP_COLOR,
    SETTING_ID_LAMP_BRIGHTNESS,
    SETTING_ID_LAMP_DELAY,

    /* Network section */
    SETTING_ID_NETWORK_DHCP,
    SETTING_ID_NETWORK_IP,
    SETTING_ID_NETWORK_MASK,
    SETTING_ID_NETWORK_GATEWAY,
    SETTING_ID_NETWORK_DNS,
    SETTING_ID_NETWORK_SSID,
    SETTING_ID_NETWORK_HOSTNAME,
    SETTING_ID_NETWORK_WKEY,
    SETTING_ID_NETWORK_NTPSERVER,
    SETTING_ID_NETWORK_TELNET_ENABLED,

    /* MQTT client section */
    SETTING_ID_MQTT_ENABLED,
    SETTING_ID_MQTT_HOST,
    SETTING_ID_MQTT_PORT,
    SETTING_ID_MQTT_USERNAME,
    SETTING_ID_MQTT_PASSWORD,

    /* Home assistant section */
    SETTING_ID_HA_DISCOVERY_PREFIX,

    /* Alarm section */
    SETTING_ID_ALARM_ENABLED,
    SETTING_ID_ALARM_FILENAME,
    SETTING_ID_ALARM_TIME,
    SETTING_ID_ALARM_SNOOZE,
    SETTING_ID_ALARM_VOLUME,
    SETTING_ID_ALARM_GRADUAL,
    SETTING_ID_ALARM_DOW,
    SETTING_ID_ALARM_MESSAGE,
    SETTING_ID_ALARM_VISUAL,
    SETTING_ID_ALARM_VISUAL_SPEED,
    SETTING_ID_ALARM_LAMP_MODE,
    SETTING_ID_ALARM_LAMP_SPEED,
    SETTING_ID_ALARM_LAMP_COLOR,
    SETTING_ID_ALARM_LAMP_BRIGHTNESS
};

#define SETTING_ID_ALARM_BEGIN              SETTING_ID_ALARM_ENABLED  
#define SETTING_ID_END                      SETTING_ID_ALARM_LAMP_BRIGHTNESS


/* Settings file section ID's */
enum {
    SECTION_ID_UNKNOWN = 0,
    SECTION_ID_ANY = 0,
    SECTION_ID_CLOCK,
    SECTION_ID_ALS,
    SECTION_ID_LCD,
    SECTION_ID_LAMP,
    SECTION_ID_NETWORK,
    SECTION_ID_ALARM,
    SECTION_ID_MQTT,
    SECTION_ID_HA,
};

/* Settings parser token types */
enum {
    TOKEN_NAME = 0,
    TOKEN_VALUE,
    TOKEN_WHITESPACE,
    TOKEN_COMMENT,
    TOKEN_LIST,
};

/* Tasks ID's */
enum {
    TASK_CONFIG_BACKUP = 1,
    TASK_CONFIG_RESTORE
};

/* Night lamp settings */
struct NightLampSettings {
    uint8_t color;
    uint8_t brightness;
    uint8_t delay_off;
    uint8_t speed;
    uint8_t mode;
};

/* Alarm profile settings */
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

/* Clock settings */
struct ClockSettings {
    bool use_ntp = false;
    char timezone[ MAX_TZ_NAME_LENGTH + 1];
    bool display_24h = false;
    bool alarm_on[2] = { false, false };

    uint8_t clock_color = 1;
    uint8_t clock_brightness = 40;
    uint8_t lcd_contrast = 50;
    uint8_t date_format = 0;
    uint8_t als_preset = 0;

    struct NightLampSettings lamp;
};

/* Network settings */
struct NetworkSettings {
    bool dhcp = true;
    uint8_t ip[4] = { 0, 0, 0, 0 };
    uint8_t mask[4] = { 255, 255, 255, 0 };
    uint8_t gateway[4] = { 0, 0, 0, 0 };
    uint8_t dns[4] = { 0, 0, 0, 0 };

    char ssid[ MAX_SSID_LENGTH + 1 ];
    char wkey[ MAX_WKEY_LENGTH + 1 ];
    char hostname[ MAX_HOSTNAME_LENGTH + 1 ];
    char ntpserver[ MAX_NTPSERVER_LENGTH + 1];
    bool telnetEnabled = false;

    bool mqtt_enabled = false;
    char mqtt_host[ MAX_MQTT_HOST_LENGTH + 1];
    char mqtt_username[ MAX_MQTT_USERNAME_LENGTH + 1];
    char mqtt_password[ MAX_MQTT_PASSWORD_LENGTH + 1];
    uint16_t mqtt_port = 1883;
    char discovery_prefix[ MAX_DISCOVERY_PREFIX_LENGTH ];
};


/*******************************************************************************
 *
 * @brief   Configuration manager
 * 
 *******************************************************************************/
class ConfigManager : public ITask {

  public:
    void reset();
    void load( uint8_t section = EEPROM_SECTION_ALL );
    void save( uint8_t section = EEPROM_SECTION_ALL );
    void apply( uint8_t section = EEPROM_SECTION_ALL );
    bool isEepromValid();
    void formatEeprom();
    bool startRestore( const char *filename );
    bool startBackup( const char *filename, bool overwrite = true );
    void endBackup( int error = TASK_SUCCESS );
    void endRestore( int error = TASK_SUCCESS );
    void runTasks();

    ClockSettings clock;        /* Clock settings */
    NetworkSettings network;    /* Network settings */


  private:
    bool writeNextLine();
    void writeConfigLine( const char* name, uint8_t type, void* value );
    bool readNextLine();
    uint8_t parseConfigLine( char* name, char* value );
    void parseSettingValue( char* src, void* dest, uint8_t type, uint8_t min = 0, uint8_t max = 255 );
    bool matchSettingName( char* testName, const char* name, uint8_t section );

    uint8_t _currentSettingID = 0;      /* Current settings ID read or written to config file */
    uint8_t _currentSectionID = 0;      /* Current section ID read or written to config file */
    int8_t _currentAlarmID = -1;        /* Current alarm profile read or written to config file */
    FatFile _sd_file;                   /* SD file manager instance */
};

extern ConfigManager g_config;

#endif /* CONFIG_H */