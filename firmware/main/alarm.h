//******************************************************************************
//
// Project : Alarm Clock V3
// File    : alarm.h
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

#include <Adafruit_VS1053.h>
#include "time.h"
#include "screen.h"



#define MAX_ALARM_PROFILES          10
#define ALARM_PROFILE_NAME_LENGTH   11
#define ALARM_FILENAME_LENGTH       40
#define ALARM_MESSAGE_LENGTH        16


#define SELECT_FILE_PLAY_DELAY      750 /* ms */

#define ALARM_MODE_OFF              0x00
#define ALARM_MODE_AUDIO            0x01
#define ALARM_MODE_VISUAL           0x02
#define ALARM_MODE_TEST             0x04
#define ALARM_MODE_SCREEN           0x08
#define ALARM_MODE_SNOOZE           0x10

#define ALARM_MODE_NORMAL           ALARM_MODE_AUDIO | ALARM_MODE_VISUAL | ALARM_MODE_SCREEN
#define ALARM_MODE_TEST_AUDIO       ALARM_MODE_AUDIO | ALARM_MODE_TEST
#define ALARM_MODE_TEST_VISUAL      ALARM_MODE_VISUAL | ALARM_MODE_TEST
#define ALARM_MODE_TEST_PROFILE     ALARM_MODE_NORMAL | ALARM_MODE_TEST

#define ALARM_VISUAL_NONE           0
#define ALARM_VISUAL_FLASHING       1
#define ALARM_VISUAL_FADING         2
#define ALARM_VISUAL_RAINBOW        3
#define ALARM_VISUAL_WHITE_FLASH    4
#define ALARM_VISUAL_RED_FLASH      5



#define SCREEN_ID_ALARM             15


bool alarmScreen_eventEnterScreen( Screen *currentScreen );
bool alarmScreen_eventExitScreen( Screen *currentScreen, Screen *newScreen );
bool alarmScreen_eventKeypress( Screen *screen, uint8_t key );
bool alarmScreen_eventDrawScreen( Screen *screen );
void alarmScreen_eventTimeout( Screen *screen );

struct AlarmProfile {
    char name[ ALARM_PROFILE_NAME_LENGTH + 1 ];
    char filename[ ALARM_FILENAME_LENGTH + 1 ];
    char message[ ALARM_MESSAGE_LENGTH + 1 ];
    uint8_t snoozeDelay;
    uint8_t volume;
    bool gradual;
    uint8_t visualMode;
    Time time;
    uint8_t dow = 0x7F;
};


class Alarm : private Adafruit_VS1053 {

  public:


    Alarm( int8_t pin_reset, int8_t pin_cs, int8_t pin_xdcs, int8_t pin_dreq, int8_t pin_sd_cs, int8_t pin_sd_detect );

    void begin();
    bool loadProfile( AlarmProfile *profile, uint8_t id );
    bool loadProfile( uint8_t id );
    void saveProfile( AlarmProfile *profile, uint8_t id );
    void saveProfile( uint8_t id );
    uint8_t readProfileName( uint8_t id, char *buffer );
    bool readProfileAlarmTime( uint8_t id, Time *time, uint8_t *dow );
    
    bool DetectSDCard();
    bool isSDCardPresent();
    bool initSDCard();
    bool openNextFile();
    bool openFile( char *name );
    
    void setVolume( uint8_t vol );
    void play( uint8_t mode );
    void play( uint8_t mode, uint16_t delay );
    void stop();
    void snooze();
    uint16_t getSnoozeTimeRemaining();
    void resume();

    void processAlarm();
    bool checkForAlarms( DateTime *now );
    int8_t getNextAlarmID( DateTime *currentTime, bool matchNow );
    int16_t getNextAlarmOffset( int8_t profile_id, DateTime *currentTime, bool matchNow );

    bool isSnoozing();
    bool isPlaying();
    uint8_t getPlayMode();
    bool isAlarmEnabled();
    AlarmProfile profile;
    File currentFile;

  private:

    void feedBuffer();
    void visualStart();
    void visualStep();
    void visualStop();
    void audioStop();
    void audioStart();

    uint8_t _pin_sd_detect;
    uint8_t _pin_sd_cs;

    unsigned long _timerStart = 0;
    unsigned long _snoozeStart = 0;
    uint16_t _playDelay = 0;
    uint16_t _visualStepDelay = 0;
    uint8_t _visualStepValue = 0;
    bool _visualStepReverse = false;

    bool _sd_present = false;
    uint8_t _playMode = 0;

    uint16_t _pgm_audio_ptr = 0;
    File _sd_root;
};

extern Alarm g_alarm;

#endif /* ALARM_H */
