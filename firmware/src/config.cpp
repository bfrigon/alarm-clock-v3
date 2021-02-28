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

#include <SdFat.h>
#include <tzdata.h>
#include <config.h>
#include <task_errors.h>

#include "alarm.h"
#include "ui/screen.h"
#include "drivers/lamp.h"
#include "drivers/neoclock.h"
#include "services/ntpclient.h"
#include "services/telnet_console.h"




/*! ------------------------------------------------------------------------
 *
 * @brief   Load settings from EEPROM
 *
 * @param   section    Which block of the EEPROM configuration to load
 * 
 */
void ConfigManager::load( uint8_t section ) {
    uint8_t c;
    uint8_t byte;

    /* Checks if EEPROM starts with the magic code 'BEEF'. If not, assumes
       the config is not present or corrupted and restore the default
       settings */
    if( this->isEepromValid() == false ) {
        this->reset();
        return;
    }

    if( section & EEPROM_SECTION_CLOCK ) {

        for( c = 0; c < sizeof( ClockSettings ); c++ ) {
            byte = EEPROM.read( EEPROM_ADDR_CLOCK_CONFIG + c );

            *( ( ( uint8_t* )&this->clock ) + c ) = byte;
        }

        this->clock.lamp.mode = LAMP_MODE_OFF;
    }

    if( section & EEPROM_SECTION_NETWORK ) {

        for( c = 0; c < sizeof( NetworkSettings ); c++ ) {
            byte = EEPROM.read( EEPROM_ADDR_NETWORK_CONFIG + c );

            *( ( ( uint8_t* )&this->network ) + c ) = byte;
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Save settings to EEPROM
 * 
 * @param   section    Which block of the EEPROM configuration to load
 *
 */
void ConfigManager::save( uint8_t section ) {
    uint8_t c;
    uint8_t byte;

    /* Save valid config magic number (0xBEEF) */
    EEPROM.update( EEPROM_ADDR_MAGIC + 0, 0xEF );
    EEPROM.update( EEPROM_ADDR_MAGIC + 1, 0xBE );

    if( section & EEPROM_SECTION_CLOCK ) {

        for( c = 0; c < sizeof( ClockSettings ); c++ ) {
            byte = *((( uint8_t* )&this->clock ) + c );

            EEPROM.update( EEPROM_ADDR_CLOCK_CONFIG + c, byte );
        }
    }

    if( section & EEPROM_SECTION_NETWORK ) {

        for( c = 0; c < sizeof( NetworkSettings ); c++ ) {
            byte = *((( uint8_t* )&this->network ) + c );

            EEPROM.update( EEPROM_ADDR_NETWORK_CONFIG + c, byte );
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Apply the settings to the different modules.
 * 
 * @param   section    Which block of the EEPROM configuration to load
 *
 */
void ConfigManager::apply( uint8_t section ) {

    if( section & EEPROM_SECTION_CLOCK ) {

        g_clock.setColorFromTable( this->clock.clock_color );
        g_clock.setBrightness( this->clock.clock_brightness );
        g_lcd.setContrast( this->clock.lcd_contrast );

        g_timezone.setTimezoneByName( g_config.clock.timezone );

        g_clock.requestClockUpdate();

        g_ntp.setAutoSync( g_config.clock.use_ntp );

        g_screen.requestScreenUpdate( false );
    }

    if( section & EEPROM_SECTION_NETWORK ) {

        g_wifi.setAutoReconnect( true, true );
        g_wifi.disconnect();

        g_telnetConsole.enableServer( g_config.network.telnetEnabled );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Restore default settings or initialize EEPROM contents.
 *
 */
void ConfigManager::reset() {
    this->network.ssid[0] = 0;
    this->network.wkey[0] = 0;
    this->network.telnetEnabled = false;

    this->clock.lamp.brightness = 60;
    this->clock.lamp.mode = LAMP_MODE_OFF;
    this->clock.lamp.color = COLOR_WHITE;
    this->clock.use_ntp = true;

    strcpy_P( this->clock.timezone, TZ_ETC_UTC );
    strcpy_P( this->network.hostname, S_DEFAULT_HOSTNAME );
    strcpy_P( this->network.ntpserver, S_DEFAULT_NTPSERVER );


    /* Store default config */
    this->save();


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

    for( uint8_t i = 0; i < MAX_NUM_PROFILES; i++ ) {

        g_alarm.saveProfile( &profile, i );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Look for the magic code at the start of EEPROM to determine if 
 *          the config is present
 *
 * @return  TRUE if config found, FALSE otherwise.
 * 
 */
bool ConfigManager::isEepromValid() {

    uint16_t magic;
    EEPROM.get( EEPROM_ADDR_MAGIC, magic );

    if( magic != 0xBEEF ) {
        return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the config erase flag in the EEPROM configuration block.
 * 
 * @details Destroy the configuration stored in EEPROM by writing the magic code 
 *          0xDEAD at the start of EEPROM configuration block. On the next reboot, 
 *          the default configuration will be restored.
 * 
 */
void ConfigManager::formatEeprom() {

    int16_t magic = 0xDEAD;
    EEPROM.put( EEPROM_ADDR_MAGIC, magic );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run tasks for the configuration manager
 * 
 */
void ConfigManager::runTasks() {

    switch( this->getCurrentTask() ) {

        case TASK_CONFIG_BACKUP:
            if( this->writeNextLine() == false ) {
                this->endBackup();
            }

            break;

        case TASK_CONFIG_RESTORE:
            if( this->readNextLine() == false ) {
                this->endRestore();
            }

            break;

        default:

            /* Nothing to do here */
            return;
    }

}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the configuration backup task.
 *
 * @param   overwrite    TRUE to allow overwriting file if already present, 
 *                       FALSE to fail if file exists.
 *
 * @return  TRUE if successfuly started, FALSE otherwise.
 * 
 */
bool ConfigManager::startBackup( const char *filename, bool overwrite ) {

    /* Checks if another task is already running. */
    if( this->startTask( TASK_CONFIG_BACKUP ) != TASK_CONFIG_BACKUP ) {
        return false;
    }

    if( g_alarm.isSDCardPresent() == false ) {
        this->endBackup( ERR_CONFIG_NO_SDCARD );
        return false;
    }

    _currentSectionID = SECTION_ID_UNKNOWN;
    _currentSettingID = 0;
    _currentAlarmID = 0;

    if( _sd_file.isOpen() == true ) {
        _sd_file.close();
    }

    if( _sd_file.openCwd() == false ) {
        this->endBackup( ERR_CONFIG_FILE_CANT_OPEN );
        return false;
    }

    if( overwrite == false && _sd_file.exists( filename ) == true ) {
        this->endBackup( ERR_CONFIG_FILE_EXISTS );
        return false;
    }

    _sd_file.close();

    uint8_t flags;
    flags = O_CREAT | O_WRITE | ( overwrite == true ? O_TRUNC : O_EXCL );

    if( _sd_file.open( filename, flags ) == false ) {
        this->endBackup( ERR_CONFIG_FILE_CANT_OPEN );
        return false;
    }

    /* Write file header */
    this->writeConfigLine( NULL, SETTING_TYPE_COMMENT, ( void* )COMMENT_FILE_HEADER );
    _sd_file.sync();

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Stops the configuration backup task.
 *
 * @param   error    Task result
 * 
 */
void ConfigManager::endBackup( int error ) {
    if( _sd_file.isOpen() == true ) {
        _sd_file.close();
    }

    this->endTask( error );
    g_screen.requestScreenUpdate( false );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the configuration restore task.
 *
 * @return  TRUE if successfuly started, FALSE otherwise.
 * 
 */
bool ConfigManager::startRestore( const char *filename) {
    if( this->startTask( TASK_CONFIG_RESTORE ) != TASK_CONFIG_RESTORE ) {
        return false;
    }

    if( g_alarm.isSDCardPresent() == false ) {
        this->endBackup( ERR_CONFIG_NO_SDCARD );
        return false;
    }

    _currentSectionID = SECTION_ID_UNKNOWN;
    _currentSettingID = 0;
    _currentAlarmID = -1;

    if( _sd_file.open( filename, O_READ ) == false ) {
        this->endBackup( ERR_CONFIG_FILE_NOT_FOUND );
        return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Stops the configuration restore task.
 *
 * @param   error    Task result
 * 
 */
void ConfigManager::endRestore( int error ) {
    if( _sd_file.isOpen() == true ) {
        _sd_file.close();
    }

    this->endTask( error );
    g_screen.requestScreenUpdate( false );

    if( error == TASK_SUCCESS ) {

        g_alarm.saveProfile( _currentAlarmID );
        this->save( EEPROM_SECTION_ALL );

        this->apply( EEPROM_SECTION_ALL );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reads the next line in the backup file and set the corresponding 
 *          setting value.
 *
 * @return  TRUE if successful, FALSE otherwise
 * 
 */
bool ConfigManager::readNextLine() {

    if( _sd_file.peek() == -1 ) {
        /* Reached EOF or error */
        return false;
    }

    char name[ MAX_LENGTH_SETTING_NAME + 1 ];
    char value[ MAX_LENGTH_SETTING_VALUE + 1 ];

    /* Read the next line */
    uint8_t type = this->parseConfigLine( name, value );

    if( _sd_file.getError() != 0 ) {

        this->setTaskError( ERR_CONFIG_FILE_READ );
        return false;
    }

    /* Ignore line if empty */
    if( strnlen( name, 1 ) == 0 ) {
        return true;
    }

    /* If setting is a section header, find out the section ID from the name */
    if( type == SETTING_TYPE_SECTION ) {

        if( strcmp_P( name, SETTING_NAME_SECTION_CLOCK ) == 0 ) {
            _currentSectionID = SECTION_ID_CLOCK;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_ALS ) == 0 ) {
            _currentSectionID = SECTION_ID_ALS;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_LAMP ) == 0 ) {
            _currentSectionID = SECTION_ID_LAMP;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_LCD ) == 0 ) {
            _currentSectionID = SECTION_ID_LCD;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_NETWORK ) == 0 ) {
            _currentSectionID = SECTION_ID_NETWORK;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_ALARM ) == 0 ) {


            if( _currentAlarmID >= 0 ) {
                g_alarm.saveProfile( _currentAlarmID );
            }

            uint8_t alarmID = atoi( value );

            if( alarmID < MAX_NUM_PROFILES ) {

                g_alarm.loadProfile( alarmID );

                _currentAlarmID = alarmID;
                _currentSectionID = SECTION_ID_ALARM;

            } else {

                _currentAlarmID = -1;
                _currentSectionID = SECTION_ID_UNKNOWN;

            }

        } else {
            _currentSectionID = SECTION_ID_UNKNOWN;
        }

        return true;
    }


    if( this->matchSettingName( name, SETTING_NAME_24H, SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->clock.display_24h, SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_COLOR,  SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->clock.clock_color, SETTING_TYPE_INTEGER, 0, COLOR_TABLE_MAX_COLORS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_BRIGHTNESS, SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->clock.clock_brightness, SETTING_TYPE_INTEGER,
                                 MIN_CLOCK_BRIGHTNESS, MAX_CLOCK_BRIGHTNESS );

    } else if( this->matchSettingName( name, SETTING_NAME_DATEFMT, SECTION_ID_LCD ) == true ) {
        this->parseSettingValue( value, &this->clock.date_format, SETTING_TYPE_INTEGER, 0, MAX_DATE_FORMATS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_USE_NTP, SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->clock.use_ntp, SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_TIMEZONE, SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->clock.timezone, SETTING_TYPE_STRING, 0, MAX_TZ_NAME_LENGTH );

    } else if( this->matchSettingName( name, SETTING_NAME_ALS_PRESET, SECTION_ID_ALS ) == true ) {
        this->parseSettingValue( value, &this->clock.als_preset, SETTING_TYPE_INTEGER, 0, MAX_ALS_PRESETS_NAMES - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_CONTRAST, SECTION_ID_LCD ) == true ) {
        this->parseSettingValue( value, &this->clock.lcd_contrast, SETTING_TYPE_INTEGER,
                                 MIN_LCD_CONTRAST, MAX_LCD_CONTRAST );

    } else if( this->matchSettingName( name, SETTING_NAME_COLOR,  SECTION_ID_LAMP ) == true ) {
        this->parseSettingValue( value, &this->clock.lamp.color, SETTING_TYPE_INTEGER, 0, COLOR_TABLE_MAX_COLORS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_BRIGHTNESS, SECTION_ID_LAMP ) == true ) {
        this->parseSettingValue( value, &this->clock.lamp.brightness, SETTING_TYPE_INTEGER, MIN_LAMP_BRIGHTNESS,
                                 MAX_LAMP_BRIGHTNESS );

    } else if( this->matchSettingName( name, SETTING_NAME_DELAY, SECTION_ID_LAMP ) == true ) {
        this->parseSettingValue( value, &this->clock.lamp.delay_off, SETTING_TYPE_INTEGER,
                                 MIN_LAMP_DELAY_OFF, MAX_LAMP_DELAY_OFF );

    } else if( this->matchSettingName( name, SETTING_NAME_DHCP, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.dhcp, SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_ADDRESS, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.ip, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_MASK, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.mask, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_GATEWAY, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.gateway, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_DNS, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.dns, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_HOSTNAME, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.hostname, SETTING_TYPE_STRING, 0, MAX_HOSTNAME_LENGTH );

    } else if( this->matchSettingName( name, SETTING_NAME_NTPSERVER, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.ntpserver, SETTING_TYPE_STRING, 0, MAX_NTPSERVER_LENGTH );

    } else if( this->matchSettingName( name, SETTING_NAME_TELNET_ENABLED, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.telnetEnabled, SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_SSID, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.ssid, SETTING_TYPE_STRING, 0, MAX_SSID_LENGTH );

    } else if( this->matchSettingName( name, SETTING_NAME_WKEY, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->network.wkey, SETTING_TYPE_STRING, 0, MAX_WKEY_LENGTH );

    } else if( this->matchSettingName( name, SETTING_NAME_ENABLED, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &this->clock.alarm_on[ _currentAlarmID ], SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_FILENAME, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.filename, SETTING_TYPE_STRING, 0, MAX_LENGTH_ALARM_FILENAME );
        strupr( &g_alarm.profile.filename[0] );

    } else if( this->matchSettingName( name, SETTING_NAME_TIME, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.time, SETTING_TYPE_TIME );

    } else if( this->matchSettingName( name, SETTING_NAME_SNOOZE, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.snoozeDelay, SETTING_TYPE_INTEGER,
                                 MIN_ALARM_SNOOZE_TIME, MAX_ALARM_SNOOZE_TIME );

    } else if( this->matchSettingName( name, SETTING_NAME_VOLUME, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.volume, SETTING_TYPE_INTEGER,
                                 MIN_ALARM_VOLUME, MAX_ALARM_VOLUME );

    } else if( this->matchSettingName( name, SETTING_NAME_GRADUAL, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.gradual, SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_DOW, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.dow, SETTING_TYPE_DOW );

    } else if( this->matchSettingName( name, SETTING_NAME_MESSAGE, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.message, SETTING_TYPE_STRING, 0, MAX_LENGTH_ALARM_MESSAGE );

    } else if( this->matchSettingName( name, SETTING_NAME_VISUAL_MODE, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.visualMode, SETTING_TYPE_INTEGER,
                                 0, MAX_ALARM_VISUALS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_VISUAL_SPEED, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.effectSpeed, SETTING_TYPE_INTEGER,
                                 MIN_ALARM_VISUAL_EFFECT_SPEED, MAX_ALARM_VISUAL_EFFECT_SPEED );

    } else if( this->matchSettingName( name, SETTING_NAME_LAMP_MODE, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.lamp.mode, SETTING_TYPE_INTEGER,
                                 0, MAX_ALARM_LAMP_MODES - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_LAMP_COLOR, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.lamp.color, SETTING_TYPE_INTEGER,
                                 0, COLOR_TABLE_MAX_COLORS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_LAMP_BRIGHTNESS, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &g_alarm.profile.lamp.brightness, SETTING_TYPE_INTEGER,
                                 MIN_ALARM_LAMP_BRIGHTNESS, MAX_ALARM_LAMP_BRIGHTNESS );
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Checks if the settings name on the current line in the config 
 *          file matches the provided name. 
 * 
 * @details Also checks if the settings belongs in to specified section.
 *
 * @param   currentName    Setting's name parsed from the current line in 
 *                         the backup file.
 * @param   name           Setting's name to match against.
 * @param   section        Section ID to match against. Use SECTION_ID_ANY to match
 *                         regardless of current section.
 *
 * @return  TRUE if matching, FALSE otherwise.
 * 
 */
inline bool ConfigManager::matchSettingName( char* currentName, const char* name, uint8_t section ) {
    return ( strcmp_P( currentName, name ) == 0 && ( _currentSectionID == section || section == SECTION_ID_ANY ) );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Converts the user readable format from the backup file and copy 
 *          it to the specified memory location.
 *
 * @param   src     Value string from the backup file
 * @param   dest    Pointer to the memory location where the setting
 *                  must be copied to.
 * @param   type    Variable type to parse
 * @param   min     Minimum value.
 * @param   max     Maximum value. If the setting is a STRING type, it defines
 *                  the maximum characters to be copied.
 * 
 */
void ConfigManager::parseSettingValue( char* src, void* dest, uint8_t settingType, uint8_t min,
                                       uint8_t max ) {

    IPAddress addr;
    Time time;

    switch( settingType ) {

        case SETTING_TYPE_BOOL:
            if( strcasecmp( src, SETTING_VALUE_TRUE ) == 0 ) {
                *( ( bool* )dest ) = true;

            } else if( strcasecmp( src, SETTING_VALUE_FALSE ) == 0 ) {
                *( ( bool* )dest ) = false;

            } else {
                *( ( bool* )dest ) = atoi( src );
            }

            break;

        case SETTING_TYPE_INTEGER:
            *( ( uint8_t* )dest ) = constrain( atoi( src ), min, max );
            break;

        case SETTING_TYPE_SHORT:
            *( ( uint16_t* )dest ) = constrain( atoi( src ), min, max );
            break;

        case SETTING_TYPE_STRING:
            strncpy( ( char* )dest, src, max );
            break;

        case SETTING_TYPE_IP:
            addr.fromString( src );

            for( uint8_t i = 0; i < 4; i++ ) {
                *( ( uint8_t* )dest + i ) = addr[i];
            }

            break;

        case SETTING_TYPE_TIME:
            bool isHours;

            isHours = true;
            time.hour = 0;
            time.minute = 0;

            while( *src != 0 ) {
                char chr = *src++;


                if( chr >= '0' && chr <= '9' ) {

                    if( isHours == true ) {
                        time.hour = time.hour * 10 + ( chr - '0' );

                        if( time.hour > 23 ) {
                            time.hour = 0;
                            break;
                        }

                    } else  {
                        time.minute = time.minute * 10 + ( chr - '0' );

                        if( time.minute > 59 ) {
                            time.minute = 0;
                            break;
                        }
                    }

                } else if( chr == ':' || chr == 'h' ) {

                    if( isHours == false ) {
                        break;
                    }

                    isHours = false;
                }
            }

            memcpy( dest, &time, sizeof( Time ) );
            break;

        case SETTING_TYPE_DOW:
            *( ( uint8_t* )dest ) = 0;

            uint8_t i;

            for( i = 0; i < 7; i++ ) {
                if( strcasestr_P( src, getDayName( i + 1, true ) ) != NULL ) {
                    *( ( uint8_t* )dest ) |= ( 1 <<  i );
                }
            }

            break;

    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Parse the current line in the backup file.
 *
 * @param   name     Pointer to the buffer where to copy the setting's name.
 * @param   value    Pointer to the buffer where to copy the setting's value.
 *
 * @return  The type of setting parsed.
 * 
 */
uint8_t ConfigManager::parseConfigLine( char* name, char* value ) {
    uint8_t chr;
    uint8_t nameLength = 0;
    uint8_t valueLength = 0;
    uint8_t type = SETTING_TYPE_UNKNOWN;
    bool insideQuote = false;
    uint8_t token = TOKEN_NAME;

    /* Reset the name and value buffer */
    name[0] = 0;
    value[0] = 0;

    while( _sd_file.read( &chr, 1 ) == 1 ) {

        if( chr == '\r' || chr == '\n' ) {

            int nextChar = _sd_file.peek();

            /* Consumes the next character if it is a CR or LF. */
            if( nextChar == '\r' || nextChar == '\n' ) {
                _sd_file.read();
            }

            /* End of line */
            break;

        } else if( insideQuote == false && chr == ';' ) {

            token = TOKEN_COMMENT;

        } else if( isspace( chr ) && insideQuote == false ) {

            if( token == TOKEN_NAME && nameLength > 0 ) {
                token = TOKEN_WHITESPACE;
            }

            if( token == TOKEN_VALUE && valueLength > 0 ) {
                token = TOKEN_WHITESPACE;
            }

        } else if( ( token == TOKEN_NAME || token == TOKEN_VALUE ) && chr == '[' ) {
            if( token == TOKEN_NAME ) {
                type = SETTING_TYPE_SECTION;

            } else {
                token = TOKEN_LIST;
                insideQuote = true;
            }


        } else if( ( token == TOKEN_NAME || token == TOKEN_LIST ) && chr == ']' ) {
            token = TOKEN_WHITESPACE;

        } else if( ( token == TOKEN_NAME || token == TOKEN_WHITESPACE ) && chr == ':' ) {
            token = TOKEN_VALUE;

        } else if( token == TOKEN_VALUE && chr == '"' ) {

            type = SETTING_TYPE_STRING;

            if( insideQuote == true && valueLength > 0 && value[ valueLength - 1 ] == '\\' ) {

                /* Include the quote if it is preceded by an escape character. */
                value[ valueLength - 1 ] = '"';

            } else if( insideQuote == false ) {
                insideQuote = true;

            } else {
                /* Ignore everything after the quote is closed. */
                token = TOKEN_WHITESPACE;
            }

        } else if( token == TOKEN_NAME ) {
            name[ nameLength++ ] = tolower( chr );

        } else if( token == TOKEN_VALUE || token == TOKEN_LIST ) {

            value[ valueLength++ ] = ( insideQuote == false ? tolower( chr ) :  chr );
        }

    }

    /* Add NULL character */
    if( name[ nameLength ] != 0 )   { name[ nameLength++ ] = 0; }
    if( value[ valueLength ] != 0 ) { value[ valueLength++ ] = 0; }

    return type;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Write the next setting line to the backup file.
 *
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool ConfigManager::writeNextLine()  {

    _currentSettingID++;

    if( _currentSettingID == SETTING_ID_ALARM_BEGIN ) {
        g_alarm.loadProfile( this ->_currentAlarmID );
    }

    switch( _currentSettingID ) {

        case SETTING_ID_CLOCK_24H:
            this->writeConfigLine( SETTING_NAME_SECTION_CLOCK, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_24H, SETTING_TYPE_BOOL, &this->clock.display_24h );
            break;

        case SETTING_ID_CLOCK_COLOR:
            this->writeConfigLine( SETTING_NAME_COLOR, SETTING_TYPE_INTEGER, &this->clock.clock_color );
            break;

        case SETTING_ID_CLOCK_BRIGHTNESS:
            this->writeConfigLine( SETTING_NAME_BRIGHTNESS, SETTING_TYPE_INTEGER, &this->clock.clock_brightness );
            break;

        case SETTING_ID_CLOCK_NTP:
            this->writeConfigLine( SETTING_NAME_USE_NTP, SETTING_TYPE_BOOL, &this->clock.use_ntp );
            break;

        case SETTING_ID_TIMEZONE:
            this->writeConfigLine( SETTING_NAME_TIMEZONE, SETTING_TYPE_STRING, &this->clock.timezone );
            break;

        case SETTING_ID_ALS_PRESET:
            this->writeConfigLine( SETTING_NAME_SECTION_ALS, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_ALS_PRESET, SETTING_TYPE_INTEGER, &this->clock.als_preset );
            break;

        case SETTING_ID_LCD_DATEFMT:
            this->writeConfigLine( SETTING_NAME_SECTION_LCD, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_DATEFMT, SETTING_TYPE_INTEGER, &this->clock.date_format );
            break;

        case SETTING_ID_LCD_CONTRAST:
            this->writeConfigLine( SETTING_NAME_CONTRAST, SETTING_TYPE_INTEGER, &this->clock.lcd_contrast );
            break;

        case SETTING_ID_LAMP_COLOR:
            this->writeConfigLine( SETTING_NAME_SECTION_LAMP, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_COLOR, SETTING_TYPE_INTEGER, &this->clock.lamp.color );
            break;

        case SETTING_ID_LAMP_BRIGHTNESS:
            this->writeConfigLine( SETTING_NAME_BRIGHTNESS, SETTING_TYPE_INTEGER, &this->clock.lamp.brightness );
            break;

        case SETTING_ID_LAMP_DELAY:
            this->writeConfigLine( SETTING_NAME_DELAY, SETTING_TYPE_INTEGER, &this->clock.lamp.delay_off );
            break;

        case SETTING_ID_NETWORK_DHCP:
            this->writeConfigLine( SETTING_NAME_SECTION_NETWORK, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_DHCP, SETTING_TYPE_BOOL, &this->network.dhcp );
            break;

        case SETTING_ID_NETWORK_IP:
            this->writeConfigLine( SETTING_NAME_ADDRESS, SETTING_TYPE_IP, &this->network.ip );
            break;

        case SETTING_ID_NETWORK_MASK:
            this->writeConfigLine( SETTING_NAME_MASK, SETTING_TYPE_IP, &this->network.mask );
            break;

        case SETTING_ID_NETWORK_GATEWAY:
            this->writeConfigLine( SETTING_NAME_GATEWAY, SETTING_TYPE_IP, &this->network.gateway );
            break;

        case SETTING_ID_NETWORK_DNS:
            this->writeConfigLine( SETTING_NAME_DNS, SETTING_TYPE_IP, &this->network.dns );
            break;

        case SETTING_ID_NETWORK_HOSTNAME:
            this->writeConfigLine( SETTING_NAME_HOSTNAME, SETTING_TYPE_STRING, &this->network.hostname );
            break;

        case SETTING_ID_NETWORK_NTPSERVER:
            this->writeConfigLine( SETTING_NAME_NTPSERVER, SETTING_TYPE_STRING, &this->network.ntpserver );
            break;

        case SETTING_ID_NETWORK_TELNET_ENABLED:
            this->writeConfigLine( SETTING_NAME_TELNET_ENABLED, SETTING_TYPE_BOOL, &this->network.telnetEnabled );
            break;

        case SETTING_ID_NETWORK_SSID:
            this->writeConfigLine( SETTING_NAME_SSID, SETTING_TYPE_STRING, &this->network.ssid );
            break;

        case SETTING_ID_NETWORK_WKEY:
            this->writeConfigLine( SETTING_NAME_WKEY, SETTING_TYPE_STRING, &this->network.wkey );
            break;

        case SETTING_ID_ALARM_ENABLED:
            this->writeConfigLine( SETTING_NAME_SECTION_ALARM, SETTING_TYPE_SECTION, &_currentAlarmID );
            this->writeConfigLine( SETTING_NAME_ENABLED, SETTING_TYPE_BOOL, &this->clock.alarm_on[ _currentAlarmID ] );
            break;

        case SETTING_ID_ALARM_FILENAME:
            this->writeConfigLine( SETTING_NAME_FILENAME, SETTING_TYPE_STRING, &g_alarm.profile.filename );
            break;

        case SETTING_ID_ALARM_TIME:
            this->writeConfigLine( SETTING_NAME_TIME, SETTING_TYPE_TIME, &g_alarm.profile.time );
            break;

        case SETTING_ID_ALARM_SNOOZE:
            this->writeConfigLine( SETTING_NAME_SNOOZE, SETTING_TYPE_INTEGER, &g_alarm.profile.snoozeDelay );
            break;

        case SETTING_ID_ALARM_VOLUME:
            this->writeConfigLine( SETTING_NAME_VOLUME, SETTING_TYPE_INTEGER, &g_alarm.profile.volume );
            break;

        case SETTING_ID_ALARM_GRADUAL:
            this->writeConfigLine( SETTING_NAME_GRADUAL, SETTING_TYPE_BOOL, &g_alarm.profile.gradual );
            break;

        case SETTING_ID_ALARM_DOW:
            this->writeConfigLine( SETTING_NAME_DOW, SETTING_TYPE_DOW, &g_alarm.profile.dow );
            break;

        case SETTING_ID_ALARM_MESSAGE:
            this->writeConfigLine( SETTING_NAME_MESSAGE, SETTING_TYPE_STRING, &g_alarm.profile.message );
            break;

        case SETTING_ID_ALARM_VISUAL:
            this->writeConfigLine( SETTING_NAME_VISUAL_MODE, SETTING_TYPE_INTEGER, &g_alarm.profile.visualMode );
            break;

        case SETTING_ID_ALARM_VISUAL_SPEED:
            this->writeConfigLine( SETTING_NAME_VISUAL_SPEED, SETTING_TYPE_INTEGER, &g_alarm.profile.effectSpeed );
            break;

        case SETTING_ID_ALARM_LAMP_MODE:
            this->writeConfigLine( SETTING_NAME_LAMP_MODE, SETTING_TYPE_INTEGER, &g_alarm.profile.lamp.mode );
            break;

        case SETTING_ID_ALARM_LAMP_SPEED:
            this->writeConfigLine( SETTING_NAME_LAMP_SPEED, SETTING_TYPE_INTEGER, &g_alarm.profile.lamp.color );
            break;

        case SETTING_ID_ALARM_LAMP_COLOR:
            this->writeConfigLine( SETTING_NAME_LAMP_COLOR, SETTING_TYPE_INTEGER, &g_alarm.profile.lamp.color );
            break;

        case SETTING_ID_ALARM_LAMP_BRIGHTNESS:
            this->writeConfigLine( SETTING_NAME_LAMP_BRIGHTNESS, SETTING_TYPE_INTEGER, &g_alarm.profile.lamp.brightness );
            break;
    }

    _sd_file.sync();

    if( _sd_file.getError() != 0 ) {
        this->setTaskError( ERR_CONFIG_FILE_WRITE );
        return false;
    }



    if( _currentSettingID >= SETTING_ID_END ) {

        _currentAlarmID++;

        if( _currentAlarmID < MAX_NUM_PROFILES ) {
            _currentSettingID = SETTING_ID_ALARM_BEGIN - 1;
            return true;
        }

        return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Converts the setting value from memory to an user readable format.
 *
 * @param   name     Name of the setting.
 * @param   type     Type of setting to convert.
 * @param   value    Settings value to convert.
 *
 */
void ConfigManager::writeConfigLine( const char* name, uint8_t type, void* value ) {

    char buffer[ MAX_LENGTH_SETTING_VALUE ];

    if( _sd_file.isOpen() == false ) {
        return;
    }

    if( type != SETTING_TYPE_COMMENT ) {

        if( type == SETTING_TYPE_SECTION ) {
            _sd_file.write( "\r\n[" );
        }

        strcpy_P( buffer, name );

        _sd_file.write( buffer );


        if( type == SETTING_TYPE_SECTION ) {
            _sd_file.write( "]" );

            if( value != NULL ) {
                _sd_file.write( ":" );
                type = SETTING_TYPE_INTEGER;
            }

        } else {
            _sd_file.write( ": " );
        }
    }

    switch( type ) {
        case SETTING_TYPE_BOOL:

            if( *( ( bool* )value ) == true ) {
                _sd_file.write( SETTING_VALUE_TRUE );

            } else {
                _sd_file.write( SETTING_VALUE_FALSE );
            }

            break;

        case SETTING_TYPE_STRING:
            _sd_file.write( '"' );
            _sd_file.write( ( char* )value );
            _sd_file.write( '"' );
            break;

        case SETTING_TYPE_INTEGER:
            itoa( *( ( uint8_t* )value ), buffer, 10 );

            _sd_file.write( buffer );
            break;

        case SETTING_TYPE_SHORT:
            itoa( *( ( uint16_t* )value ), buffer, 10 );

            _sd_file.write( buffer );
            break;

        case SETTING_TYPE_IP:
            uint8_t ip[4];
            memcpy( &ip, value, sizeof( ip ) );

            sprintf( buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );

            _sd_file.write( buffer );
            break;

        case SETTING_TYPE_COMMENT:

            strncpy_P( buffer, ( const char* )value, MAX_LENGTH_SETTING_VALUE );
            _sd_file.write( buffer );
            break;

        case SETTING_TYPE_TIME:
            Time time;
            memcpy( &time, value, sizeof( Time ) );

            sprintf( buffer, "%02d:%02d", time.hour, time.minute );

            _sd_file.write( buffer );
            break;

        case SETTING_TYPE_DOW:
            uint8_t dow = *( ( uint8_t* )value );
            uint8_t n = 0;

            _sd_file.write( "[" );

            for( uint8_t i = 0; i < 7; i++ ) {

                if( dow & ( 1 << i ) ) {

                    if( n > 0 ) {
                        _sd_file.write( ", " );
                    }

                    strcpy_P( buffer, getDayName( i + 1, true ) );
                    _sd_file.write( buffer );
                    n++;
                }
            }

            _sd_file.write( "]" );
    }


    _sd_file.write( "\r\n" );
}