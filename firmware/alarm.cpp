//******************************************************************************
//
// Project : Alarm Clock V3
// File    : alarm.cpp
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

#include <EEPROM.h>
#include <Arduino.h>
#include "alarm.h"
#include <avr/pgmspace.h>
#include "resources.h"
#include "screen.h"
#include "config.h"
#include "us2066.h"
#include "neoclock.h"
#include "tpa2016.h"
#include "qt1070.h"
#include "lamp.h"


Screen _screen_alarm( SCREEN_ID_ALARM );

bool _showHelpMsg = false;

uint8_t vs1053_buffer[ VS1053_DATABUFFERLEN ];


Alarm::Alarm( int8_t pin_reset, int8_t pin_cs, int8_t pin_xdcs, int8_t pin_dreq, int8_t pin_sd_cs, int8_t pin_sd_detect, int8_t pin_alarm_sw ) 
    : Adafruit_VS1053( pin_reset, pin_cs, pin_xdcs, pin_dreq ) {

    this->_pin_sd_cs = pin_sd_cs;

    this->_pin_sd_detect = pin_sd_detect;
    pinMode( pin_sd_detect, INPUT );

    this->_pin_alarm_sw = pin_alarm_sw;
    pinMode( pin_alarm_sw, INPUT );
}

void Alarm::begin() {

    Adafruit_VS1053::begin();

    this->setVolume( 0 );



    this->sciWrite( VS1053_REG_BASS, 0 );



    _screen_alarm.eventDrawScreen = &alarmScreen_eventDrawScreen;
    _screen_alarm.eventKeypress = &alarmScreen_eventKeypress;
    _screen_alarm.eventEnterScreen = &alarmScreen_eventEnterScreen;
    _screen_alarm.eventTimeout = &alarmScreen_eventTimeout;
    _screen_alarm.timeout = 3000;
    
}


bool Alarm::isSDCardPresent() {
    return this->_sd_present;
}

bool Alarm::DetectSDCard() {


    if ( digitalRead( this->_pin_sd_detect ) == LOW ) {


        /* Already detected, no need to re-initialize */
        if ( this->_sd_present == true ) {
            return true;
        }

        /* Start the debouce timer */
        if (( this->_timerStart == 0 ) || ( this->_timerStart > millis() )) {

            this->_timerStart = millis();
            return false;
        }

        /* Still wait fot the detect card signal to make up it's mind. */
        if (( millis() - this->_timerStart ) < 1000 ) {
            return false;
        }

        /* The card was detected, now we can initialize or re-initalize it */
        this->_sd_present = true;

    } else {

        /* Already not present, do nothing */
        if ( this->_sd_present == false ) {
            return false;
        }

        if ( this->_playMode != ALARM_MODE_OFF ) {

            if ( this->_playMode & ALARM_MODE_TEST ) {
                this->stop();
            } else {

                /* If SD card is removed during an alarm, don't stop the alarm.
                   Fallback sound will be played once the snooze delay is elapsed. */
                this->snooze();
            }
        }

        this->currentFile.close();
        this->_sd_root.close();
        SD.end();

        this->_timerStart = 0;
        this->_sd_present = false;

        return false;
    }

    this->_sd_present = false;
    this->_timerStart = 0;
    

    Serial.println( F("Init SD card... "));

    if ( SD.begin( this->_pin_sd_cs ) == false ) {

        Serial.println( F("SD init failed "));
        return false;
    }

    /* Open root directory */
    this->_sd_root = SD.open( "/" );
    if ( this->_sd_root == false ) {

        Serial.println( F("Cannot open root directory on SD card"));
        return false;
    }

    this->_sd_present = true;
    return true;
}


bool Alarm::openNextFile() {

    if ( this->_playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    if ( this->openFile( NULL ) == false ) {

        this->profile.filename[0] = 0;
        return false;
    }

    return true;
}

bool Alarm::openFile( char *name ) {

    if ( this->_sd_root == false ) {
        return false;
    }

    /* Close current file if open */
    if (this->currentFile != false ) {
        this->currentFile.close();
    }

    /* Open the specified file */
    if ( name != NULL )  {

        if ( strlen( name ) > 0 ) {

            this->currentFile = SD.open( name, FILE_READ );

            if ( this->currentFile != false ) {
                strcpy( this->profile.filename, this->currentFile.name() );
            }
        }

    /* Open the next file in the root directory */
    } else {

        this->currentFile = this->_sd_root.openNextFile();

        /* If the next filename is the same than the currently slected one, go
           to the next file */
        if ( strcmp( this->profile.filename, this->currentFile.name()) == 0 ) {
            this->currentFile = this->_sd_root.openNextFile();
        }

        strcpy( this->profile.filename, this->currentFile.name() );
    }


    while ( this->currentFile != false ) {

        /* Validate file format */
        if (    strstr_P( this->profile.filename, PSTR( ".MP3" )) != NULL ||
                strstr_P( this->profile.filename, PSTR( ".MID" )) != NULL ||
                strstr_P( this->profile.filename, PSTR( ".OGG" )) != NULL ||
                strstr_P( this->profile.filename, PSTR( ".AAC" )) != NULL ||
                strstr_P( this->profile.filename, PSTR( ".WAV" )) != NULL ) {

            /* File extention is valid */
            return true;
        }

        /* invalid extention, close the file and go for the next one */
        this->currentFile.close();

        this->currentFile = this->_sd_root.openNextFile();
        strcpy( this->profile.filename, this->currentFile.name() );
    }


    if ( this->currentFile == false ) {

        this->_sd_root.rewindDirectory();
        return false;
    }

    return true;
}


bool Alarm::readProfileAlarmTime( uint8_t profile_id, Time *time, uint8_t *dow ) {
    uint8_t i;
    uint8_t byte;

    if ( profile_id > MAX_ALARM_PROFILES - 1 ) {
        return false;
    }

    if ( time != NULL ) {
        for ( i = 0; i < sizeof( Time ); i++ ) {

            byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( profile_id * sizeof( AlarmProfile ))
            + i + offsetof( struct AlarmProfile, time ));

            *((( uint8_t* )time ) + i ) = byte;
        }
    }

    if ( dow != NULL ) {
        byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( profile_id * sizeof( AlarmProfile ))
                + offsetof( struct AlarmProfile, dow ));

        *dow = byte;
    }

    return true;
}

bool Alarm::loadProfile( uint8_t id ) {

    return this->loadProfile( &this->profile, id );
}

bool Alarm::loadProfile( AlarmProfile *profile, uint8_t id ) {

    if ( id > MAX_ALARM_PROFILES - 1 ) {
        return false;
    }

    for ( uint8_t i = 0; i < sizeof( AlarmProfile ); i++ ) {

        uint8_t byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( id * sizeof( AlarmProfile )) + i );

        *((( uint8_t* )profile) + i) = byte;
    }


    this->setVolume( this->profile.volume );

    return true;
}

void Alarm::saveProfile( uint8_t id ) {
    this->saveProfile( &this->profile, id );
}

void Alarm::saveProfile( AlarmProfile *profile, uint8_t id ) {
    if ( id > MAX_ALARM_PROFILES - 1 ) {
        return;
    }

    for ( uint8_t i = 0; i < sizeof( AlarmProfile ); i++ ) {

        uint8_t byte = *((( uint8_t* )profile ) + i );

        EEPROM.update( EEPROM_ADDR_PROFILES + ( id * sizeof( AlarmProfile )) + i, byte );
    }

}

void Alarm::play( uint8_t mode, uint16_t delay ) {

    if ( this->_playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    this->_playMode = mode;
    this->_timerStart = millis();
    this->_playDelay = delay;
    this->_snoozeStart = 0;
}

void Alarm::play( uint8_t mode ) {

    if ( this->_playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    this->detectAlarmSwitchState();
    if ( this->_alarm_sw_on == false && (( mode & ALARM_MODE_TEST ) == 0 )) {
        return;
    }

    this->_playMode = mode;
    this->_timerStart = 0;
    this->_playDelay = 0;
    this->_snoozeStart = 0;


    if ( mode & ALARM_MODE_SCREEN ) {
        gotoScreen( &_screen_alarm, true, g_currentScreen );
    }

    if ( mode & ALARM_MODE_AUDIO ) {
        this->audioStart();
    }

    if ( mode & ALARM_MODE_VISUAL || mode & ALARM_MODE_LAMP ) {
        this->visualStart();
    }
}


void Alarm::stop() {

    if ( this->_playMode == ALARM_MODE_OFF ) {
        return;
    }

    if ( this->_playMode & ALARM_MODE_AUDIO ) {
        this->audioStop();
    }

    if ( this->_playMode & ALARM_MODE_VISUAL || this->_playMode & ALARM_MODE_LAMP  ) {
        this->visualStop();
    }

    if ( this->_playMode & ALARM_MODE_SCREEN ) {
        _screen_alarm.exitScreen();
    }

    this->_playMode = ALARM_MODE_OFF;
    this->_timerStart = 0;    
    this->_snoozeStart = 0;   
}

void Alarm::audioStop() {

    g_amplifier.disableOutputs();

    if ( this->_playMode & ALARM_MODE_AUDIO ) {

        // cancel all playback
        sciWrite( VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_CANCEL );
    }
}

void Alarm::audioStart() {
    if (( this->_playMode & ALARM_MODE_AUDIO) == 0 ) {
        return;
    }

    this->openFile( this->profile.filename );


    if ( this->currentFile == false ) {
        this->_pgm_audio_ptr = 0;
    } else {
        this->currentFile.seek( 0 );
    }

    /* reset playback */
    this->sciWrite( VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW );

    /* resync */
    this->sciWrite( VS1053_REG_WRAMADDR, 0x1e29 );
    this->sciWrite( VS1053_REG_WRAM, 0 );

    /* As explained in datasheet, set twice 0 in REG_DECODETIME to set time back to 0 */
    sciWrite( VS1053_REG_DECODETIME, 0x00 );
    sciWrite( VS1053_REG_DECODETIME, 0x00 );
    

    g_amplifier.enableOutputs();
}


void Alarm::snooze() {

    if (( this->_playMode == ALARM_MODE_OFF ) || ( this->_playMode & ALARM_MODE_SNOOZE )) {
        return;
    }

    /* if snooze delay is off, turn off the alarm instead. */
    if ( this->profile.snoozeDelay == 0 ) {
        this->stop();
        return;
    }

    this->_playMode |= ALARM_MODE_SNOOZE;
    this->_snoozeStart = millis();
        
    this->audioStop();
    this->visualStop();
    

    g_screenClear = true;
    g_screenUpdate = true;

    _screen_alarm.resetTimeout();
    _screen_alarm.timeout = 1000;
}

void Alarm::resume() {
    
    if ( this->_playMode == ALARM_MODE_OFF || (( this->_playMode & ALARM_MODE_SNOOZE ) == 0 )) {
        return;
    }

    this->_playMode &= ~ALARM_MODE_SNOOZE;
    this->_snoozeStart = 0;

    this->audioStart();
    this->visualStart();

    if ( this->_playMode & ALARM_MODE_SCREEN ) {

        g_screenUpdate = true;
        g_screenClear = true;
    }

    
}


void Alarm::setVolume( uint8_t vol ) {

    /* 120: -60db  0: +0db */

    if ( vol > 100 ) { vol = 100; }
    vol = 100 - ( vol * 100 / 100 );

    Adafruit_VS1053::setVolume( vol, vol );

}

bool Alarm::isSnoozing() {
    return this->_playMode & ALARM_MODE_SNOOZE;
}

bool Alarm::isPlaying() {
    return ( this->_playMode != ALARM_MODE_OFF && (( this->_playMode & ALARM_MODE_SNOOZE ) == 0 ));
}

uint8_t Alarm::getPlayMode() {
    return this->_playMode;
}

uint16_t Alarm::getSnoozeTimeRemaining() {

    if (( this->_playMode & ALARM_MODE_SNOOZE ) == 0 ) {
        return 0;
    }

    if ( this->profile.snoozeDelay == 0 ) {
        return 0;
    }

    return (this->profile.snoozeDelay * 60 ) - (( millis() - this->_snoozeStart ) / 1000 );
}






void Alarm::visualStart() {

    /* Turn on lamp if option enabled */
    if( this->_playMode & ALARM_MODE_LAMP && profile.lamp.mode != LAMP_MODE_OFF ) {

        this->profile.lamp.delay_off = 0;
        g_lamp.activate( &this->profile.lamp );
    }

    /* Visual mode not enabled */
    if (( this->_playMode & ALARM_MODE_VISUAL ) == 0 || this->profile.visualMode == ALARM_VISUAL_NONE ) {
        return;
    }

    this->_visualStepReverse = false;

    switch ( this->profile.visualMode ) {

        case ALARM_VISUAL_FADING:
            this->_visualStepValue = g_config.clock_brightness;
            break;

        default:
            this->_visualStepValue = 0;
            break;
    }

    this->updateVisualStepDelay();
}


inline void Alarm::updateVisualStepDelay() {
    switch ( this->profile.visualMode ) {
        case ALARM_VISUAL_FADING:
        case ALARM_VISUAL_RAINBOW:
            this->_visualStepDelay = 250 / this->profile.effectSpeed;
            break;

        case ALARM_VISUAL_NONE:
            this->_visualStepDelay = 0;
            break;


        default:
            this->_visualStepDelay = 2000 / this->profile.effectSpeed;
            break;        
    }
}

inline void Alarm::visualStep() {

    if ( this->profile.visualMode == ALARM_VISUAL_NONE ) {
        return;
    }

    /* Check if visual effect is enabled */
    if ( this->_visualStepDelay == 0 ) {
        return; 
    }

    /* Check if the effect next step delay is elapsed */
    if (( millis() - this->_timerStart) < this->_visualStepDelay ) {
        return;
    }

    switch ( this->profile.visualMode ) {

        case ALARM_VISUAL_FLASHING:
        case ALARM_VISUAL_RED_FLASH:
            this->_visualStepReverse = !this->_visualStepReverse;
            g_clock.setBrightness( this->_visualStepReverse ? 0 : ( g_config.clock_brightness + 25 ));

            if ( this->profile.visualMode == ALARM_VISUAL_RED_FLASH ) {
                g_clock.setColorFromTable( COLOR_RED );
            }
            break;

        case ALARM_VISUAL_WHITE_FLASH:
            this->_visualStepReverse = !this->_visualStepReverse;

            g_clock.setBrightness( g_config.clock_brightness + 25 );
            g_clock.setColorFromTable( this->_visualStepReverse ? COLOR_WHITE : g_config.clock_color );
            break;

        case ALARM_VISUAL_FADING:

            if ( this->_visualStepValue < (( g_config.clock_brightness < 25 ) ? 5 : ( g_config.clock_brightness - 20 ))) {
                this->_visualStepReverse = false;
            }
            if ( this->_visualStepValue > ( g_config.clock_brightness + 20 )) {
                this->_visualStepReverse = true;
            }

            this->_visualStepValue += ( this->_visualStepReverse ? -5 : 5 )    ;
            g_clock.setBrightness( this->_visualStepValue );

            break;

        case ALARM_VISUAL_RAINBOW:

            g_clock.setBrightness( g_config.clock_brightness + 25 );

            this->_visualStepValue += 5;


            if( this->_visualStepValue < 85) {
                g_clock.setColorRGB( this->_visualStepValue * 3,
                                     255 - this->_visualStepValue * 3,
                                     0 );

            } else if( this->_visualStepValue < 170 ) {

                g_clock.setColorRGB( 255 - ( this->_visualStepValue - 85 ) * 3,
                                     0,
                                     ( this->_visualStepValue - 85 ) * 3 );
            } else {

                g_clock.setColorRGB( 0,
                                     (this->_visualStepValue - 170 ) * 3,
                                     255 - ( this->_visualStepValue - 170 ) * 3 );
            }

            break;

    }

    g_clock.update();

    this->_timerStart = millis();
    
    this->updateVisualStepDelay();
}

inline void Alarm::visualStop() {

    /* Turn off lamp if active */
    g_lamp.deactivate();

    /* Restore clock settings */
    g_clock.setColorFromTable( g_config.clock_color );
    g_clock.setBrightness( g_config.clock_brightness );

    g_clock.update();
}



void Alarm::processAlarmEvents() {

    if ( this->_alarm_sw_on == false && (( this->_playMode & ALARM_MODE_TEST ) == 0 )) {
        this->stop();
        return;
    }

    if ( this->_playMode & ALARM_MODE_SNOOZE ) {

        if ( millis() - this->_snoozeStart > ( this->profile.snoozeDelay * 60000 )) {
            this->resume();
        }

        return;
    }

    if ( this->_playMode == ALARM_MODE_OFF ) {
        return;
    }

    

    if ( this->_playDelay > 0 ) {

        if ( millis() - this->_timerStart < this->_playDelay ) {
            return;
        }
        

        this->play( this->_playMode );
    }

    if ( this->_playMode & ALARM_MODE_AUDIO ) {
        this->feedBuffer();
    }

    if ( this->_playMode & ALARM_MODE_VISUAL ) {
        this->visualStep();
    }
}

inline void Alarm::feedBuffer() {

    if ( this->_playMode & ALARM_MODE_SNOOZE ) {
        return;
    }

    if ( this->readyForData() == false ) {
        return;
    }

    
    uint8_t bytesRead;

    /* Playback from program memory space */
    if ( this->currentFile == false ) {

        if ( this->_pgm_audio_ptr + VS1053_DATABUFFERLEN > DEFAULT_ALARMSOUND_DATA_LENGTH ) {

            bytesRead = DEFAULT_ALARMSOUND_DATA_LENGTH - this->_pgm_audio_ptr;
            memcpy_P( &vs1053_buffer, &_DEFAULT_ALARMSOUND_DATA[ this->_pgm_audio_ptr ], bytesRead );

            this->_pgm_audio_ptr = 0;

        } else {

            bytesRead = VS1053_DATABUFFERLEN;
            memcpy_P( &vs1053_buffer, &_DEFAULT_ALARMSOUND_DATA[ this->_pgm_audio_ptr ], VS1053_DATABUFFERLEN );

            this->_pgm_audio_ptr += VS1053_DATABUFFERLEN;
        }

    /* Playback from file on SD card */
    } else {

        bytesRead = this->currentFile.read( vs1053_buffer, VS1053_DATABUFFERLEN );

        if (bytesRead == 0 ) {
            /* Play the file in loop */
            this->currentFile.seek( 0 );
        }
    }

    Adafruit_VS1053::playData( vs1053_buffer, bytesRead );
}


bool Alarm::isAlarmSwitchOn() {
    return this->_alarm_sw_on;
}

bool Alarm::detectAlarmSwitchState() {

    this->_alarm_sw_on = ( digitalRead( this->_pin_alarm_sw ) == HIGH );
    return this->_alarm_sw_on;
}

bool Alarm::isAlarmEnabled() {

    if ( this->_alarm_sw_on == false ) {
        return false;
    }

    return (( g_config.alarm_on[ 0 ] == true ) || ( g_config.alarm_on[ 1 ] == true ));
}

bool Alarm::checkForAlarms( DateTime *now ) {

    if ( this->isAlarmEnabled() == false ) {
        return false;
    }

    int8_t alarm_id = this->getNextAlarmID( now, true );
    int16_t offset = this->getNextAlarmOffset( alarm_id, now, true );

    if ( offset != 0 ) {
        return false;
    }

    this->loadProfile( alarm_id );
    this->play( ALARM_MODE_NORMAL );

    return true;
}

int8_t Alarm::getNextAlarmID( DateTime *currentTime, bool matchNow ) {

    if ( this->isAlarmEnabled() == false ) {

        /* All alarms off */
        return -1;
    }

    if ( g_config.alarm_on[ 1 ] == true ) {

        if ( g_config.alarm_on [ 0 ] == false ) {

            /* Alarm 0 is not active, so alarm 1 is always next */
            return 1;
        }

        /* Compare the time offset from both alarms. */
        if ( this->getNextAlarmOffset( 1, currentTime, matchNow ) < this->getNextAlarmOffset( 0, currentTime, matchNow ) ) {

            /* Alarm 1 is sooner than alarm 0 */
            return 1;
        }
    }

    /* Alarm 0 is sooner, or is the only alarm active */
    return 0;
}


int16_t Alarm::getNextAlarmOffset( int8_t alarm_id, DateTime *currentTime, bool matchNow ) {

    if ( alarm_id > MAX_ALARM_PROFILES - 1 ) {
        return -1;
    }

    Time profile_time;
    uint8_t profile_dow;

    if ( g_alarm.readProfileAlarmTime( alarm_id, &profile_time, &profile_dow ) == false ) {
        return -1;
    }

    uint8_t a_hour = profile_time.hour;
    uint8_t a_min = profile_time.minute;
    uint8_t a_dow = currentTime->dayOfWeek();
    uint8_t a_dayOffset = 0;


    /* No days selected, considering alarm off */
    if ( profile_dow == 0x00 ) {
        return -1;
    }

    /* Find out on which day the next alarm occurs */
    while ( a_dayOffset < 7 ) {
        if ( a_dow > 7 )
            a_dow = 1;

        if ( bitRead( profile_dow, a_dow - 1 )) {

            /* Next alarm is on a different day than the current one. */
            if ( a_dow != currentTime->dayOfWeek() ) {
                break;
            } else {

                /* Next alarm is now */
                if ( matchNow && a_hour == currentTime->hour() && a_min == currentTime->minute() ) {
                    break;
                }

                /* Check if the alarm time is equal or greater than the current time, if not,
                   continue searching */
                if ( a_hour > currentTime->hour() || ( a_hour == currentTime->hour() && a_min > currentTime->minute() )) {
                    break;
                }
            }
        }

        a_dow++;
        a_dayOffset++;
    }

    return ( a_dayOffset * 1440 ) + (( a_hour - currentTime->hour() ) * 60 ) + ( a_min - currentTime->minute() );
}




bool alarmScreen_eventEnterScreen( Screen *screen ) {

    _showHelpMsg = false;
       
    return true;
}

bool alarmScreen_eventKeypress( Screen *screen, uint8_t key ) {
    uint8_t mode = g_alarm.getPlayMode();

    if ( mode & ALARM_MODE_TEST ) {
        g_alarm.stop();
        return false;
    }

    if ( g_alarm.isSnoozing() ) {

        if ( key == ( KEY_ALARM | KEY_SHIFT )) { 
            g_alarm.stop();
            return false;

        } else {
            _showHelpMsg = true;

            _screen_alarm.resetTimeout();
            _screen_alarm.timeout = 3000;

            g_screenUpdate = true;
            g_screenClear = true;
        }

    } else {

        if ( g_alarm.profile.snoozeDelay != 0 ) {
            g_alarm.snooze();

        } else {

            g_alarm.stop();
        }
    }

    return false;
}

bool alarmScreen_eventDrawScreen( Screen *screen ) {
    uint8_t mode = g_alarm.getPlayMode();

    

    if ( g_alarm.isSnoozing() && (( mode & ALARM_MODE_TEST ) == 0 )) {
        if ( _showHelpMsg == true ) {

            g_lcd.print_P( S_INSTR_CANCEL_ALARM_1);
            g_lcd.setPosition( 1, 0 );
            g_lcd.print_P( S_INSTR_CANCEL_ALARM_2 );

            return true;
        }

        uint16_t remaining;
        remaining = g_alarm.getSnoozeTimeRemaining();

        g_lcd.setPosition( 0, 0 );
        g_lcd.print_P( S_SNOOZE, 16, TEXT_ALIGN_CENTER );

        g_lcd.setPosition( 1, 5 );
        g_lcd.printf_P( S_SNOOZE_TIME, remaining / 60, remaining % 60 );

        return true;
    }
    

    g_lcd.setPosition( 1, 0 );
    if ( strlen( g_alarm.profile.message ) > 0 ) {
        g_lcd.print( g_alarm.profile.message, 16, TEXT_ALIGN_CENTER );

    } else {

        if ( g_alarm.profile.time.hour < 12 ) {
            g_lcd.print_P( S_ALARM_MSG_MORNING, 16, TEXT_ALIGN_CENTER );
            
        } else if ( g_alarm.profile.time.hour < 18 ) {
            g_lcd.print_P( S_ALARM_MSG_AFTERNOON, 16, TEXT_ALIGN_CENTER );

        } else {
            g_lcd.print_P( S_ALARM_MSG_EVENING, 16, TEXT_ALIGN_CENTER );
        }
    }

    return true;
}

void alarmScreen_eventTimeout( Screen *screen ) {
    if ( _showHelpMsg == true ) {
        _showHelpMsg = false;

        _screen_alarm.resetTimeout();
        _screen_alarm.timeout = 1000;

        g_screenClear = true;
    }

    g_screenUpdate = true;
}


 