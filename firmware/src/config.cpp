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
#include "config.h"
#include "alarm.h"
#include "screen.h"
#include "drivers/lamp.h"
#include "drivers/neoclock.h"



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
void ConfigManager::load() {
    uint8_t c;
    uint8_t byte;

    /* Checks if EEPROM starts with the magic code 'BEEF'. If not, assumes
       the config is not present or corrupted and restore the default
       settings */
    if( this->isEepromValid() == false ) {
        this->reset();
        return;
    }

    for( c = 0; c < sizeof( GlobalSettings ); c++ ) {
        byte = EEPROM.read( EEPROM_ADDR_CONFIG + c );

        *( ( ( uint8_t* )&this->settings ) + c ) = byte;
    }


    this->settings.lamp.mode = LAMP_MODE_OFF;
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
void ConfigManager::save() {
    uint8_t c;
    uint8_t byte;

    /* Save valid config magic number (0xBEEF) */
    EEPROM.update( EEPROM_ADDR_MAGIC + 0, 0xEF );
    EEPROM.update( EEPROM_ADDR_MAGIC + 1, 0xBE );

    for( c = 0; c < sizeof( GlobalSettings ); c++ ) {
        byte = *( ( ( uint8_t* )&this->settings ) + c );

        EEPROM.update( EEPROM_ADDR_CONFIG + c, byte );
    }

    Serial.println( "Written EEPROM" );
}


/*--------------------------------------------------------------------------
 *
 * Apply the settings to the different modules.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
void ConfigManager::apply() {

    g_clock.setColorFromTable( this->settings.clock_color );
    g_clock.setBrightness( this->settings.clock_brightness );
    g_lcd.setContrast( this->settings.lcd_contrast );

    g_clockUpdate = true;
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
void ConfigManager::reset() {
    this->settings.ssid[0] = 0;
    this->settings.wkey[0] = 0;
    this->settings.lamp.brightness = 60;
    this->settings.lamp.mode = LAMP_MODE_OFF;
    this->settings.lamp.color = COLOR_WHITE;


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


/*--------------------------------------------------------------------------
 *
 * Look for the magic code at the start of EEPROM to determine if the
 * config is present
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if config found or FALSE otherwise.
 */
bool ConfigManager::isEepromValid() {

    uint16_t magic;
    EEPROM.get( EEPROM_ADDR_MAGIC, magic );

    if( magic != 0xBEEF ) {
        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Destroy the configuration by writing the magic code 0xDEAD at the start
 * of EEPROM. On the next reboot, the default configuration will be restored.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void ConfigManager::formatEeprom() {

    int16_t magic = 0xDEAD;
    EEPROM.put( EEPROM_ADDR_MAGIC, magic );
}


/*--------------------------------------------------------------------------
 *
 * Run tasks for the configuration manager
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void ConfigManager::runTask() {

    switch( this->getCurrentTask() ) {

        case TASK_BACKUP_CONFIG:
            if( this->writeNextLine() == false ) {
                this->endBackup();
            }

            break;

        case TASK_RESTORE_CONFIG:
            if( this->readNextLine() == false ) {
                this->endRestore();
            }

            break;

        default:

            /* Nothing to do here */
            return;
    }

}


/*--------------------------------------------------------------------------
 *
 * Starts the configuration backup task.
 *
 * Arguments
 * ---------
 *  - overwrite : TRUE to overwrite backup file if already present, FALSE will
 *                fail if file exists.
 *
 * Returns : TRUE if successfuly started or FALSE otherwise.
 */
bool ConfigManager::startBackup( bool overwrite ) {

    /* Checks if another task is already running. */
    if( this->startTask( TASK_BACKUP_CONFIG ) != TASK_BACKUP_CONFIG ) {
        return false;
    }

    if( g_alarm.isSDCardPresent() == false ) {
        this->endBackup( TASK_ERROR_NO_SDCARD );
        return false;
    }

    this->_currentSectionID = SECTION_ID_UNKNOWN;
    this->_currentSettingID = 0;
    this->_currentAlarmID = 0;

    if( this->_sd_file.isOpen() == true ) {
        this->_sd_file.close();
    }

    uint8_t flags;
    flags = O_CREAT | O_WRITE | ( overwrite == true ? O_TRUNC : O_EXCL );

    if( this->_sd_file.open( CONFIG_BACKUP_FILENAME, flags ) == false ) {
        this->endBackup( TASK_ERROR_CANT_OPEN );
        return false;
    }

    /* Write file header */
    this->writeConfigLine( NULL, SETTING_TYPE_COMMENT, ( void* )COMMENT_FILE_HEADER );
    this->_sd_file.sync();


    g_screenUpdate = true;

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Stops the configuration backup task.
 *
 * Arguments
 * ---------
 *  - error : Task result
 *
 * Returns : Nothing
 */
void ConfigManager::endBackup( int error ) {
    if( this->_sd_file.isOpen() == true ) {
        this->_sd_file.close();
    }

    this->endTask( error );
    g_screenUpdate = true;
}


/*--------------------------------------------------------------------------
 *
 * Starts the configuration restore task.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successfuly started or FALSE otherwise.
 */
bool ConfigManager::startRestore() {
    if( this->startTask( TASK_RESTORE_CONFIG ) != TASK_RESTORE_CONFIG ) {
        return false;
    }

    if( g_alarm.isSDCardPresent() == false ) {
        this->endBackup( TASK_ERROR_NO_SDCARD );
        return false;
    }

    this->_currentSectionID = SECTION_ID_UNKNOWN;
    this->_currentSettingID = 0;
    this->_currentAlarmID = -1;

    if( this->_sd_file.open( CONFIG_BACKUP_FILENAME, O_READ ) == false ) {
        this->endBackup( TASK_ERROR_NOT_FOUND );
        return false;
    }

    g_screenUpdate = true;
    return true;
}


/*--------------------------------------------------------------------------
 *
 * Stops the configuration restore task.
 *
 * Arguments
 * ---------
 *  - error : Task result
 *
 * Returns : Nothing
 */
void ConfigManager::endRestore( int error ) {
    if( this->_sd_file.isOpen() == true ) {
        this->_sd_file.close();
    }

    this->endTask( error );
    g_screenUpdate = true;

    if( error == TASK_SUCCESS ) {

        g_alarm.saveProfile( this->_currentAlarmID );
        this->save();

        this->apply();
    }
}


/*--------------------------------------------------------------------------
 *
 * Reads the next line in the backup file and set the corresponding setting
 * value.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful or FALSE otherwise
 */
bool ConfigManager::readNextLine() {

    if( this->_sd_file.peek() == -1 ) {
        /* Reached EOF or error */
        return false;
    }

    char name[ MAX_LENGTH_SETTING_NAME + 1 ];
    char value[ MAX_LENGTH_SETTING_VALUE + 1 ];

    /* Read the next line */
    uint8_t type = this->parseConfigLine( name, value );

    if( this->_sd_file.getError() != 0 ) {

        this->setTaskError( TASK_ERROR_READ );
        return false;
    }

    /* Ignore line if empty */
    if( strnlen( name, 1 ) == 0 ) {
        return true;
    }

    /* If setting is a section header, find out the section ID from the name */
    if( type == SETTING_TYPE_SECTION ) {

        if( strcmp_P( name, SETTING_NAME_SECTION_CLOCK ) == 0 ) {
            this->_currentSectionID = SECTION_ID_CLOCK;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_LAMP ) == 0 ) {
            this->_currentSectionID = SECTION_ID_LAMP;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_LCD ) == 0 ) {
            this->_currentSectionID = SECTION_ID_LCD;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_NETWORK ) == 0 ) {
            this->_currentSectionID = SECTION_ID_NETWORK;

        } else if( strcmp_P( name, SETTING_NAME_SECTION_ALARM ) == 0 ) {


            if( this->_currentAlarmID >= 0 ) {
                g_alarm.saveProfile( this->_currentAlarmID );
            }

            uint8_t alarmID = atoi( value );

            if( alarmID < MAX_NUM_PROFILES ) {

                g_alarm.loadProfile( alarmID );

                this->_currentAlarmID = alarmID;
                this->_currentSectionID = SECTION_ID_ALARM;

            } else {

                this->_currentAlarmID = -1;
                this->_currentSectionID = SECTION_ID_UNKNOWN;

            }

        } else {
            this->_currentSectionID = SECTION_ID_UNKNOWN;
        }

        return true;
    }


    if( this->matchSettingName( name, SETTING_NAME_24H, SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->settings.clock_24h, SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_COLOR,  SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->settings.clock_color, SETTING_TYPE_INTEGER, 0, COLOR_TABLE_MAX_COLORS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_BRIGHTNESS, SECTION_ID_CLOCK ) == true ) {
        this->parseSettingValue( value, &this->settings.clock_brightness, SETTING_TYPE_INTEGER,
                                 MIN_CLOCK_BRIGHTNESS, MAX_CLOCK_BRIGHTNESS );

    } else if( this->matchSettingName( name, SETTING_NAME_DATEFMT, SECTION_ID_LCD ) == true ) {
        this->parseSettingValue( value, &this->settings.date_format, SETTING_TYPE_INTEGER, 0, MAX_DATE_FORMATS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_CONTRAST, SECTION_ID_LCD ) == true ) {
        this->parseSettingValue( value, &this->settings.lcd_contrast, SETTING_TYPE_INTEGER,
                                 MIN_LCD_CONTRAST, MAX_LCD_CONTRAST );

    } else if( this->matchSettingName( name, SETTING_NAME_COLOR,  SECTION_ID_LAMP ) == true ) {
        this->parseSettingValue( value, &this->settings.lamp.color, SETTING_TYPE_INTEGER, 0, COLOR_TABLE_MAX_COLORS - 1 );

    } else if( this->matchSettingName( name, SETTING_NAME_BRIGHTNESS, SECTION_ID_LAMP ) == true ) {
        this->parseSettingValue( value, &this->settings.lamp.brightness, SETTING_TYPE_INTEGER, MIN_LAMP_BRIGHTNESS,
                                 MAX_LAMP_BRIGHTNESS );

    } else if( this->matchSettingName( name, SETTING_NAME_DELAY, SECTION_ID_LAMP ) == true ) {
        this->parseSettingValue( value, &this->settings.lamp.delay_off, SETTING_TYPE_INTEGER,
                                 MIN_LAMP_DELAY_OFF, MAX_LAMP_DELAY_OFF );

    } else if( this->matchSettingName( name, SETTING_NAME_DHCP, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->settings.net_dhcp, SETTING_TYPE_BOOL );

    } else if( this->matchSettingName( name, SETTING_NAME_ADDRESS, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->settings.net_ip, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_MASK, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->settings.net_mask, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_GATEWAY, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->settings.net_gateway, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_DNS, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->settings.net_dns, SETTING_TYPE_IP );

    } else if( this->matchSettingName( name, SETTING_NAME_SSID, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->settings.ssid, SETTING_TYPE_STRING, 0, MAX_SSID_LENGTH );

    } else if( this->matchSettingName( name, SETTING_NAME_WKEY, SECTION_ID_NETWORK ) == true ) {
        this->parseSettingValue( value, &this->settings.wkey, SETTING_TYPE_STRING, 0, MAX_WKEY_LENGTH );

    } else if( this->matchSettingName( name, SETTING_NAME_ENABLED, SECTION_ID_ALARM ) == true ) {
        this->parseSettingValue( value, &this->settings.alarm_on[ this->_currentAlarmID ], SETTING_TYPE_BOOL );

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


/*--------------------------------------------------------------------------
 *
 * Checks if the settings name on the current line in the config file matches
 * the provided name. Also checks if the settings belongs in to specified
 * section.
 *
 * Arguments
 * ---------
 *  - currentName : Setting's name parsed from the current line in the backup
 *                  file.
 *  - name        : Setting's name to match against.
 *  - section     : Section ID to match against. Use SECTION_ID_ANY to match
 *                  regardless of current section.
 *
 * Returns : TRUE if matching or FALSE otherwise.
 */
inline bool ConfigManager::matchSettingName( char* currentName, const char* name, uint8_t section ) {
    return ( strcmp_P( currentName, name ) == 0 && ( this->_currentSectionID == section || section == SECTION_ID_ANY ) );
}


/*--------------------------------------------------------------------------
 *
 * Converts the user readable format from the backup file and copy it to
 * the specified memory location.
 *
 * Arguments
 * ---------
 *  - src  : Value string from the backup file
 *  - dest : Pointer to the memory location where the setting
 *           must be copied to.
 *  - type : Variable type to parse
 *  - min  : Minimum value.
 *  - max  : Maximum value. If the setting is a STRING type, it defines
 *           the maximum characters to be copied.
 *
 * Returns : Nothing
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


/*--------------------------------------------------------------------------
 *
 * Parse the current line in the backup file.
 *
 * Arguments
 * ---------
 *  - name  : Pointer to the buffer where to copy the setting's name.
 *  - value : Pointer to the buffer where to copy the setting's value.
 *
 * Returns : The type of setting parsed.
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

    while( this->_sd_file.read( &chr, 1 ) == 1 ) {

        if( chr == '\r' || chr == '\n' ) {

            int nextChar = this->_sd_file.peek();

            /* Consumes the next character if it is a CR or LF. */
            if( nextChar == '\r' || nextChar == '\n' ) {
                this->_sd_file.read();
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


    // Serial.println( name );
    // Serial.println( value );
    // Serial.println();

    return type;
}


/*--------------------------------------------------------------------------
 *
 * Write the next setting line to the backup file.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool ConfigManager::writeNextLine()  {

    this->_currentSettingID++;

    if( this->_currentSettingID == SETTING_ID_ALARM_BEGIN ) {
        g_alarm.loadProfile( this ->_currentAlarmID );
    }

    switch( this->_currentSettingID ) {

        case SETTING_ID_CLOCK_24H:
            this->writeConfigLine( SETTING_NAME_SECTION_CLOCK, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_24H, SETTING_TYPE_BOOL, &this->settings.clock_24h );
            break;

        case SETTING_ID_CLOCK_COLOR:
            this->writeConfigLine( SETTING_NAME_COLOR, SETTING_TYPE_INTEGER, &this->settings.clock_color );
            break;

        case SETTING_ID_CLOCK_BRIGHTNESS:
            this->writeConfigLine( SETTING_NAME_BRIGHTNESS, SETTING_TYPE_INTEGER, &this->settings.clock_brightness );
            break;

        case SETTING_ID_LCD_DATEFMT:
            this->writeConfigLine( SETTING_NAME_SECTION_LCD, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_DATEFMT, SETTING_TYPE_INTEGER, &this->settings.date_format );
            break;

        case SETTING_ID_LCD_CONTRAST:
            this->writeConfigLine( SETTING_NAME_CONTRAST, SETTING_TYPE_INTEGER, &this->settings.lcd_contrast );
            break;

        case SETTING_ID_LAMP_COLOR:
            this->writeConfigLine( SETTING_NAME_SECTION_LAMP, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_COLOR, SETTING_TYPE_INTEGER, &this->settings.lamp.color );
            break;

        case SETTING_ID_LAMP_BRIGHTNESS:
            this->writeConfigLine( SETTING_NAME_BRIGHTNESS, SETTING_TYPE_INTEGER, &this->settings.lamp.brightness );
            break;

        case SETTING_ID_LAMP_DELAY:
            this->writeConfigLine( SETTING_NAME_DELAY, SETTING_TYPE_INTEGER, &this->settings.lamp.delay_off );
            break;

        case SETTING_ID_NETWORK_DHCP:
            this->writeConfigLine( SETTING_NAME_SECTION_NETWORK, SETTING_TYPE_SECTION, NULL );
            this->writeConfigLine( SETTING_NAME_DHCP, SETTING_TYPE_BOOL, &this->settings.net_dhcp );
            break;

        case SETTING_ID_NETWORK_IP:
            this->writeConfigLine( SETTING_NAME_ADDRESS, SETTING_TYPE_IP, &this->settings.net_ip );
            break;

        case SETTING_ID_NETWORK_MASK:
            this->writeConfigLine( SETTING_NAME_MASK, SETTING_TYPE_IP, &this->settings.net_mask );
            break;

        case SETTING_ID_NETWORK_GATEWAY:
            this->writeConfigLine( SETTING_NAME_GATEWAY, SETTING_TYPE_IP, &this->settings.net_gateway );
            break;

        case SETTING_ID_NETWORK_DNS:
            this->writeConfigLine( SETTING_NAME_DNS, SETTING_TYPE_IP, &this->settings.net_dns );
            break;

        case SETTING_ID_NETWORK_SSID:
            this->writeConfigLine( SETTING_NAME_SSID, SETTING_TYPE_STRING, &this->settings.ssid );
            break;

        case SETTING_ID_NETWORK_WKEY:
            this->writeConfigLine( SETTING_NAME_WKEY, SETTING_TYPE_STRING, &this->settings.wkey );
            break;

        case SETTING_ID_ALARM_ENABLED:
            this->writeConfigLine( SETTING_NAME_SECTION_ALARM, SETTING_TYPE_SECTION, &this->_currentAlarmID );
            this->writeConfigLine( SETTING_NAME_ENABLED, SETTING_TYPE_BOOL, &this->settings.alarm_on[ this->_currentAlarmID ] );
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

    this->_sd_file.sync();

    if( this->_sd_file.getError() != 0 ) {
        this->setTaskError( TASK_ERROR_WRITE );
        return false;
    }



    if( this->_currentSettingID >= SETTING_ID_END ) {

        this->_currentAlarmID++;

        if( this->_currentAlarmID < MAX_NUM_PROFILES ) {
            this->_currentSettingID = SETTING_ID_ALARM_BEGIN - 1;
            return true;
        }

        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Converts the setting value from memory to an user readable format.
 *
 * Arguments
 * ---------
 *  - name  : Name of the setting.
 *  - type  : Type of setting
 *  -
 *
 * Returns : Nothing.
 */
void ConfigManager::writeConfigLine( const char* name, uint8_t type, void* value ) {

    char buffer[ MAX_LENGTH_SETTING_VALUE ];

    if( this->_sd_file.isOpen() == false ) {
        return;
    }

    if( type != SETTING_TYPE_COMMENT ) {

        if( type == SETTING_TYPE_SECTION ) {
            this->_sd_file.write( "\r\n[" );
        }

        strcpy_P( buffer, name );

        this->_sd_file.write( buffer );


        if( type == SETTING_TYPE_SECTION ) {
            this->_sd_file.write( "]" );

            if( value != NULL ) {
                this->_sd_file.write( ":" );
                type = SETTING_TYPE_INTEGER;
            }

        } else {
            this->_sd_file.write( ": " );
        }
    }

    switch( type ) {
        case SETTING_TYPE_BOOL:

            if( *( ( bool* )value ) == true ) {
                this->_sd_file.write( SETTING_VALUE_TRUE );

            } else {
                this->_sd_file.write( SETTING_VALUE_FALSE );
            }

            break;

        case SETTING_TYPE_STRING:
            this->_sd_file.write( '"' );
            this->_sd_file.write( ( char* )value );
            this->_sd_file.write( '"' );
            break;

        case SETTING_TYPE_INTEGER:
            itoa( *( ( uint8_t* )value ), buffer, 10 );

            this->_sd_file.write( buffer );
            break;

        case SETTING_TYPE_IP:
            uint8_t ip[4];
            memcpy( &ip, value, sizeof( ip ) );

            sprintf( buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );

            this->_sd_file.write( buffer );
            break;

        case SETTING_TYPE_COMMENT:

            strncpy_P( buffer, ( const char* )value, MAX_LENGTH_SETTING_VALUE );
            this->_sd_file.write( buffer );
            break;

        case SETTING_TYPE_TIME:
            Time time;
            memcpy( &time, value, sizeof( Time ) );

            sprintf( buffer, "%02d:%02d", time.hour, time.minute );

            this->_sd_file.write( buffer );
            break;

        case SETTING_TYPE_DOW:
            uint8_t dow = *( ( uint8_t* )value );
            uint8_t n = 0;

            this->_sd_file.write( "[" );

            for( uint8_t i = 0; i < 7; i++ ) {

                if( dow & ( 1 << i ) ) {

                    if( n > 0 ) {
                        this->_sd_file.write( ", " );
                    }

                    strcpy_P( buffer, getDayName( i + 1, true ) );
                    this->_sd_file.write( buffer );
                    n++;
                }
            }

            this->_sd_file.write( "]" );
    }


    this->_sd_file.write( "\r\n" );
}