//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/alarm.h
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
#ifndef ALARM_H
#define ALARM_H


#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <avr/pgmspace.h>


#include "libs/time.h"
#include "libs/task.h"
#include "hardware.h"
#include "resources.h"
#include "config.h"



#define SELECT_FILE_PLAY_DELAY      750 /* ms */

#define ALARM_MODE_OFF              0x00
#define ALARM_MODE_AUDIO            0x01
#define ALARM_MODE_VISUAL           0x02
#define ALARM_MODE_SCREEN           0x04
#define ALARM_MODE_SNOOZE           0x08
#define ALARM_MODE_LAMP             0x10
#define ALARM_MODE_TEST             0x80

#define ALARM_MODE_NORMAL           ALARM_MODE_AUDIO | ALARM_MODE_VISUAL | ALARM_MODE_LAMP | ALARM_MODE_SCREEN
#define ALARM_MODE_TEST_AUDIO       ALARM_MODE_TEST | ALARM_MODE_AUDIO
#define ALARM_MODE_TEST_VISUAL      ALARM_MODE_TEST | ALARM_MODE_VISUAL
#define ALARM_MODE_TEST_LAMP        ALARM_MODE_TEST | ALARM_MODE_LAMP
#define ALARM_MODE_TEST_PROFILE     ALARM_MODE_TEST | ALARM_MODE_NORMAL

#define ALARM_VISUAL_NONE           0
#define ALARM_VISUAL_FLASHING       1
#define ALARM_VISUAL_FADING         2
#define ALARM_VISUAL_RAINBOW        3
#define ALARM_VISUAL_WHITE_FLASH    4
#define ALARM_VISUAL_RED_FLASH      5




class Alarm : private VS1053, public Task {

  public:

    Alarm( int8_t pin_reset, int8_t pin_cs, int8_t pin_xdcs, int8_t pin_dreq, int8_t pin_sd_cs, int8_t pin_sd_detect,
           int8_t pin_alarm_sw, int8_t pin_amp_shdn );

    uint8_t begin();
    void end();
    void updatePowerState();

    bool loadProfile( struct AlarmProfile *profile, uint8_t id );
    bool loadProfile( uint8_t id );
    void saveProfile( struct AlarmProfile *profile, uint8_t id );
    void saveProfile( uint8_t id );
    bool readProfileAlarmTime( uint8_t id, Time *time, uint8_t *dow );

    bool detectSDCard();
    bool isSDCardPresent();
    bool openNextFile();
    bool openFile( char *name );
    bool fileExists( char* filename );

    void setVolume( uint8_t vol );
    void play( uint8_t mode );
    void play( uint8_t mode, uint16_t delay );
    void stop();
    void snooze();
    uint16_t getSnoozeTimeRemaining();
    void resume();

    bool isAlarmSwitchOn();
    bool detectAlarmSwitchState();

    void runTask();
    bool checkForAlarms( DateTime *now );
    int8_t getNextAlarmID( DateTime *currentTime, bool matchNow );
    int16_t getNextAlarmOffset( int8_t profile_id, DateTime *currentTime, bool matchNow );

    bool isSnoozing();
    bool isPlaying();
    uint8_t getPlayMode();
    bool isAlarmEnabled();
    struct AlarmProfile profile;
    FatFile currentFile;
    
  private:

    void feedBuffer();
    void visualStart();
    void visualStep();
    void visualStop();
    void audioStop();
    void audioStart();
    inline void Alarm::updateVisualStepDelay();

    uint8_t _pin_sd_detect;
    uint8_t _pin_sd_cs;
    uint8_t _pin_alarm_sw;
    bool _init = false;

    int8_t _rtcmin = -1;


    unsigned long _timerStart = 0;
    unsigned long _alarmStart = 0;
    unsigned long _snoozeStart = 0;
    uint16_t _playDelay = 0;
    uint16_t _visualStepDelay = 0;
    uint8_t _visualStepValue = 0;
    bool _visualStepReverse = false;

    bool _sd_present = false;
    bool _alarm_sw_on = false;
    uint8_t _playMode = ALARM_MODE_OFF;
    uint8_t _volume = 0;

    uint16_t _pgm_audio_ptr = 0;

    SdFat _sd;
    TPA2016 _amplifier;
};

extern Alarm g_alarm;

#endif /* ALARM_H */
