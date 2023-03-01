//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui.h
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
#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <tzdata.h>
#include <config.h>
#include <alarm.h>
#include <resources.h>
#include "screen.h"



// ----------------------------------------
// Screen item id's
// ----------------------------------------
enum {
    /* --- Main menu --- */
    ID_MAIN_SET_ALARMS = 1,
    ID_MAIN_SET_TIME,
    ID_MAIN_LIST_PROFILES,
    ID_MAIN_EDIT_DISPLAY,
    ID_MAIN_EDIT_LAMP,
    ID_MAIN_SELECT_TIMEZONE,
    ID_MAIN_EDIT_NETWORK,
    ID_MAIN_SERVICES,
    ID_MAIN_SETTINGS,

    /* --- Select timezone screen --- */
    ID_TZ_REGION_AFRICA,
    ID_TZ_REGION_ANTARCTICA,
    ID_TZ_REGION_ARCTIC_OCEAN,
    ID_TZ_REGION_ASIA,
    ID_TZ_REGION_ATLANTIC_OCEAN,
    ID_TZ_REGION_AUSTRALIA,
    ID_TZ_REGION_CARIBBEAN,
    ID_TZ_REGION_CENTRAL_AMERICA,
    ID_TZ_REGION_ETCETERA,
    ID_TZ_REGION_EUROPE,
    ID_TZ_REGION_INDIAN_OCEAN,
    ID_TZ_REGION_NORTH_AMERICA,
    TD_TZ_REGION_MIDDLE_EAST,
    ID_TZ_REGION_PACIFIC_OCEAN,
    ID_TZ_REGION_SOUTH_AMERICA,
    ID_TZ_REGION_UNKNOWN,

    /* --- Set alarms screen --- */
    ID_ALARM_ON_1,
    ID_ALARM_ON_2,
    ID_ALARM_EDIT_1,
    ID_ALARM_EDIT_2,

    /* --- Set time screen --- */
    ID_SET_TIME,
    ID_SET_DATE_DAY,
    ID_SET_DATE_MONTH,
    ID_SET_DATE_YEAR,

    /* --- Display settings screen --- */
    ID_CLOCK_24H,
    ID_CLOCK_COLOR,
    ID_CLOCK_BRIGHTNESS,
    ID_LCD_CONTRAST,
    ID_DATE_FORMAT,
    ID_ALS_PRESET,

    /* --- Network settings screen --- */
    ID_NETWORK_STATUS,
    ID_NETWORK_DHCP,
    ID_NETWORK_IP,
    ID_NETWORK_MASK,
    ID_NETWORK_GATEWAY,
    ID_NETWORK_DNS,
    ID_NETWORK_HOSTNAME,
    ID_NETWORK_TELNET_ENABLED,

    /* --- Edit profile screen / edit alarm screen --- */
    ID_PROFILE_FILENAME,
    ID_PROFILE_TIME,
    ID_PROFILE_SNOOZE,
    ID_PROFILE_DOW,
    ID_PROFILE_VOLUME,
    ID_PROFILE_GRADUAL,
    ID_PROFILE_VISUAL,
    ID_PROFILE_LAMP,
    ID_PROFILE_MSG,
    ID_PROFILE_TEST,

    /* --- Night lamp settings screen --- */
    ID_LAMP_COLOR,
    ID_LAMP_BRIGHTNESS,
    ID_LAMP_DELAY,
    ID_LAMP_EFFECT_SPEED,
    ID_LAMP_MODE,

    /* --- Alarm profile visual edit screen --- */
    ID_PROFILE_VISUAL_MODE,
    ID_PROFILE_VISUAL_SPEED,

    /* --- Settings menu screen --- */
    ID_SETTINGS_BACKUP,
    ID_SETTINGS_RESTORE,
    ID_SETTINGS_FACTORY_RESET,
    ID_SETTINGS_BATT_STATUS,

    /* --- Services menu --- */
    ID_SERVICE_TELNET,
    ID_SERVICE_NTP_AUTOSYNC,
    ID_SERVICE_MQTT,

    /* --- YES/NO dialog screen --- */
    ID_DIALOG_YES,
    ID_DIALOG_NO,
};


// ----------------------------------------
// Screen id's
// ----------------------------------------
enum {
    SCREEN_ID_ROOT = 0 ,
    SCREEN_ID_SUSPEND,
    SCREEN_ID_MAIN_MENU,
    SCREEN_ID_SET_TIME,
    SCREEN_ID_SET_ALARMS,
    SCREEN_ID_EDIT_ALARM,
    SCREEN_ID_SHOW_ALARMS,
    SCREEN_ID_NETWORK,
    SCREEN_ID_DISPLAY_SETTINGS,
    SCREEN_ID_LIST_PROFILES,
    SCREEN_ID_EDIT_PROFILE,
    SCREEN_ID_EDIT_NIGHT_LAMP,
    SCREEN_ID_EDIT_ALARM_LAMP,
    SCREEN_ID_EDIT_ALARM_VISUAL,
    SCREEN_ID_ALARM,
    SCREEN_ID_MENU_SETTINGS,
    SCREEN_ID_SETTINGS_MANAGER,
    SCREEN_ID_BATT_STATUS,
    SCREEN_ID_NET_STATUS,
    SCREEN_ID_SERVICES,
    SCREEN_ID_SELECT_TIMEZONE,
};


void enableNightLamp();
void disableNightLamp();

/* default screen events */
void onEnterScreen( Screen* screen, uint8_t prevScreenID );
bool onExitScreen( Screen* currentScreen );
void onValueChange( Screen* screen, ScreenItem* item );
void onSelectionChange( Screen* screen, ScreenItem* item, uint8_t fieldPos, bool fullscreen );
bool onDrawItem( Screen* screen, ScreenItem* item, uint16_t index, bool isSelected, uint8_t row, uint8_t col );
bool onKeypress( Screen* screen, uint8_t key );

/* Root screen events */
bool rootScreen_onDrawScreen( Screen* screen );
bool rootScreen_onKeypress( Screen* screen, uint8_t key );

/* Suspend screen events */
bool suspendScreen_onKeypress( Screen* screen, uint8_t key );
bool suspendScreen_onDrawScreen( Screen* screen );

/* Show alarm screen events */
void showAlarmScreen_onEnterScreen( Screen* screen, uint8_t prevScreenID );
bool showAlarmScreen_onExitScreen( Screen* screen  );
bool showAlarmScreen_onKeypress( Screen* screen, uint8_t key );
bool showAlarmScreen_onDrawScreen( Screen* screen );

/* Select timezone screen */
void selectTimezone_onEnterScreen( Screen* screen, uint8_t prevScreenID );
bool selectTimezone_onExitScreen( Screen* screen  );
bool selectTimezone_onDrawItem( Screen* screen, ScreenItem* item, uint16_t index, bool isSelected, uint8_t row, uint8_t col );
void selectTimezone_onSelectionChange( Screen* screen, ScreenItem* item, uint8_t fieldPos, bool fullscreen );

/* Active alarm screen events */
void alarmScreen_onEnterScreen( Screen* screen, uint8_t prevScreenID );
bool alarmScreen_onKeypress( Screen* screen, uint8_t key );
bool alarmScreen_onDrawScreen( Screen* screen );
void alarmScreen_onTimeout( Screen* screen );

/* Settings menu screen events */
void settingsMenu_onValueChange( Screen* screen, ScreenItem* item );

/* Settings manager screen events */
bool settingsManager_onDrawScreen( Screen* screen );
bool settingsManager_onKeypress( Screen* screen, uint8_t key );
void settingsManager_onEnterScreen( Screen* screen, uint8_t prevScreenID );

bool battStatus_onDrawScreen( Screen* screen );
void battStatus_onTimeout( Screen* screen );
void battStatus_onEnterScreen( Screen* screen, uint8_t prevScreenID );
bool battStatus_onKeypress( Screen* screen, uint8_t key );

bool netStatus_onDrawScreen( Screen* screen );
void netStatus_onEnterScreen( Screen* screen, uint8_t prevScreenID );
bool netStatus_onKeypress( Screen* screen, uint8_t key );


extern struct Time adjTime;
extern struct Date adjDate;
extern uint8_t selectedProfile;
extern uint8_t selectedAlarm;
extern uint16_t g_selectedTimezone;



//--------------------------------------------------------------------------
//
// Screen data
//
//--------------------------------------------------------------------------

/* Yes/No dialog screen items */
PROGMEM const struct ScreenItemBase ITEMS_DIALOG_YESNO[] = {
    ITEM_LINK( ID_DIALOG_NO, 1, 0, S_NO, NULL, ITEM_NORMAL ),
    ITEM_LINK( ID_DIALOG_YES, 1, 8, S_YES, NULL, ITEM_NORMAL ),
    ITEM_END()
};


// ----------------------------------------
// Root screen
// ----------------------------------------
PROGMEM const ScreenData screen_root {
    id                    : SCREEN_ID_ROOT, 
    items                 : NULL,
    customCharacterSet    : CUSTOM_CHARACTERS_ROOT,
    eventEnterScreen      : NULL,
    eventExitScreen       : NULL,
    eventValueChange      : NULL,
    eventDrawScreen       : &rootScreen_onDrawScreen,
    eventKeypress         : &rootScreen_onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL
};


// ----------------------------------------
// Suspend screen
// ----------------------------------------
PROGMEM const ScreenData screen_suspend {
    id                    : SCREEN_ID_SUSPEND, 
    items                 : NULL,
    customCharacterSet    : CUSTOM_CHARACTERS_ROOT,
    eventEnterScreen      : NULL,
    eventExitScreen       : NULL,
    eventValueChange      : NULL,
    eventDrawScreen       : &suspendScreen_onDrawScreen,
    eventKeypress         : &suspendScreen_onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Select timezone screen
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_SELECT_TIMEZONE[] = {

    ITEM_LIST( ID_TZ_REGION_AFRICA, 0, 0, _TZ_REGION_NAMES[ TZ_REGION_AFRICA ], &g_selectedTimezone, NULL,
               TZ_REGION_AFRICA_INDEX, TZ_REGION_AFRICA_INDEX + TZ_REGION_AFRICA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_LIST( ID_TZ_REGION_ANTARCTICA, 1, 0, _TZ_REGION_NAMES[ TZ_REGION_ANTARCTICA ], &g_selectedTimezone, NULL,
               TZ_REGION_ANTARCTICA_INDEX, TZ_REGION_ANTARCTICA_INDEX + TZ_REGION_ANTARCTICA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),               
    ITEM_LIST( ID_TZ_REGION_ASIA, 3, 0, _TZ_REGION_NAMES[ TZ_REGION_ASIA ], &g_selectedTimezone, NULL,
               TZ_REGION_ASIA_INDEX, TZ_REGION_ASIA_INDEX + TZ_REGION_ASIA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),               
    ITEM_LIST( ID_TZ_REGION_ATLANTIC_OCEAN, 4, 0, _TZ_REGION_NAMES[ TZ_REGION_ATLANTIC_OCEAN ], &g_selectedTimezone, NULL,
               TZ_REGION_ATLANTIC_OCEAN_INDEX, TZ_REGION_ATLANTIC_OCEAN_INDEX + TZ_REGION_ATLANTIC_OCEAN_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),               
    ITEM_LIST( ID_TZ_REGION_AUSTRALIA, 5, 0, _TZ_REGION_NAMES[ TZ_REGION_AUSTRALIA ], &g_selectedTimezone, NULL,
               TZ_REGION_AUSTRALIA_INDEX, TZ_REGION_AUSTRALIA_INDEX + TZ_REGION_AUSTRALIA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),               
    ITEM_LIST( ID_TZ_REGION_CARIBBEAN, 6, 0, _TZ_REGION_NAMES[ TZ_REGION_CARIBBEAN ], &g_selectedTimezone, NULL,
               TZ_REGION_CARIBBEAN_INDEX, TZ_REGION_CARIBBEAN_INDEX + TZ_REGION_CARIBBEAN_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),               
    ITEM_LIST( ID_TZ_REGION_CENTRAL_AMERICA, 7, 0, _TZ_REGION_NAMES[ TZ_REGION_CENTRAL_AMERICA ], &g_selectedTimezone, NULL,
               TZ_REGION_CENTRAL_AMERICA_INDEX, TZ_REGION_CENTRAL_AMERICA_INDEX + TZ_REGION_CENTRAL_AMERICA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_LIST( ID_TZ_REGION_ETCETERA, 8, 0, _TZ_REGION_NAMES[ TZ_REGION_ETCETERA ], &g_selectedTimezone, NULL,
               TZ_REGION_ETCETERA_INDEX, TZ_REGION_ETCETERA_INDEX + TZ_REGION_ETCETERA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_LIST( ID_TZ_REGION_EUROPE, 9, 0, _TZ_REGION_NAMES[ TZ_REGION_EUROPE ], &g_selectedTimezone, NULL,
               TZ_REGION_EUROPE_INDEX, TZ_REGION_EUROPE_INDEX + TZ_REGION_EUROPE_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_LIST( ID_TZ_REGION_INDIAN_OCEAN, 10, 0, _TZ_REGION_NAMES[ TZ_REGION_INDIAN_OCEAN ], &g_selectedTimezone, NULL,
               TZ_REGION_INDIAN_OCEAN_INDEX, TZ_REGION_INDIAN_OCEAN_INDEX + TZ_REGION_INDIAN_OCEAN_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_LIST( TD_TZ_REGION_MIDDLE_EAST, 11, 0, _TZ_REGION_NAMES[ TZ_REGION_MIDDLE_EAST ], &g_selectedTimezone, NULL,
               TZ_REGION_MIDDLE_EAST_INDEX, TZ_REGION_MIDDLE_EAST_INDEX + TZ_REGION_MIDDLE_EAST_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),               
    ITEM_LIST( ID_TZ_REGION_NORTH_AMERICA, 12, 0, _TZ_REGION_NAMES[ TZ_REGION_NORTH_AMERICA ], &g_selectedTimezone, NULL,
               TZ_REGION_NORTH_AMERICA_INDEX, TZ_REGION_NORTH_AMERICA_INDEX + TZ_REGION_NORTH_AMERICA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_LIST( ID_TZ_REGION_PACIFIC_OCEAN, 13, 0, _TZ_REGION_NAMES[ TZ_REGION_PACIFIC_OCEAN ], &g_selectedTimezone, NULL,
               TZ_REGION_PACIFIC_OCEAN_INDEX, TZ_REGION_PACIFIC_OCEAN_INDEX + TZ_REGION_PACIFIC_OCEAN_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_LIST( ID_TZ_REGION_SOUTH_AMERICA, 14, 0, _TZ_REGION_NAMES[ TZ_REGION_SOUTH_AMERICA ], &g_selectedTimezone, NULL,
               TZ_REGION_SOUTH_AMERICA_INDEX, TZ_REGION_SOUTH_AMERICA_INDEX + TZ_REGION_SOUTH_AMERICA_SIZE - 1, 
               DISPLAY_WIDTH - 2, ITEM_EDIT_FULLSCREEN | ITEM_LIST_VALUE_INT16 ),
    ITEM_END()
};

PROGMEM const ScreenData screen_select_timezone {
    id                    : SCREEN_ID_SELECT_TIMEZONE, 
    items                 : ITEMS_SELECT_TIMEZONE,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &selectTimezone_onEnterScreen,
    eventExitScreen       : &selectTimezone_onExitScreen,
    eventValueChange      : NULL,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : &selectTimezone_onDrawItem,
    eventSelectionChanged : &selectTimezone_onSelectionChange,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Edit alarm screen
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_EDIT_ALARM[] = {
    ITEM_TIME( ID_PROFILE_TIME, 0, 0, NULL, &g_alarm.profile.time, ITEM_COMPACT ),
    ITEM_DOW( ID_PROFILE_DOW, 1, 0, NULL, &g_alarm.profile.dow, ITEM_NORMAL ),

    ITEM_END()
};

PROGMEM const ScreenData screen_edit_alarm {
    id                    : SCREEN_ID_EDIT_ALARM, 
    items                 : ITEMS_EDIT_ALARM,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Set alarm screen
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_SET_ALARM[] = {

    /* Alarm 1 */
    ITEM_STATIC( 0, 0, S_ALARM_NUM1, ITEM_NORMAL ),
    ITEM_TOGGLE( ID_ALARM_ON_1, 0, 2, NULL, &g_config.clock.alarm_on[ 0 ], ITEM_NORMAL ),
    ITEM_LINK( ID_ALARM_EDIT_1, 0, 7, NULL, &screen_edit_alarm, ITEM_NORMAL ),

    /* Alarm 2 */
    ITEM_STATIC( 1, 0, S_ALARM_NUM2, ITEM_NORMAL ),
    ITEM_TOGGLE( ID_ALARM_ON_2, 1, 2, NULL, &g_config.clock.alarm_on[ 1 ], ITEM_NORMAL ),
    ITEM_LINK( ID_ALARM_EDIT_2, 1, 7, NULL, &screen_edit_alarm, ITEM_NORMAL ),

    ITEM_END()
};

PROGMEM const ScreenData screen_set_alarms {
    id                    : SCREEN_ID_SET_ALARMS, 
    items                 : ITEMS_SET_ALARM,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : &onDrawItem,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Set date/time screen
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_SET_TIME[] = {
    ITEM_TIME( ID_SET_TIME, 0, 0, NULL, &adjTime, ITEM_COMPACT ),
    ITEM_NUMBER( ID_SET_DATE_DAY, 1, 0, NULL, &adjDate.day, 1, 31, ITEM_COMPACT | ITEM_NUMBER_INC_WHOLE ),
    ITEM_MONTH( ID_SET_DATE_MONTH, 1, 3, NULL, &adjDate.month, ITEM_COMPACT ),
    ITEM_YEAR( ID_SET_DATE_YEAR, 1, 7, NULL, &adjDate.year, ITEM_COMPACT ),
    ITEM_END()
};

PROGMEM const ScreenData screen_set_time {
    id                    : SCREEN_ID_SET_TIME, 
    items                 : ITEMS_SET_TIME,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : &onSelectionChange,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Show next alarm screen
// ----------------------------------------
PROGMEM const ScreenData screen_show_alarms {
    id                    : SCREEN_ID_SHOW_ALARMS, 
    items                 : NULL,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &showAlarmScreen_onEnterScreen,
    eventExitScreen       : &showAlarmScreen_onExitScreen,
    eventValueChange      : NULL,
    eventDrawScreen       : &showAlarmScreen_onDrawScreen,
    eventKeypress         : &showAlarmScreen_onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Alarm screen
// ----------------------------------------
PROGMEM const ScreenData screen_alarm {
    id                    : SCREEN_ID_ALARM, 
    items                 : NULL,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &alarmScreen_onEnterScreen,
    eventExitScreen       : NULL,
    eventValueChange      : NULL,
    eventDrawScreen       : &alarmScreen_onDrawScreen,
    eventKeypress         : &alarmScreen_onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : &alarmScreen_onTimeout,
};


// ----------------------------------------
// Edit alarm profile visual settings
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_EDIT_PROFILE_VISUAL[] = {
    ITEM_LIST( ID_PROFILE_VISUAL_MODE, 0, 0, S_EDIT_PROFILE_VISUAL_MODE, &g_alarm.profile.visualMode,
               _ALARM_VISUAL, 0, MAX_ALARM_VISUALS - 1, ALARM_VISUAL_NAME_LENGTH,
               ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN ),

    ITEM_BAR( ID_PROFILE_VISUAL_SPEED, 1, 0, S_EDIT_PROFILE_VISUAL_SPEED, &g_alarm.profile.effectSpeed,
              MIN_ALARM_VISUAL_EFFECT_SPEED, MAX_ALARM_VISUAL_EFFECT_SPEED, 9, ITEM_EDIT_FULLSCREEN ),
    ITEM_END()
};

PROGMEM const ScreenData screen_edit_alarm_visual {
    id                    : SCREEN_ID_EDIT_ALARM_VISUAL, 
    items                 : ITEMS_EDIT_PROFILE_VISUAL,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : &onSelectionChange,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Edit alarm lamp settings
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_EDIT_ALARM_LAMP[] = {
    ITEM_LIST( ID_LAMP_MODE, 0, 0, S_NIGHT_LAMP_EFFECT, &g_alarm.profile.lamp.mode,
               _ALARM_LAMP_MODES, 0, MAX_ALARM_LAMP_MODES - 1, ALARM_LAMP_MODES_NAME_LENGTH,
               ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN ),

    ITEM_LIST( ID_LAMP_COLOR, 1, 0, S_NIGHT_LAMP_COLOR, &g_alarm.profile.lamp.color,
               _COLOR_NAMES, 0, COLOR_TABLE_MAX_COLORS - 1, COLOR_NAME_MAX_LENGTH,
               ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN ),

    ITEM_BAR( ID_LAMP_BRIGHTNESS, 2, 0, S_NIGHT_LAMP_BRIGHTNESS, &g_alarm.profile.lamp.brightness,
              MIN_ALARM_LAMP_BRIGHTNESS, MAX_ALARM_LAMP_BRIGHTNESS, 12, ITEM_EDIT_FULLSCREEN ),

    ITEM_BAR( ID_LAMP_EFFECT_SPEED, 3, 0, S_EDIT_PROFILE_VISUAL_SPEED, &g_alarm.profile.lamp.speed,
              MIN_ALARM_LAMP_EFFECT_SPEED, MAX_ALARM_LAMP_EFFECT_SPEED, 9, ITEM_EDIT_FULLSCREEN ),
    ITEM_END()
};

PROGMEM const ScreenData screen_edit_alarm_lamp {
    id                    : SCREEN_ID_EDIT_ALARM_LAMP, 
    items                 : ITEMS_EDIT_ALARM_LAMP,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : &onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : &onSelectionChange,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Edit alarm profile screen
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_EDIT_PROFILE[] = {
    ITEM_LIST( ID_PROFILE_FILENAME, 0, 0, S_EDIT_PROFILE_FILENAME, NULL,
               &g_alarm.profile.filename, 0, 0, DISPLAY_WIDTH,
               ITEM_LIST_SRAM_POINTER | ITEM_EDIT_FULLSCREEN | ITEM_COMPACT ),

    ITEM_BAR( ID_PROFILE_VOLUME, 1, 0, S_EDIT_PROFILE_VOLUME, &g_alarm.profile.volume,
              MIN_ALARM_VOLUME, MAX_ALARM_VOLUME, 10, ITEM_EDIT_FULLSCREEN ),

    ITEM_NUMBER( ID_PROFILE_SNOOZE, 2, 0, S_EDIT_PROFILE_SNOOZE, &g_alarm.profile.snoozeDelay,
                 MIN_ALARM_SNOOZE_TIME, MAX_ALARM_SNOOZE_TIME,
                 ITEM_NUMBER_INC_WHOLE | ITEM_EDIT_FULLSCREEN | ITEM_NOCURSOR ),

    ITEM_TOGGLE( ID_PROFILE_GRADUAL, 3, 0, S_EDIT_PROFILE_GRADUAL, &g_alarm.profile.gradual, ITEM_NORMAL ),

    ITEM_LINK( ID_PROFILE_VISUAL, 4, 0, S_EDIT_PROFILE_VISUAL, &screen_edit_alarm_visual, ITEM_NORMAL ),

    ITEM_LINK( ID_PROFILE_LAMP, 5, 0, S_EDIT_PROFILE_LAMP, &screen_edit_alarm_lamp, ITEM_NORMAL ),

    ITEM_TEXT( ID_PROFILE_MSG, 6, 0, S_EDIT_PROFILE_MESSAGE, &g_alarm.profile.message,
               MAX_LENGTH_ALARM_MESSAGE, ITEM_EDIT_FULLSCREEN ),

    ITEM_LINK( ID_PROFILE_TEST, 7, 0, S_EDIT_PROFILE_TEST, NULL, ITEM_NORMAL ),
    ITEM_END()
};

PROGMEM const ScreenData screen_edit_profile {
    id                    : SCREEN_ID_EDIT_PROFILE, 
    items                 : ITEMS_EDIT_PROFILE,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : &onDrawItem,
    eventSelectionChanged : &onSelectionChange,
    eventTimeout          : NULL,
};


// ----------------------------------------
// List alaram profiles screen
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_LIST_PROFILES[] = {
    ITEM_LINK( ID_ALARM_EDIT_1, 0, 0, S_EDIT_ALARM_1, &screen_edit_profile, ITEM_NORMAL ),
    ITEM_LINK( ID_ALARM_EDIT_2, 1, 0, S_EDIT_ALARM_2, &screen_edit_profile, ITEM_NORMAL ),
    ITEM_END()
};

PROGMEM const ScreenData screen_list_profiles {
    id                    : SCREEN_ID_LIST_PROFILES, 
    items                 : ITEMS_LIST_PROFILES,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Display settings menu
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_DISPLAY_SETTINGS[] = {
    ITEM_TOGGLE( ID_CLOCK_24H, 0, 0, S_MENU_SETTINGS_24H, &g_config.clock.display_24h, ITEM_COMPACT ),

    ITEM_LIST( ID_CLOCK_COLOR, 1, 0, S_MENU_SETTINGS_COLOR, &g_config.clock.clock_color,
               _COLOR_NAMES, 0, COLOR_TABLE_MAX_COLORS - 1, COLOR_NAME_MAX_LENGTH,
               ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN ),

    ITEM_BAR( ID_CLOCK_BRIGHTNESS, 2, 0, S_MENU_SETTINGS_BRIGHT, &g_config.clock.clock_brightness,
              MIN_CLOCK_BRIGHTNESS, MAX_CLOCK_BRIGHTNESS, 12, ITEM_EDIT_FULLSCREEN ),

    ITEM_BAR( ID_LCD_CONTRAST, 3, 0, S_MENU_SETTINGS_LCD_CTR, &g_config.clock.lcd_contrast,
              MIN_LCD_CONTRAST, MAX_LCD_CONTRAST, 12, ITEM_EDIT_FULLSCREEN ),

    ITEM_LIST( ID_DATE_FORMAT, 4, 0, S_MENU_SETTINGS_DATE_FMT, &g_config.clock.date_format,
               _DATE_FORMATS, 0, MAX_DATE_FORMATS - 1, DATE_FORMAT_LENGTH,
               ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN | ITEM_COMPACT ),

    ITEM_LIST( ID_ALS_PRESET, 5, 0, S_MENU_SETTINGS_ALS_PRESET, &g_config.clock.als_preset,
               _ALS_PRESET_NAMES, 0, MAX_ALS_PRESETS_NAMES - 1, ALS_PRESET_NAME_LENGTH,
               ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN | ITEM_COMPACT ),

    ITEM_END()
};

PROGMEM const ScreenData screen_display {
    id                    : SCREEN_ID_DISPLAY_SETTINGS, 
    items                 : ITEMS_DISPLAY_SETTINGS,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Edit night lamp settings
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_EDIT_NIGHT_LAMP[] = {
    ITEM_LIST( ID_LAMP_COLOR, 0, 0, S_NIGHT_LAMP_COLOR, &g_config.clock.lamp.color,
               _COLOR_NAMES, 0, COLOR_TABLE_MAX_COLORS - 1, COLOR_NAME_MAX_LENGTH,
               ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN ),

    ITEM_BAR( ID_LAMP_BRIGHTNESS, 1, 0, S_NIGHT_LAMP_BRIGHTNESS, &g_config.clock.lamp.brightness,
              MIN_LAMP_BRIGHTNESS, MAX_LAMP_BRIGHTNESS, 12, ITEM_EDIT_FULLSCREEN ),

    ITEM_NUMBER( ID_LAMP_DELAY, 2, 0, S_NIGHT_LAMP_DELAY, &g_config.clock.lamp.delay_off,
                 MIN_LAMP_DELAY_OFF, MAX_LAMP_DELAY_OFF,
                 ITEM_NUMBER_INC_WHOLE | ITEM_EDIT_FULLSCREEN | ITEM_NOCURSOR ),
    ITEM_END()
};

PROGMEM const ScreenData screen_edit_night_lamp {
    id                    : SCREEN_ID_EDIT_NIGHT_LAMP, 
    items                 : ITEMS_EDIT_NIGHT_LAMP,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : &onKeypress,
    eventDrawItem         : &onDrawItem,
    eventSelectionChanged : &onSelectionChange ,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Network status screen
// ----------------------------------------
PROGMEM const ScreenData screen_net_status {
    id                    : SCREEN_ID_NET_STATUS, 
    items                 : NULL,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &netStatus_onEnterScreen,
    eventExitScreen       : NULL,
    eventValueChange      : NULL,
    eventDrawScreen       : &netStatus_onDrawScreen,
    eventKeypress         : &netStatus_onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Network settings menu
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_NETWORK[] = {
    ITEM_LINK( ID_NETWORK_STATUS, 0, 0, S_MENU_NETWORK_STATUS, &screen_net_status, ITEM_NORMAL ),
    ITEM_TOGGLE( ID_NETWORK_DHCP, 1, 0, S_MENU_NETWORK_DHCP, &g_config.network.dhcp, ITEM_NORMAL ),
    ITEM_IP( ID_NETWORK_IP, 2, 0, S_MENU_NETWORK_IP, &g_config.network.ip, ITEM_EDIT_FULLSCREEN ),
    ITEM_IP( ID_NETWORK_MASK, 3, 0, S_MENU_NETWORK_MASK, &g_config.network.mask, ITEM_EDIT_FULLSCREEN ),
    ITEM_IP( ID_NETWORK_GATEWAY, 4, 0, S_MENU_NETWORK_GATEWAY, &g_config.network.gateway, ITEM_EDIT_FULLSCREEN ),
    ITEM_IP( ID_NETWORK_DNS, 5, 0, S_MENU_NETWORK_DNS, &g_config.network.dns, ITEM_EDIT_FULLSCREEN ),
    ITEM_TEXT( ID_NETWORK_HOSTNAME, 6, 0, S_MENU_NETWORK_HOSTNAME, &g_config.network.hostname,
               MAX_HOSTNAME_LENGTH, ITEM_EDIT_FULLSCREEN ),
    ITEM_END()
};

PROGMEM const ScreenData screen_network {
    id                    : SCREEN_ID_NETWORK, 
    items                 : ITEMS_NETWORK,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : NULL,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Settings manager screen
// ----------------------------------------
PROGMEM const ScreenData screen_settings_manager {
    id                    : SCREEN_ID_SETTINGS_MANAGER, 
    items                 : ITEMS_DIALOG_YESNO,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &settingsManager_onEnterScreen,
    eventExitScreen       : NULL,
    eventValueChange      : NULL,
    eventDrawScreen       : &settingsManager_onDrawScreen,
    eventKeypress         : &settingsManager_onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Battery status menu
// ----------------------------------------
PROGMEM const ScreenData screen_batt_status {
    id                    : SCREEN_ID_BATT_STATUS, 
    items                 : NULL,
    customCharacterSet    : CUSTOM_CHARACTERS_ROOT,
    eventEnterScreen      : &battStatus_onEnterScreen,
    eventExitScreen       : NULL,
    eventValueChange      : NULL,
    eventDrawScreen       : &battStatus_onDrawScreen,
    eventKeypress         : &battStatus_onKeypress,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : &battStatus_onTimeout,
};


// ----------------------------------------
// Settings menu
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_MENU_SETTINGS[] = {
    ITEM_LINK( ID_SETTINGS_BACKUP, 0, 0, S_MENU_SETTINGS_BACKUP, &screen_settings_manager, ITEM_NORMAL ),
    ITEM_LINK( ID_SETTINGS_RESTORE, 1, 0, S_MENU_SETTINGS_RESTORE, &screen_settings_manager, ITEM_NORMAL ),
    ITEM_LINK( ID_SETTINGS_FACTORY_RESET, 2, 0, S_MENU_SETTINGS_RESET, &screen_settings_manager, ITEM_NORMAL ),
    ITEM_LINK( ID_SETTINGS_BATT_STATUS, 3, 0, S_MENU_SETTINGS_BATT_INFO, &screen_batt_status, ITEM_NORMAL ),
    ITEM_END()
};

PROGMEM const ScreenData screen_menu_settings {
    id                    : SCREEN_ID_MENU_SETTINGS, 
    items                 : ITEMS_MENU_SETTINGS,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : NULL,
    eventExitScreen       : NULL,
    eventValueChange      : &settingsMenu_onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Service manager menu
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_MENU_SERVICES[] = {
    ITEM_TOGGLE( ID_SERVICE_TELNET, 0, 0, S_MENU_SERVICE_TELNET, &g_config.network.telnetEnabled, ITEM_COMPACT ),
    ITEM_TOGGLE( ID_SERVICE_NTP_AUTOSYNC, 1, 0, S_MENU_SERVICE_TIME_AUTOSYNC, &g_config.clock.use_ntp, ITEM_COMPACT ),
    ITEM_TOGGLE( ID_SERVICE_MQTT, 2, 0, S_MENU_SERVICE_MQTT, &g_config.network.mqtt_enabled, ITEM_COMPACT ),
    ITEM_END()
};

PROGMEM const ScreenData screen_menu_services {
    id                    : SCREEN_ID_SERVICES, 
    items                 : ITEMS_MENU_SERVICES,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};


// ----------------------------------------
// Main menu
// ----------------------------------------
PROGMEM const struct ScreenItemBase ITEMS_MAIN_MENU[] = {
    ITEM_LINK( ID_MAIN_SET_ALARMS, 0, 0, S_MAIN_MENU_SET_ALARMS, &screen_set_alarms, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_SET_TIME, 1, 0, S_MAIN_MENU_SET_TIME, &screen_set_time, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_SELECT_TIMEZONE, 2, 0, S_MAIN_MENU_TIMEZONE, &screen_select_timezone, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_LIST_PROFILES, 3, 0, S_MAIN_MENU_PROFILES, &screen_list_profiles, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_EDIT_DISPLAY, 4, 0, S_MAIN_MENU_DISPLAY, &screen_display, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_EDIT_LAMP, 5, 0, S_MAIN_MENU_LAMP, &screen_edit_night_lamp, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_EDIT_NETWORK, 6, 0, S_MAIN_MENU_NETWORK, &screen_network, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_SERVICES, 7, 0, S_MAIN_MENU_SERVICES, &screen_menu_services, ITEM_NORMAL ),
    ITEM_LINK( ID_MAIN_SETTINGS, 8, 0, S_MAIN_MENU_SETTINGS, &screen_menu_settings, ITEM_NORMAL ),
    ITEM_END()
};

PROGMEM const ScreenData screen_main_menu {
    id                    : SCREEN_ID_MAIN_MENU, 
    items                 : ITEMS_MAIN_MENU,
    customCharacterSet    : CUSTOM_CHARACTERS_DEFAULT,
    eventEnterScreen      : &onEnterScreen,
    eventExitScreen       : &onExitScreen,
    eventValueChange      : &onValueChange,
    eventDrawScreen       : NULL,
    eventKeypress         : NULL,
    eventDrawItem         : NULL,
    eventSelectionChanged : NULL,
    eventTimeout          : NULL,
};

#endif /* UI_H */