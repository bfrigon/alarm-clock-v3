//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/alarm.cpp
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
#include "alarm.h"
#include "screen.h"
#include "ui/ui.h"


uint8_t vs1053_buffer[VS1053_DATABUFFERLEN];


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - pin_cs        : Codec chip select pin
 *  - pin_xdcs      : Codec data select pin
 *  - pin_dreq      : Codec data request pin.
 *  - pin_reset     : Codec reset pin.
 *  - pin_sd_cs     : SD card chip select pin.
 *  - pin_sd_detect : SD card detect pin.
 *  - pin_alarm_sw  : Pin connected to the alarm switch.
 *  - pin_amp_shdn  : Amplifier shutdown pin.
 */
Alarm::Alarm( int8_t pin_reset, int8_t pin_cs, int8_t pin_xdcs, int8_t pin_dreq, int8_t pin_sd_cs, int8_t pin_sd_detect,
              int8_t pin_alarm_sw, int8_t pin_amp_shdn ) : VS1053( pin_cs, pin_xdcs, pin_dreq, pin_reset ) {

    this->_pin_sd_cs = pin_sd_cs;

    this->_pin_sd_detect = pin_sd_detect;
    pinMode( pin_sd_detect, INPUT );

    this->_pin_alarm_sw = pin_alarm_sw;
    pinMode( pin_alarm_sw, INPUT );

    this->_amplifier.setPins( pin_amp_shdn );
}


/*--------------------------------------------------------------------------
 *
 * Initialize the codec and amplifier.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
uint8_t Alarm::begin() {
    if( this->_init == true ) {
        return;
    }

    this->_init = true;
    this->_volume = 0;

    this->updatePowerState();
}


/*--------------------------------------------------------------------------
 *
 * Power down the amplifier and codec.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::end() {
    if( this->_init == false ) {
        return;
    }

    this->_init = false;

    this->_amplifier.end();
    delay( 50 );

    VS1053::end();
}


/*--------------------------------------------------------------------------
 *
 * Power up or down the codec and amplifier based on the current power mode
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::updatePowerState() {

    if( this->_init == false ) {
        return;
    }

    if( g_power.getPowerMode() != POWER_MODE_SUSPEND ) {
        VS1053::begin();

        this->_amplifier.begin();
        this->_amplifier.setFixedGain( 25 );

        delay( 50 );

    } else {
        this->_amplifier.end();
        delay( 50 );

        VS1053::end();
    }
}


/*--------------------------------------------------------------------------
 *
 * Return whether the SD card was detected or not.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if detectedor False otherwise.
 */
bool Alarm::isSDCardPresent() {
    return this->_sd_present;
}


/*--------------------------------------------------------------------------
 *
 * Check the current state of the SD card detect pin. If a card is found,
 * initialize the SdFat library.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if detectedor False otherwise.
 */
bool Alarm::DetectSDCard() {
    if( digitalRead( this->_pin_sd_detect ) == LOW ) {

        /* Already detected, no need to re-initialize */
        if( this->_sd_present == true ) {
            return true;
        }

        if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
            if( this->_timerStart == 0 ) {
                /* millis() not usable in suspend mode, debounce using wake-up counter */
                this->_timerStart = 1;
                return false;
            }

        } else {
            /* Start the debouce timer using millis() */
            if( this->_timerStart == 0 ) {
                this->_timerStart = millis();
                return false;
            }

            /* Still wait fot the detect card signal to make up it's mind. */
            if( ( millis() - this->_timerStart ) < 1000 ) {
                return false;
            }
        }

        /* The card was detected, now we can initialize or re-initalize it */
        this->_sd_present = true;

    } else {
        /* Already not present, do nothing */
        if( this->_sd_present == false ) {
            return false;
        }

        if( this->_playMode != ALARM_MODE_OFF ) {
            if( this->_playMode & ALARM_MODE_TEST ) {
                this->stop();

            } else {
                /* If SD card is removed during an alarm, don't stop the alarm.
                   Fallback sound will be played once the snooze delay is elapsed. */
                this->snooze();
            }
        }

        this->currentFile.close();
        this->_timerStart = 0;
        this->_sd_present = false;
        return false;
    }

    this->_sd_present = false;
    this->_timerStart = 0;
    Serial.println( F( "Init SD card... " ) );

    if( this->_sd.begin( this->_pin_sd_cs ) == false ) {
        Serial.println( F( "SD init failed " ) );
        return false;
    }

    /* Open root directory */
    this->_sd.vwd()->rewind();

    if( this->_sd.vwd()->isOpen() == false ) {
        Serial.println( F( "Cannot open root directory on SD card" ) );
        return false;
    }

    this->_sd_present = true;
    return true;
}


/*--------------------------------------------------------------------------
 *
 * Open the next music file in the SD card root directory. If the end of
 * the directory was reached, it will select the fallback file stored
 * in program memory. The next time openNextFile is called, it will select the
 * first file.
 *
 * If the card was removed or a read error occurs, it will use the fallback
 * file instead.
 *
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successfulor False otherwise.
 */
bool Alarm::openNextFile() {
    if( this->_playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    if( this->openFile( NULL ) == false ) {
        this->profile.filename[0] = 0;
        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Open a specific music file on the SD card located in the root directory.
 *
 * If the file is not found or card was removed, it will use the fallback
 * file instead.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successfulor False otherwise.
 */
bool Alarm::openFile( char *name ) {
    if( this->_sd.vwd()->isOpen() == false ) {
        return false;
    }

    /* Close current file if open */
    if( this->currentFile.isOpen() ) {
        this->currentFile.close();
    }

    /* Open the specified file */
    if( name != NULL ) {
        if( strlen( name ) > 0 ) {
            this->currentFile.open( this->_sd.vwd(), name, O_READ );
            this->currentFile.getSFN( this->profile.filename );
        }

        /* Open the next file in the root directory */

    } else {
        this->currentFile.openNext( this->_sd.vwd(), O_READ );

        char buffer[ ALARM_FILENAME_LENGTH ];
        this->currentFile.getSFN( buffer );

        /* If the next filename is the same than the currently slected one, go
           to the next file */
        if( strncmp( this->profile.filename, buffer, ALARM_FILENAME_LENGTH )
                == 0 ) {
            this->currentFile.openNext( this->_sd.vwd(), O_READ );
        }

        strcpy( this->profile.filename, buffer );
    }

    while( this->currentFile.isOpen() != false ) {

        /* Validate file extension */
        if( this->currentFile.isFile() == true ) {
            if( strstr_P( this->profile.filename, PSTR( ".MP3" ) ) != NULL
                    || strstr_P( this->profile.filename, PSTR( ".MID" ) ) != NULL
                    || strstr_P( this->profile.filename, PSTR( ".OGG" ) ) != NULL
                    || strstr_P( this->profile.filename, PSTR( ".AAC" ) ) != NULL
                    || strstr_P( this->profile.filename, PSTR( ".WAV" ) ) != NULL ) {

                /* File extention is valid */
                return true;
            }
        }

        /* invalid extention, close the file and go for the next one */
        this->currentFile.close();
        this->currentFile.openNext( this->_sd.vwd(), O_READ );
        this->currentFile.getSFN( this->profile.filename );
    }

    if( this->currentFile.isOpen() == false ) {
        this->_sd.vwd()->rewind();
        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Read the alarm time for a given profile ID
 *
 * Arguments
 * ---------
 *  - profile_id : Profile to read the time from.
 *  - time       : Pointer to a Time structure where the alarm time will be
 *                 written.
 *  - dow        : Pointer to a unsigned integer where the day of week mask
 *                 will be copied.
 *
 * Returns : TRUE if successfulor False otherwise.
 */
bool Alarm::readProfileAlarmTime( uint8_t profile_id, Time *time, uint8_t *dow ) {
    uint8_t i;
    uint8_t byte;

    if( profile_id > MAX_ALARM_PROFILES - 1 ) {
        return false;
    }

    if( time != NULL ) {
        for( i = 0; i < sizeof( Time ); i++ ) {
            byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( profile_id * sizeof( AlarmProfile ) ) + i
                                + offsetof( struct AlarmProfile, time ) );


            * ( ( ( uint8_t * ) time ) + i ) = byte;
        }
    }

    if( dow != NULL ) {
        byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( profile_id * sizeof( AlarmProfile ) )
                            + offsetof( struct AlarmProfile, dow ) );


        *dow = byte;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Load the given alarm profile from EEPROM
 *
 * Arguments
 * ---------
 *  - id : Alarm profile ID to load.
 *
 * Returns : TRUE if successfulor False otherwise.
 */
bool Alarm::loadProfile( uint8_t id ) {
    return this->loadProfile( &this->profile, id );
}


/*--------------------------------------------------------------------------
 *
 * Load the given alarm profile from EEPROM into an AlarmProfile structure.
 *
 * Arguments
 * ---------
 *  - id : Alarm profile ID to load.
 *
 * Returns : TRUE if successfulor False otherwise.
 */
bool Alarm::loadProfile( AlarmProfile *profile, uint8_t id ) {
    if( id > MAX_ALARM_PROFILES - 1 ) {
        return false;
    }

    for( uint8_t i = 0; i < sizeof( AlarmProfile ); i++ ) {
        uint8_t byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( id * sizeof( AlarmProfile ) ) + i );
        * ( ( ( uint8_t * ) profile ) + i ) = byte;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Store the current profile settings in EEPROM.
 *
 * Arguments
 * ---------
 *  - id : Alarm profile ID to load.
 *
 * Returns :
 */
void Alarm::saveProfile( uint8_t id ) {
    this->saveProfile( &this->profile, id );
}


/*--------------------------------------------------------------------------
 *
 * Store alarm profile settings contained in a given AlarmProfile structure
 * to EEPROM
 *
 * Arguments
 * ---------
 *  - profile : AlarmProfile structure containing the settings.
 *  - id      : Profile ID to save the settings to.
 *
 * Returns :
 */
void Alarm::saveProfile( AlarmProfile *profile, uint8_t id ) {
    if( id > MAX_ALARM_PROFILES - 1 ) {
        return;
    }

    for( uint8_t i = 0; i < sizeof( AlarmProfile ); i++ ) {
        uint8_t byte = * ( ( ( uint8_t * ) profile ) + i );
        EEPROM.update( EEPROM_ADDR_PROFILES + ( id * sizeof( AlarmProfile ) ) + i, byte );
    }
}


/*--------------------------------------------------------------------------
 *
 * Activate the alarm with a start delay.
 *
 * Arguments
 * ---------
 *  - mode  : Alarm play mode.
 *  - delay : Delay in ms to wait before activating alarm.
 *
 * Returns : Nothing
 */
void Alarm::play( uint8_t mode, uint16_t delay ) {
    if( this->_playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    this->_playMode = mode;
    this->_timerStart = millis();
    this->_playDelay = delay;
    this->_snoozeStart = 0;
}


/*--------------------------------------------------------------------------
 *
 * Activate the alarm immediately.
 *
 * Arguments
 * ---------
 *  - mode  : Alarm play mode.
 *
 * Returns : Nothing
 */
void Alarm::play( uint8_t mode ) {
    if( this->_playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    this->detectAlarmSwitchState();

    if( this->_alarm_sw_on == false && ( ( mode & ALARM_MODE_TEST ) == 0 ) ) {
        return;
    }

    this->_playMode = mode;
    this->_timerStart = 0;
    this->_playDelay = 0;
    this->_snoozeStart = 0;
    this->_alarmStart = millis();

    if( mode & ALARM_MODE_SCREEN ) {
        screen_alarm.activate( true, g_currentScreen );
    }

    if( mode & ALARM_MODE_AUDIO ) {
        this->audioStart();
    }

    if( mode & ALARM_MODE_VISUAL || mode & ALARM_MODE_LAMP ) {
        this->visualStart();
    }
}


/*--------------------------------------------------------------------------
 *
 * Stops the alarm.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::stop() {
    if( this->_playMode == ALARM_MODE_OFF ) {
        return;
    }

    if( this->_playMode & ALARM_MODE_AUDIO ) {
        this->audioStop();
    }

    if( this->_playMode & ALARM_MODE_VISUAL || this->_playMode & ALARM_MODE_LAMP ) {
        this->visualStop();
    }

    if( this->_playMode & ALARM_MODE_SCREEN ) {
        screen_alarm.exitScreen();
    }

    this->_playMode = ALARM_MODE_OFF;
    this->_timerStart = 0;
    this->_snoozeStart = 0;
}


/*--------------------------------------------------------------------------
 *
 * Stop the audio element of the alarm.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::audioStop() {
    this->_amplifier.disableOutputs();

    if( this->_playMode & ALARM_MODE_AUDIO ) {

        // cancel all playback
        sciWrite( VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_CANCEL );
    }
}


/*--------------------------------------------------------------------------
 *
 * Start the audio element of the alarm.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::audioStart() {
    if( ( this->_playMode & ALARM_MODE_AUDIO ) == 0 ) {
        return;
    }

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        g_power.setPowerMode( POWER_MODE_LOW_POWER );
    }

    if( this->profile.gradual == true && ( ( this->_playMode & ALARM_MODE_TEST ) == 0 ) ) {
        this->setVolume( 0 );

    } else {
        this->setVolume( this->profile.volume );
    }

    this->openFile( this->profile.filename );

    if( this->currentFile.isOpen() == false ) {
        this->_pgm_audio_ptr = 0;

    } else {
        this->currentFile.rewind();
    }

    /* reset playback */
    this->sciWrite( VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW );

    /* resync */
    this->sciWrite( VS1053_REG_WRAMADDR, 0x1e29 );
    this->sciWrite( VS1053_REG_WRAM, 0 );

    /* As explained in datasheet, set twice 0 in REG_DECODETIME to set time back to 0 */
    sciWrite( VS1053_REG_DECODETIME, 0x00 );
    sciWrite( VS1053_REG_DECODETIME, 0x00 );
    this->_amplifier.enableOutputs();
}


/*--------------------------------------------------------------------------
 *
 * Pause the alarm playback and snooze for the given amount of time in the
 * profile settings. If the snooze delay is set to 0, the alarm will stop.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::snooze() {
    if( ( this->_playMode == ALARM_MODE_OFF ) || ( this->_playMode & ALARM_MODE_SNOOZE ) ) {
        return;
    }

    /* if snooze delay is off, turn off the alarm instead. */
    if( this->profile.snoozeDelay == 0 ) {
        this->stop();
        return;
    }

    this->_playMode |= ALARM_MODE_SNOOZE;
    this->_snoozeStart = g_rtc.getEpoch();
    this->audioStop();
    this->visualStop();

    g_screenClear = true;
    g_screenUpdate = true;

    screen_alarm.resetTimeout();
    screen_alarm.setTimeout( 1000 );
}


/*--------------------------------------------------------------------------
 *
 * Resume the alarm playback from a snooze state.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::resume() {
    if( this->_playMode == ALARM_MODE_OFF || ( ( this->_playMode & ALARM_MODE_SNOOZE ) == 0 ) ) {
        return;
    }

    this->_playMode &= ~ALARM_MODE_SNOOZE;
    this->_snoozeStart = 0;
    this->_alarmStart = millis();

    this->audioStart();
    this->visualStart();

    if( this->_playMode & ALARM_MODE_SCREEN ) {
        g_screenUpdate = true;
        g_screenClear = true;
    }
}


/*--------------------------------------------------------------------------
 *
 * Set the codec volume.
 *
 * Arguments
 * ---------
 *  vol : 0-100%. 0 represents a 50 dB attenuation, 100 is 0 dB
 *
 * Returns : Nothing
 */
void Alarm::setVolume( uint8_t vol ) {
    /* 0: 50 dB attenuation, 100: Full volume */

    if( vol > 100 ) {
        vol = 100;
    }

    this->_volume = vol;

    VS1053::setVolume( 100 - vol, 100 - vol );
}


/*--------------------------------------------------------------------------
 *
 * Returns whether or not the current alarm state is snoozing.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if currently snoozingor False otherwise.
 */
bool Alarm::isSnoozing() {
    return this->_playMode & ALARM_MODE_SNOOZE;
}


/*--------------------------------------------------------------------------
 *
 * Return whether or the the alarm is currently playing.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if currently playingor False otherwise.
 */
bool Alarm::isPlaying() {
    return this->_playMode != ALARM_MODE_OFF && ( ( this->_playMode & ALARM_MODE_SNOOZE ) == 0 );
}


/*--------------------------------------------------------------------------
 *
 * Get the current alarm play mode
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Alarm play mode.
 */
uint8_t Alarm::getPlayMode() {
    return this->_playMode;
}


/*--------------------------------------------------------------------------
 *
 * Get the time remaining before the alarm playback is resumed.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : The time remaining in seconds.
 */
uint16_t Alarm::getSnoozeTimeRemaining() {
    if( ( this->_playMode & ALARM_MODE_SNOOZE ) == 0 ) {
        return 0;
    }

    if( this->profile.snoozeDelay == 0 ) {
        return 0;
    }

    return ( this->profile.snoozeDelay * 60 ) - ( g_rtc.getEpoch() - this->_snoozeStart );
}


/*--------------------------------------------------------------------------
 *
 * Starts the visual element of the alarm playback.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Alarm::visualStart() {
    /* Turn on lamp if option enabled */
    if( this->_playMode & ALARM_MODE_LAMP && profile.lamp.mode != LAMP_MODE_OFF ) {
        this->profile.lamp.delay_off = 0;
        g_lamp.activate( &this->profile.lamp );
    }

    /* Visual mode not enabled */
    if( ( this->_playMode & ALARM_MODE_VISUAL ) == 0
            || this->profile.visualMode == ALARM_VISUAL_NONE ) {
        return;
    }

    this->_visualStepReverse = false;

    switch( this->profile.visualMode ) {

        case ALARM_VISUAL_FADING:
            this->_visualStepValue = g_config.clock_brightness;
            break;

        default:
            this->_visualStepValue = 0;
            break;
    }

    this->updateVisualStepDelay();
}


/*--------------------------------------------------------------------------
 *
 * Update the visual effect animation next step delay when speed settings
 * changes.
 *
 * Arguments
 * ---------
 *  None
  *
 * Returns : Nothing
 */
inline void Alarm::updateVisualStepDelay() {

    switch( this->profile.visualMode ) {

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


/*--------------------------------------------------------------------------
 *
 * Process the next step of the visual effect animation.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
inline void Alarm::visualStep() {
    if( this->profile.visualMode == ALARM_VISUAL_NONE ) {
        return;
    }

    /* Check if visual effect is enabled */
    if( this->_visualStepDelay == 0 ) {
        return;
    }

    /* Check if the effect next step delay is elapsed */
    if( ( millis() - this->_timerStart ) < this->_visualStepDelay ) {
        return;
    }

    switch( this->profile.visualMode ) {
        case ALARM_VISUAL_FLASHING:
        case ALARM_VISUAL_RED_FLASH:

            this->_visualStepReverse = !this->_visualStepReverse;

            g_clock.setBrightness( this->_visualStepReverse ? 0 : ( g_config.clock_brightness + 25 ) );

            if( this->profile.visualMode == ALARM_VISUAL_RED_FLASH ) {
                g_clock.setColorFromTable( COLOR_RED );
            }

            break;

        case ALARM_VISUAL_WHITE_FLASH:
            this->_visualStepReverse = !this->_visualStepReverse;

            g_clock.setBrightness( g_config.clock_brightness + 25 );
            g_clock.setColorFromTable( this->_visualStepReverse ? COLOR_WHITE : g_config.clock_color );
            break;

        case ALARM_VISUAL_FADING:
            if( this->_visualStepValue
                    < ( ( g_config.clock_brightness < 25 ) ? 5 : ( g_config.clock_brightness - 20 ) ) ) {
                this->_visualStepReverse = false;
            }

            if( this->_visualStepValue > ( g_config.clock_brightness + 20 ) ) {
                this->_visualStepReverse = true;
            }

            this->_visualStepValue += ( this->_visualStepReverse ? -5 : 5 );
            g_clock.setBrightness( this->_visualStepValue );
            break;

        case ALARM_VISUAL_RAINBOW:
            g_clock.setBrightness( g_config.clock_brightness + 25 );

            this->_visualStepValue += 5;

            if( this->_visualStepValue < 85 ) {
                g_clock.setColorRGB( this->_visualStepValue * 3, 255 - this->_visualStepValue * 3, 0 );

            } else if( this->_visualStepValue < 170 ) {
                g_clock.setColorRGB( 255 - ( this->_visualStepValue - 85 ) * 3, 0,
                                     ( this->_visualStepValue - 85 ) * 3 );

            } else {
                g_clock.setColorRGB( 0, ( this->_visualStepValue - 170 ) * 3,
                                     255 - ( this->_visualStepValue - 170 ) * 3 );
            }

            break;
    }

    g_clock.update();

    this->_timerStart = millis();
    this->updateVisualStepDelay();
}


/*--------------------------------------------------------------------------
 *
 * Stops the visual element of the alarm playback.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
inline void Alarm::visualStop() {

    /* Turn off lamp if active */
    g_lamp.deactivate();

    /* Restore clock settings */
    g_clock.setColorFromTable( g_config.clock_color );
    g_clock.setBrightness( g_config.clock_brightness );
    g_clock.update();
}


/*--------------------------------------------------------------------------
 *
 * Feed alarm audio buffer and process visual effect animation
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
void Alarm::processAlarmEvents() {
    if( this->_playMode == ALARM_MODE_OFF ) {
        return;
    }

    if( this->_alarm_sw_on == false && ( ( this->_playMode & ALARM_MODE_TEST ) == 0 ) ) {
        this->stop();
        return;
    }

    if( this->_playMode & ALARM_MODE_SNOOZE ) {
        if( g_rtc.getEpoch() - this->_snoozeStart > ( this->profile.snoozeDelay * 60 ) ) {
            this->resume();
        }

        return;
    }

    if( this->_playDelay > 0 ) {
        if( millis() - this->_timerStart < this->_playDelay ) {
            return;
        }

        this->play( this->_playMode );
    }


    if( this->profile.gradual == true && ( ( this->_playMode & ALARM_MODE_TEST ) == 0 ) ) {
        uint8_t volume = ( uint8_t )( ( unsigned long )( millis() - this->_alarmStart ) / ( 45000 / this->profile.volume ) );

        if( volume > this->profile.volume ) {
            volume = this->profile.volume;
        }

        if( volume != this->_volume ) {
            this->setVolume( volume );
        }
    }


    if( this->_playMode & ALARM_MODE_AUDIO ) {
        g_power.resetSuspendDelay();
        this->feedBuffer();
    }

    if( this->_playMode & ALARM_MODE_VISUAL ) {
        this->visualStep();
    }
}


/*--------------------------------------------------------------------------
 *
 * Send data to the codec when it is ready to receive data.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
inline void Alarm::feedBuffer() {
    if( this->_playMode & ALARM_MODE_SNOOZE ) {
        return;
    }

    if( this->readyForData() == false ) {
        return;
    }

    uint8_t bytesRead;

    if( this->currentFile.isOpen() == false ) {

        /* Playback from program memory space */
        if( this->_pgm_audio_ptr + VS1053_DATABUFFERLEN > DEFAULT_ALARMSOUND_DATA_LENGTH ) {

            bytesRead = DEFAULT_ALARMSOUND_DATA_LENGTH - this->_pgm_audio_ptr;

            memcpy_P( &vs1053_buffer, &_DEFAULT_ALARMSOUND_DATA[this->_pgm_audio_ptr], bytesRead );

            this->_pgm_audio_ptr = 0;

        } else {

            bytesRead = VS1053_DATABUFFERLEN;

            memcpy_P( &vs1053_buffer, &_DEFAULT_ALARMSOUND_DATA[this->_pgm_audio_ptr],
                      VS1053_DATABUFFERLEN );

            this->_pgm_audio_ptr += VS1053_DATABUFFERLEN;
        }


    } else {
        /* Playback from file on SD card */
        bytesRead = this->currentFile.read( vs1053_buffer, VS1053_DATABUFFERLEN );

        if( bytesRead == 0 ) {

            /* Play the file in loop */
            this->currentFile.rewind();
        }
    }

    this->playData( vs1053_buffer, bytesRead );
}


/*--------------------------------------------------------------------------
 *
 * Returns whether or not the alarm switch was ON.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if alarm switch is ONor False otherwise.
 */
bool Alarm::isAlarmSwitchOn() {
    return this->_alarm_sw_on;
}


/*--------------------------------------------------------------------------
 *
 * Checks the current state of the alarm switch pin.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if alarm switch is ONor False otherwise.
 */
bool Alarm::detectAlarmSwitchState() {

    this->_alarm_sw_on = ( digitalRead( this->_pin_alarm_sw ) == HIGH );
    return this->_alarm_sw_on;
}


/*--------------------------------------------------------------------------
 *
 * Returns wether or not an alarm is set and that the alarm switch is ON.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
bool Alarm::isAlarmEnabled() {
    if( this->_alarm_sw_on == false ) {
        return false;
    }

    return ( ( g_config.alarm_on[0] == true ) || ( g_config.alarm_on[1] == true ) );
}


/*--------------------------------------------------------------------------
 *
 * Checks if the given time match an alarm. If so, load it's profile and start
 * playback.
 *
 * Arguments
 * ---------
 *  - now : Current date/time
 *
 * Returns : TRUE if an alarm matchor False otherwise
 */
bool Alarm::checkForAlarms( DateTime *now ) {
    if( this->isAlarmEnabled() == false ) {
        return false;
    }

    if( this->_playMode != ALARM_MODE_OFF && ( ( this->_playMode & ALARM_MODE_TEST ) == 0 ) ) {
        return true;
    }

    int8_t  alarm_id = this->getNextAlarmID( now, true );
    int16_t offset = this->getNextAlarmOffset( alarm_id, now, true );

    if( offset != 0 ) {
        return false;
    }

    this->loadProfile( alarm_id );
    this->play( ALARM_MODE_NORMAL );
    return true;
}


/*--------------------------------------------------------------------------
 *
 * Get the alarm profile ID which is set to occur next.
 *
 * Arguments
 * ---------
 *  - currentTime : DateTime structure containing the current time.
 *  - matchNow    : TRUE to include alarm which match the current time 
 *                  or FALSE to ignore 
 *
 * Returns : The next alarm ID or -1 if no alarm are set.
 */
int8_t Alarm::getNextAlarmID( DateTime *currentTime, bool matchNow ) {
    if( this->isAlarmEnabled() == false ) {
        /* All alarms off */
        return -1;
    }

    if( g_config.alarm_on[1] == true ) {
        if( g_config.alarm_on[0] == false ) {

            /* Alarm 0 is not active, so alarm 1 is always next */
            return 1;
        }

        /* Compare the time offset from both alarms. */
        if( this->getNextAlarmOffset( 1, currentTime, matchNow )
                < this->getNextAlarmOffset( 0, currentTime, matchNow ) ) {
            /* Alarm 1 is sooner than alarm 0 */
            return 1;
        }
    }

    /* Alarm 0 is sooner, or is the only alarm active */
    return 0;
}


/*--------------------------------------------------------------------------
 *
 * Get the delay in minutes until the next alarm is set to occur.
 *
 * Arguments
 * ---------
 *  - currentTime : DateTime structure containing the current time.
 *  - matchNow    : TRUE to include alarm which match the current time 
 *                  or FALSE to ignore 
 *
 * Returns : The delay in minutes or -1 if no alarm is set.
 */
int16_t Alarm::getNextAlarmOffset( int8_t alarm_id, DateTime *currentTime, bool matchNow ) {
    if( alarm_id > MAX_ALARM_PROFILES - 1 ) {
        return -1;
    }

    Time    profile_time;
    uint8_t profile_dow;

    if( g_alarm.readProfileAlarmTime( alarm_id, &profile_time, &profile_dow ) == false ) {
        return -1;
    }

    uint8_t a_hour = profile_time.hour;
    uint8_t a_min = profile_time.minute;
    uint8_t a_dow = currentTime->dow();
    uint8_t a_dayOffset = 0;

    /* No days selected, considering alarm off */
    if( profile_dow == 0x00 ) {
        return -1;
    }

    /* Find out on which day the next alarm occurs */
    while( a_dayOffset < 7 ) {
        if( a_dow > 7 ) {
            a_dow = 1;
        }

        if( bitRead( profile_dow, a_dow - 1 ) ) {
            /* Next alarm is on a different day than the current one. */
            if( a_dow != currentTime->dow() ) {
                break;

            } else {
                /* Next alarm is now */
                if( matchNow && a_hour == currentTime->hour() && a_min == currentTime->minute() ) {
                    break;
                }

                /* Check if the alarm time is equal or greater than the current time, if not,
                   continue searching */
                if( a_hour > currentTime->hour()
                        || ( a_hour == currentTime->hour() && a_min > currentTime->minute() ) ) {
                    break;
                }
            }
        }

        a_dow++;
        a_dayOffset++;
    }

    return ( a_dayOffset * 1440 ) + ( ( a_hour - currentTime->hour() ) * 60 )
           + ( a_min - currentTime->minute() );
}