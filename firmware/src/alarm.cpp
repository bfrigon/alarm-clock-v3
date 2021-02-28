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
#include "ui/screen.h"
#include "ui/ui.h"


uint8_t vs1053_buffer[VS1053_DATA_BLOCK_SIZE];


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 * @param   pin_cs           Codec chip select pin
 * @param   pin_xdcs         Codec data select pin
 * @param   pin_dreq         Codec data request pin.
 * @param   pin_reset        Codec reset pin.
 * @param   pin_sd_cs        SD card chip select pin.
 * @param   pin_sd_detect    SD card detect pin.
 * @param   pin_alarm_sw     Pin connected to the alarm switch.
 * @param   pin_amp_shdn     Amplifier shutdown pin.
 * 
 */
Alarm::Alarm( int8_t pin_reset, int8_t pin_cs, int8_t pin_xdcs, int8_t pin_dreq, int8_t pin_sd_cs, int8_t pin_sd_detect,
              int8_t pin_alarm_sw, int8_t pin_amp_shdn ) : VS1053( pin_cs, pin_xdcs, pin_dreq, pin_reset ) {

    _pin_sd_cs = pin_sd_cs;

    _pin_sd_detect = pin_sd_detect;
    pinMode( pin_sd_detect, INPUT );

    _pin_alarm_sw = pin_alarm_sw;
    pinMode( pin_alarm_sw, INPUT );

    _amplifier.setPins( pin_amp_shdn );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the codec and amplifier.
 * 
 */
void Alarm::begin() {
    if( _init == true ) {
        return;
    }

    _init = true;
    _volume = 0;

    this->onPowerStateChange( g_power.getPowerMode() );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Power down the amplifier and codec.
 * 
 */
void Alarm::end() {
    if( _init == false ) {
        return;
    }

    _init = false;

    _amplifier.end();
    delay( 50 );

    VS1053::end();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the TPA2016 amplifier 
 * 
 */
void Alarm::initAmplifier() {
    _amplifier.begin();

    _amplifier.setCompression( TPA2016_COMPRESSION_2_1 );
    _amplifier.setFixedGain( -3 );
    _amplifier.setMaxGain( 30 );
    _amplifier.setLimiter( false, 17 );   /* +2dBV */
    _amplifier.setAttackTime( 0 );
    _amplifier.setReleaseTime( 0 );
    _amplifier.setHoldTime( 0 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Power up or down the codec and amplifier based on the 
 *          current power mode
 *
 * @param   state    Current power state 
 * 
 */
void Alarm::onPowerStateChange( uint8_t state ) {

    if( _init == false ) {
        return;
    }

    if( state != POWER_MODE_SUSPEND ) {
        VS1053::begin();

        /* Initialize the TPA2017 amplifier */
        this->initAmplifier();
        
        delay( 50 );

        _amplifier.dumpRegs();

    } else {
        _amplifier.end();
        delay( 50 );

        VS1053::end();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Return whether the SD card was detected or not.
 *
 * @return  TRUE if detected, FALSE otherwise.
 * 
 */
bool Alarm::isSDCardPresent() {
    return _sd_present;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check the current state of the SD card detect pin. If a card 
 *          is found, initialize the SdFat library.
 *
 * @return  TRUE if detected, FALSE otherwise.
 * 
 */
bool Alarm::detectSDCard() {
    if( digitalRead( _pin_sd_detect ) == LOW ) {

        /* Already detected, no need to re-initialize */
        if( _sd_present == true ) {
            return true;
        }

        if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
            if( _timerStart == 0 ) {
                /* millis() not usable in suspend mode, debounce using wake-up counter */
                _timerStart = 1;
                return false;
            }

        } else {
            /* Start the debouce timer using millis() */
            if( _timerStart == 0 ) {
                _timerStart = millis();
                return false;
            }

            /* Still wait fot the detect card signal to make up it's mind. */
            if( ( millis() - _timerStart ) < 1000 ) {
                return false;
            }
        }

        /* The card was detected, now we can initialize or re-initalize it */
        _sd_present = true;

    } else {
        /* Already not present, do nothing */
        if( _sd_present == false ) {
            return false;
        }

        if( _playMode != ALARM_MODE_OFF ) {
            if( _playMode & ALARM_MODE_TEST ) {
                this->stop();

            } else {
                /* If SD card is removed during an alarm, don't stop the alarm.
                   Fallback sound will be played once the snooze delay is elapsed. */
                this->snooze();
            }
        }

        this->currentFile.close();
        _timerStart = 0;
        _sd_present = false;
        return false;
    }

    _sd_present = false;
    _timerStart = 0;
    //Serial.println( F( "Init SD card... " ) );

    if( _sd.begin( _pin_sd_cs ) == false ) {
        //Serial.println( F( "SD init failed " ) );
        return false;
    }

    /* Open root directory */
    _sd.vwd()->rewind();

    if( _sd.vwd()->isOpen() == false ) {
        //Serial.println( F( "Cannot open root directory on SD card" ) );
        return false;
    }

    _sd_present = true;
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Open the next music file in the SD card root directory. 
 * 
 * @details If the end of the directory was reached, it will select the 
 *          fallback file stored in program memory. The next time 
 *          openNextFile is called, it will select the first file.
 *
 *          If the card was removed or a read error occurs, it will use 
 *          the fallback file instead.
 *
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool Alarm::openNextFile() {
    if( _playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    if( this->openFile( NULL ) == false ) {
        this->profile.filename[0] = 0;
        return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Open a specific music file on the SD card located in the root directory.
 *
 * @details If the file is not found or card was removed, it will use the fallback
 *          file instead.
 *
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool Alarm::openFile( char* name ) {
    if( _sd.vwd()->isOpen() == false ) {
        return false;
    }

    /* Close current file if open */
    if( this->currentFile.isOpen() ) {
        this->currentFile.close();
    }

    if( name != NULL ) {

        /* Open the specified file */
        if( strlen( name ) > 0 ) {
            this->currentFile.open( _sd.vwd(), name, O_READ );

            this->currentFile.getSFN( this->profile.filename );
        }

    } else {
        /* Open the next file in the root directory */
        this->currentFile.openNext( _sd.vwd(), O_READ );

        char buffer[ MAX_LENGTH_ALARM_FILENAME + 1 ];
        this->currentFile.getSFN( buffer );

        /* If the next filename is the same than the currently slected one, go
           to the next file */
        if( strcmp( this->profile.filename, buffer )
                == 0 ) {
            this->currentFile.openNext( _sd.vwd(), O_READ );
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
        this->currentFile.openNext( _sd.vwd(), O_READ );
        this->currentFile.getSFN( this->profile.filename );
    }

    if( this->currentFile.isOpen() == false ) {
        _sd.vwd()->rewind();
        return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Read the alarm time for a given profile ID
 *
 * @param   profile_id    Profile to read the time from.
 * @param   time          Pointer to a Time structure where the alarm time will be
 *                        written.
 * @param   dow           Pointer to a unsigned integer where the day of week mask
 *                        will be copied.
 *
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool Alarm::readProfileAlarmTime( uint8_t profile_id, Time* time, uint8_t* dow ) {
    uint8_t i;
    uint8_t byte;

    if( profile_id > MAX_NUM_PROFILES - 1 ) {
        return false;
    }

    if( time != NULL ) {
        for( i = 0; i < sizeof( Time ); i++ ) {
            byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( profile_id * sizeof( AlarmProfile ) ) + i
                                + offsetof( struct AlarmProfile, time ) );


            * ( ( ( uint8_t* ) time ) + i ) = byte;
        }
    }

    if( dow != NULL ) {
        byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( profile_id * sizeof( AlarmProfile ) )
                            + offsetof( struct AlarmProfile, dow ) );


        *dow = byte;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Load the given alarm profile from EEPROM
 *
 * @param   id    Alarm profile ID to load.
 *
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool Alarm::loadProfile( uint8_t id ) {
    return this->loadProfile( &this->profile, id );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Load the given alarm profile from EEPROM into an 
 *          AlarmProfile structure.
 *
 * @param   profile    Pointer to the AlarmProfile structure to write to.
 * @param   id         Alarm profile ID to load.
 *
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool Alarm::loadProfile( AlarmProfile* profile, uint8_t id ) {
    if( id > MAX_NUM_PROFILES - 1 ) {
        return false;
    }

    for( uint8_t i = 0; i < sizeof( AlarmProfile ); i++ ) {
        uint8_t byte = EEPROM.read( EEPROM_ADDR_PROFILES + ( id * sizeof( AlarmProfile ) ) + i );
        * ( ( ( uint8_t* ) profile ) + i ) = byte;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Store the current profile settings in EEPROM.
 *
 * @param   id    Alarm profile ID to save.
 * 
 */
void Alarm::saveProfile( uint8_t id ) {
    this->saveProfile( &this->profile, id );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Store alarm profile settings contained in a given AlarmProfile 
 *          structure to EEPROM
 *
 * @param   profile    AlarmProfile structure containing the settings.
 * @param   id         Profile ID to save the settings to.
 *
 */
void Alarm::saveProfile( AlarmProfile* profile, uint8_t id ) {
    if( id > MAX_NUM_PROFILES - 1 ) {
        return;
    }

    for( uint8_t i = 0; i < sizeof( AlarmProfile ); i++ ) {
        uint8_t byte = * ( ( ( uint8_t* ) profile ) + i );
        EEPROM.update( EEPROM_ADDR_PROFILES + ( id * sizeof( AlarmProfile ) ) + i, byte );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Activate the alarm with a start delay.
 *
 * @param   mode     Alarm play mode.
 * @param   delay    Delay in ms to wait before activating alarm.
 * 
 */
void Alarm::play( uint8_t mode, uint16_t delay ) {
    if( _playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    _playMode = mode;
    _timerStart = millis();
    _playDelay = delay;
    _snoozeStart = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Activate the alarm immediately.
 *
 * @param   mode    Alarm play mode.
 * 
 */
void Alarm::play( uint8_t mode ) {
    if( _playMode != ALARM_MODE_OFF ) {
        this->stop();
    }

    this->detectAlarmSwitchState();

    if( _alarm_sw_on == false && ( ( mode & ALARM_MODE_TEST ) == 0 ) ) {
        return;
    }

    _playMode = mode;
    _timerStart = 0;
    _playDelay = 0;
    _snoozeStart = 0;
    _alarmStart = millis();

    if( mode & ALARM_MODE_SCREEN ) {
        g_screen.activate( &screen_alarm );
    }

    if( mode & ALARM_MODE_AUDIO ) {
        this->audioStart();
    }

    if( mode & ALARM_MODE_VISUAL || mode & ALARM_MODE_LAMP ) {
        this->visualStart();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Stops the alarm.
 * 
 */
void Alarm::stop() {
    if( _playMode == ALARM_MODE_OFF ) {
        return;
    }

    if( _playMode & ALARM_MODE_AUDIO ) {
        this->audioStop();
    }

    if( _playMode & ALARM_MODE_VISUAL || _playMode & ALARM_MODE_LAMP ) {
        this->visualStop();
    }

    if( _playMode & ALARM_MODE_SCREEN ) {
        g_screen.exitScreen();
    }

    _playMode = ALARM_MODE_OFF;
    _timerStart = 0;
    _snoozeStart = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Stop the audio element of the alarm.
 * 
 */
void Alarm::audioStop() {
    _amplifier.disableOutputs();

    if( _playMode & ALARM_MODE_AUDIO ) {

        // cancel all playback
        sciWrite( VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_CANCEL );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Start the audio element of the alarm.
 * 
 */
void Alarm::audioStart() {
    if( ( _playMode & ALARM_MODE_AUDIO ) == 0 ) {
        return;
    }

    if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
        g_power.setPowerMode( POWER_MODE_LOW_POWER );
    }

    if( this->profile.gradual == true && ( ( _playMode & ALARM_MODE_TEST ) == 0 ) ) {
        this->setVolume( 0 );

    } else {
        this->setVolume( this->profile.volume );
    }

    this->openFile( this->profile.filename );

    if( this->currentFile.isOpen() == false ) {
        _pgm_audio_ptr = 0;

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
    _amplifier.enableOutputs();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Pause the alarm playback and snooze for the given amount of 
 *          time in the profile settings. 
 * 
 * @details If the snooze delay is set to 0, the alarm will stop.
 * 
 */
void Alarm::snooze() {
    if( ( _playMode == ALARM_MODE_OFF ) || ( _playMode & ALARM_MODE_SNOOZE ) ) {
        return;
    }

    /* if snooze delay is off, turn off the alarm instead. */
    if( this->profile.snoozeDelay == 0 ) {
        this->stop();
        return;
    }

    _playMode |= ALARM_MODE_SNOOZE;
    _snoozeStart = g_rtc.getEpoch();
    this->audioStop();
    this->visualStop();

    g_screen.requestScreenUpdate( true );

    g_screen.resetTimeout();
    g_screen.setTimeout( 1000 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Resume the alarm playback from a snooze state.
 * 
 */
void Alarm::resume() {
    if( _playMode == ALARM_MODE_OFF || ( ( _playMode & ALARM_MODE_SNOOZE ) == 0 ) ) {
        return;
    }

    _playMode &= ~ALARM_MODE_SNOOZE;
    _snoozeStart = 0;
    _alarmStart = millis();

    this->audioStart();
    this->visualStart();

    if( _playMode & ALARM_MODE_SCREEN ) {
        g_screen.requestScreenUpdate( true );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the codec volume.
 *
 * @param   vol     0 represents a 50 dB attenuation, 100 is 0 dB
 * 
 */
void Alarm::setVolume( uint8_t vol ) {
    /* 0: 50 dB attenuation, 100: Full volume */

    if( vol > 100 ) {
        vol = 100;
    }

    _volume = vol;
    VS1053::setVolume( 100 - vol, 100 - vol );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns whether or not the current alarm state is snoozing.
 *
 * @return  TRUE if currently snoozing, FALSE otherwise.
 * 
 */
bool Alarm::isSnoozing() {
    return _playMode & ALARM_MODE_SNOOZE;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Return whether or the the alarm is currently playing.
 *
 * @return  TRUE if currently playing, FALSE otherwise.
 * 
 */
bool Alarm::isPlaying() {
    return _playMode != ALARM_MODE_OFF && ( ( _playMode & ALARM_MODE_SNOOZE ) == 0 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the current alarm play mode
 *
 * @return  Alarm play mode.
 * 
 */
uint8_t Alarm::getPlayMode() {
    return _playMode;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the time remaining before the alarm playback is resumed.
 *
 * @return  The time remaining in seconds.
 * 
 */
uint16_t Alarm::getSnoozeTimeRemaining() {
    if( ( _playMode & ALARM_MODE_SNOOZE ) == 0 ) {
        return 0;
    }

    if( this->profile.snoozeDelay == 0 ) {
        return 0;
    }

    return ( this->profile.snoozeDelay * 60 ) - ( g_rtc.getEpoch() - _snoozeStart );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Starts the visual element of the alarm playback.
 * 
 */
void Alarm::visualStart() {

    /* Turn on lamp if option enabled */
    if( _playMode & ALARM_MODE_LAMP && profile.lamp.mode != LAMP_MODE_OFF ) {
        this->profile.lamp.delay_off = 0;
        g_lamp.deactivate( true );
        g_lamp.activate( &this->profile.lamp );
    }

    /* Visual mode not enabled */
    if( ( _playMode & ALARM_MODE_VISUAL ) == 0
            || this->profile.visualMode == ALARM_VISUAL_NONE ) {
        return;
    }

    _visualStepReverse = false;

    switch( this->profile.visualMode ) {

        case ALARM_VISUAL_FADING:
            _visualStepValue = g_config.clock.clock_brightness;
            break;

        default:
            _visualStepValue = 0;
            break;
    }

    this->updateVisualStepDelay();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Update the visual effect animation next step delay when speed 
 *          settings changes.
 *
 */
inline void Alarm::updateVisualStepDelay() {

    switch( this->profile.visualMode ) {

        case ALARM_VISUAL_FADING:
        case ALARM_VISUAL_RAINBOW:
            _visualStepDelay = 250 / this->profile.effectSpeed;
            break;

        case ALARM_VISUAL_NONE:
            _visualStepDelay = 0;
            break;

        default:
            _visualStepDelay = 2000 / this->profile.effectSpeed;
            break;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Process the next step of the visual effect animation.
 *
 */
inline void Alarm::visualStep() {
    if( this->profile.visualMode == ALARM_VISUAL_NONE ) {
        return;
    }

    /* Check if visual effect is enabled */
    if( _visualStepDelay == 0 ) {
        return;
    }

    /* Check if the effect next step delay is elapsed */
    if( ( millis() - _timerStart ) < _visualStepDelay ) {
        return;
    }

    switch( this->profile.visualMode ) {
        case ALARM_VISUAL_FLASHING:
        case ALARM_VISUAL_RED_FLASH:

            _visualStepReverse = !_visualStepReverse;

            g_clock.setBrightness( _visualStepReverse ? 0 : ( g_config.clock.clock_brightness + 25 ) );

            if( this->profile.visualMode == ALARM_VISUAL_RED_FLASH ) {
                g_clock.setColorFromTable( COLOR_RED );
            }

            break;

        case ALARM_VISUAL_WHITE_FLASH:
            _visualStepReverse = !_visualStepReverse;

            g_clock.setBrightness( g_config.clock.clock_brightness + 25 );
            g_clock.setColorFromTable( _visualStepReverse ? COLOR_WHITE : g_config.clock.clock_color );
            break;

        case ALARM_VISUAL_FADING:
            if( _visualStepValue
                    < ( ( g_config.clock.clock_brightness < 25 ) ? 5 : ( g_config.clock.clock_brightness - 20 ) ) ) {
                _visualStepReverse = false;
            }

            if( _visualStepValue > ( g_config.clock.clock_brightness + 20 ) ) {
                _visualStepReverse = true;
            }

            _visualStepValue += ( _visualStepReverse ? -5 : 5 );
            g_clock.setBrightness( _visualStepValue );
            break;

        case ALARM_VISUAL_RAINBOW:
            g_clock.setBrightness( g_config.clock.clock_brightness + 25 );

            _visualStepValue += 5;

            if( _visualStepValue < 85 ) {
                g_clock.setColorRGB( _visualStepValue * 3, 255 - _visualStepValue * 3, 0 );

            } else if( _visualStepValue < 170 ) {
                g_clock.setColorRGB( 255 - ( _visualStepValue - 85 ) * 3, 0,
                                     ( _visualStepValue - 85 ) * 3 );

            } else {
                g_clock.setColorRGB( 0, ( _visualStepValue - 170 ) * 3,
                                     255 - ( _visualStepValue - 170 ) * 3 );
            }

            break;
    }

    g_clock.update();

    _timerStart = millis();
    this->updateVisualStepDelay();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Stops the visual element of the alarm playback.
 * 
 */
inline void Alarm::visualStop() {

    /* Turn off lamp if active */
    g_lamp.deactivate();

    /* Restore clock settings */
    g_clock.setColorFromTable( g_config.clock.clock_color );
    g_clock.setBrightness( g_config.clock.clock_brightness );
    g_clock.update();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Feed alarm audio buffer and process visual effect animation
 *
 */
void Alarm::processEvents() {

    /* Detect if the SD card is present, if so, initialize it */
    if( _sd_present != this->detectSDCard() ) {
        g_screen.requestScreenUpdate( false );
    }

    /* Detect alarm switch state */
    if( _alarm_sw_on != this->detectAlarmSwitchState() ) {
        g_power.resetSuspendDelay();

        g_clock.requestClockUpdate( true );

        if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
            g_screen.requestScreenUpdate( false );
        }
    }

    /* If time has changed, checks for alarms */
    if( g_rtc.now()->minute() != _rtcmin ) {
        _rtcmin = g_rtc.now()->minute();

        DateTime local;
        local = g_rtc.now();
        g_timezone.toLocal( &local );

        this->checkForAlarms( &local );
    }


    if( _playMode == ALARM_MODE_OFF ) {
        return;
    }

    if( _alarm_sw_on == false && ( ( _playMode & ALARM_MODE_TEST ) == 0 ) ) {
        this->stop();
        return;
    }

    if( _playMode & ALARM_MODE_SNOOZE ) {
        if( g_rtc.getEpoch() - _snoozeStart > ( this->profile.snoozeDelay * 60 ) ) {
            this->resume();
        }

        return;
    }

    if( _playDelay > 0 ) {
        if( millis() - _timerStart < _playDelay ) {
            return;
        }

        this->play( _playMode );
    }


    if( this->profile.gradual == true && ( ( _playMode & ALARM_MODE_TEST ) == 0 ) ) {
        uint8_t volume = ( uint8_t )( ( unsigned long )( millis() - _alarmStart ) / ( 45000 / this->profile.volume ) );

        if( volume > this->profile.volume ) {
            volume = this->profile.volume;
        }

        if( volume != _volume ) {
            this->setVolume( volume );
        }
    }


    if( _playMode & ALARM_MODE_AUDIO ) {
        g_power.resetSuspendDelay();
        this->feedBuffer();
    }

    if( _playMode & ALARM_MODE_VISUAL ) {
        this->visualStep();
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Send data to the codec when it is ready to receive data.
 *
 */
inline void Alarm::feedBuffer() {
    if( _playMode & ALARM_MODE_SNOOZE ) {
        return;
    }
    
    size_t bytesRead;

    uint8_t blocks = VS1053_BLOCKS_PER_RUN;
    while( blocks-- > 0 ) {

        /* Check if the codec is ready to receive the next block */
        if( this->readyForData() == false ) {
            break;
        }
    
        if( this->currentFile.isOpen() == false ) {

            /* Playback from program memory space */
            if( _pgm_audio_ptr + VS1053_DATA_BLOCK_SIZE > DEFAULT_ALARMSOUND_DATA_LENGTH ) {

                bytesRead = DEFAULT_ALARMSOUND_DATA_LENGTH - _pgm_audio_ptr;

                memcpy_P( &vs1053_buffer, &_DEFAULT_ALARMSOUND_DATA[_pgm_audio_ptr], bytesRead );

                _pgm_audio_ptr = 0;

            } else {

                bytesRead = VS1053_DATA_BLOCK_SIZE;

                memcpy_P( &vs1053_buffer, &_DEFAULT_ALARMSOUND_DATA[_pgm_audio_ptr],
                        VS1053_DATA_BLOCK_SIZE );

                _pgm_audio_ptr += VS1053_DATA_BLOCK_SIZE;
            }


        } else {
            /* Playback from file on SD card */
            bytesRead = this->currentFile.read( vs1053_buffer, VS1053_DATA_BLOCK_SIZE );

            if( bytesRead == 0 ) {

                /* Play the file in loop */
                this->currentFile.rewind();

            }
        }

        this->playData( vs1053_buffer, bytesRead );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns whether or not the alarm switch was ON.
 *
 * @return  TRUE if alarm switch is ON, FALSE otherwise.
 * 
 */
bool Alarm::isAlarmSwitchOn() {
    return _alarm_sw_on;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Checks the current state of the alarm switch pin.
 *
 * @return  TRUE if alarm switch is ONor False otherwise.
 * 
 */
bool Alarm::detectAlarmSwitchState() {

    _alarm_sw_on = ( digitalRead( _pin_alarm_sw ) == HIGH );
    return _alarm_sw_on;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns wether or not an alarm is set and that the alarm 
 *          switch is ON.
 *
 */
bool Alarm::isAlarmEnabled() {
    if( _alarm_sw_on == false ) {
        return false;
    }

    return ( ( g_config.clock.alarm_on[0] == true ) || ( g_config.clock.alarm_on[1] == true ) );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Checks if the given time match an alarm. If so, load it's profile 
 *          and start playback.
 *
 * @param   now    Current date/time
 *
 * @return  TRUE if an alarm match, FALSE otherwise
 * 
 */
bool Alarm::checkForAlarms( DateTime* now ) {
    if( this->isAlarmEnabled() == false ) {
        return false;
    }

    if( _playMode != ALARM_MODE_OFF && ( ( _playMode & ALARM_MODE_TEST ) == 0 ) ) {
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the alarm profile ID which is set to occur next.
 *
 * @param   currentTime    DateTime structure containing the current time.
 * @param   matchNow       TRUE to include alarm which match the current time
 *                         or FALSE to ignore
 *
 * @return  The next alarm ID or -1 if no alarm are set.
 * 
 */
int8_t Alarm::getNextAlarmID( DateTime* currentTime, bool matchNow ) {
    if( this->isAlarmEnabled() == false ) {
        /* All alarms off */
        return -1;
    }

    if( g_config.clock.alarm_on[1] == true ) {
        if( g_config.clock.alarm_on[0] == false ) {

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


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the delay in minutes until the next alarm is set to occur.
 *
 * @param   currentTime    DateTime structure containing the current time.
 * @param   matchNow       TRUE to include alarm which match the current time
 *                         or FALSE to ignore
 *
 * @return  The delay in minutes or -1 if no alarm is set.
 * 
 */
int16_t Alarm::getNextAlarmOffset( int8_t alarm_id, DateTime* currentTime, bool matchNow ) {
    if( alarm_id > MAX_NUM_PROFILES - 1 ) {
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
        if( a_dow > 6 ) {
            a_dow = 0;
        }

        if( bitRead( profile_dow, a_dow ) ) {
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