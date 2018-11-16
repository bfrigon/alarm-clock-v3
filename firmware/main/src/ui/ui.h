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

#include "../libs/time.h"

#include "../hardware.h"
#include "../config.h"
#include "../screen.h"
#include "../alarm.h"
#include "../resources.h"




// ----------------------------------------
// Screen item id's
// ----------------------------------------

/* --- Main menu --- */
#define ID_MAIN_SET_ALARMS          1
#define ID_MAIN_SET_TIME            2
#define ID_MAIN_LIST_PROFILES       3
#define ID_MAIN_EDIT_DISPLAY        4
#define ID_MAIN_EDIT_LAMP           5
#define ID_MAIN_EDIT_NETWORK        6

/* --- Set alarms screen --- */
#define ID_ALARM_ON_1               10
#define ID_ALARM_ON_2               11
#define ID_ALARM_EDIT_1             12
#define ID_ALARM_EDIT_2             13

/* --- Set time screen --- */
#define ID_SET_TIME                 15
#define ID_SET_DATE_DAY             16
#define ID_SET_DATE_MONTH           17
#define ID_SET_DATE_YEAR            18

/* --- Display settings screen --- */
#define ID_CLOCK_24H                20
#define ID_CLOCK_COLOR              21
#define ID_CLOCK_BRIGHTNESS         22
#define ID_LCD_CONTRAST             23
#define ID_DATE_FORMAT              24

/* --- Network settings screen --- */
#define ID_NETWORK_DHCP             30
#define ID_NETWORK_IP               31
#define ID_NETWORK_MASK             32
#define ID_NETWORK_GATEWAY          33
#define ID_NETWORK_DNS              34

/* --- Edit profile screen / edit alarm screen --- */
#define ID_PROFILE_FILENAME         40
#define ID_PROFILE_TIME             41
#define ID_PROFILE_SNOOZE           42
#define ID_PROFILE_DOW              43
#define ID_PROFILE_VOLUME           44
#define ID_PROFILE_GRADUAL          45
#define ID_PROFILE_VISUAL           46
#define ID_PROFILE_LAMP             47
#define ID_PROFILE_MSG              48
#define ID_PROFILE_TEST             49

/* --- Night lamp settings screen --- */
#define ID_LAMP_COLOR               60
#define ID_LAMP_BRIGHTNESS          61
#define ID_LAMP_DELAY               62
#define ID_LAMP_EFFECT_SPEED        63
#define ID_LAMP_MODE                64

/* --- Alarm profile visual edit screen --- */
#define ID_PROFILE_VISUAL_MODE      70
#define ID_PROFILE_VISUAL_SPEED     71


// ----------------------------------------
// Screen id's
// ----------------------------------------
#define SCREEN_ID_ROOT              0
#define SCREEN_ID_MAIN_MENU         1
#define SCREEN_ID_SET_TIME          2
#define SCREEN_ID_SET_ALARMS        3
#define SCREEN_ID_EDIT_ALARM        4
#define SCREEN_ID_SHOW_ALARMS       5
#define SCREEN_ID_NETWORK           6
#define SCREEN_ID_SETTINGS          7
#define SCREEN_ID_LIST_PROFILES     8
#define SCREEN_ID_EDIT_PROFILE      9
#define SCREEN_ID_EDIT_NIGHT_LAMP   10
#define SCREEN_ID_EDIT_ALARM_LAMP   11
#define SCREEN_ID_EDIT_ALARM_VISUAL 12
#define SCREEN_ID_ALARM             15


extern struct Time adjTime;
extern struct Date adjDate;
extern uint8_t selectedProfile;
extern uint8_t selectedAlarm;

void initScreens();

void resetClockDisplay();
bool checkFactoryResetBtn();


bool onEnterScreen( Screen *screen );
bool onExitScreen( Screen *currentScreen, Screen *newScreen );
bool onValueChange( Screen *screen, Item *item );
void onSelectionChange( Screen *screen, Item *item, uint8_t fieldPos, bool fullscreen );
bool onDrawItem( Screen *screen, Item *item, bool isSelected, uint8_t row, uint8_t col );

bool rootScreen_onDrawScreen( Screen *screen );
bool rootScreen_onKeypress( Screen *screen, uint8_t key );

bool showAlarmScreen_onKeypress( Screen *screen, uint8_t key );
bool showAlarmScreen_onDrawScreen( Screen *screen );

bool alarmScreen_onEnterScreen( Screen *currentScreen );
bool alarmScreen_onKeypress( Screen *screen, uint8_t key );
bool alarmScreen_onDrawScreen( Screen *screen );
void alarmScreen_onTimeout( Screen *screen );






extern Screen screen_root;
extern Screen screen_main_menu;
extern Screen screen_display;
extern Screen screen_network;
extern Screen screen_set_time;
extern Screen screen_set_alarms;
extern Screen screen_edit_alarm;
extern Screen screen_show_alarms;
extern Screen screen_edit_profile;
extern Screen screen_list_profiles;
extern Screen screen_edit_night_lamp;
extern Screen screen_edit_alarm_lamp;
extern Screen screen_edit_alarm_visual;
extern Screen screen_alarm;


//--------------------------------------------------------------------------
//
// Screen items
//
//--------------------------------------------------------------------------

/* Display settings menu items */
BEGIN_SCREEN_ITEMS( ITEMS_DISPLAY_SETTINGS )
ITEM_TOGGLE( ID_CLOCK_24H, 0, 0, S_MENU_SETTINGS_24H, &g_config.clock_24h, ITEM_COMPACT )

ITEM_LIST( ID_CLOCK_COLOR, 1, 0, S_MENU_SETTINGS_COLOR, &g_config.clock_color,
           _COLOR_NAMES, 0, COLOR_TABLE_MAX_COLORS - 1, COLOR_NAME_MAX_LENGTH,
           ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN )

ITEM_BAR( ID_CLOCK_BRIGHTNESS, 2, 0, S_MENU_SETTINGS_BRIGHT, &g_config.clock_brightness,
          15, 80, 12, ITEM_EDIT_FULLSCREEN )

ITEM_BAR( ID_LCD_CONTRAST, 3, 0, S_MENU_SETTINGS_LCD_CTR, &g_config.lcd_contrast,
          0, 100, 12, ITEM_EDIT_FULLSCREEN )

ITEM_LIST( ID_DATE_FORMAT, 4, 0, S_MENU_SETTINGS_DATE_FMT, &g_config.date_format,
           _DATE_FORMATS, 0, MAX_DATE_FORMATS - 1, DATE_FORMAT_LENGTH,
           ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN )

END_SCREEN_ITEMS()


/* Set time screen items */
BEGIN_SCREEN_ITEMS( ITEMS_SET_TIME )
ITEM_TIME( ID_SET_TIME, 0, 0, NULL, &adjTime, ITEM_COMPACT )
ITEM_NUMBER( ID_SET_DATE_DAY, 1, 0, NULL, &adjDate.day, 1, 31, ITEM_COMPACT | ITEM_NUMBER_INC_WHOLE )
ITEM_MONTH( ID_SET_DATE_MONTH, 1, 3, NULL, &adjDate.month, ITEM_COMPACT )
ITEM_YEAR( ID_SET_DATE_YEAR, 1, 7, NULL, &adjDate.year, ITEM_COMPACT )
END_SCREEN_ITEMS()


/* Set alarm screen items */
BEGIN_SCREEN_ITEMS( ITEMS_SET_ALARM )
// ----------------
// 1.>Off >12:00 am
// 2.>On  >12:00 am
// ----------------

/* Alarm 1 */
ITEM_STATIC( 0, 0, S_ALARM_NUM1, ITEM_NORMAL )
ITEM_TOGGLE( ID_ALARM_ON_1, 0, 2, NULL, &g_config.alarm_on[ 0 ], ITEM_NORMAL )
ITEM_LINK( ID_ALARM_EDIT_1, 0, 7, NULL, &screen_edit_alarm, ITEM_NORMAL )

/* Alarm 2 */
ITEM_STATIC( 1, 0, S_ALARM_NUM2, ITEM_NORMAL )
ITEM_TOGGLE( ID_ALARM_ON_2, 1, 2, NULL, &g_config.alarm_on[ 1 ], ITEM_NORMAL )
ITEM_LINK( ID_ALARM_EDIT_2, 1, 7, NULL, &screen_edit_alarm, ITEM_NORMAL )

END_SCREEN_ITEMS()


/* Edit alarm screen items */
BEGIN_SCREEN_ITEMS( ITEMS_EDIT_ALARM )
// ----------------
//  12:00 am
//  S - - - T F S
// ----------------

ITEM_TIME( ID_PROFILE_TIME, 0, 0, NULL, &g_alarm.profile.time, ITEM_COMPACT )
ITEM_DOW( ID_PROFILE_DOW, 1, 0, NULL, &g_alarm.profile.dow, ITEM_NORMAL )

END_SCREEN_ITEMS()


/* List alarm profiles screen */
BEGIN_SCREEN_ITEMS( ITEMS_LIST_PROFILES )
ITEM_LINK( ID_ALARM_EDIT_1, 0, 0, S_EDIT_ALARM_1, &screen_edit_profile, ITEM_NORMAL )
ITEM_LINK( ID_ALARM_EDIT_2, 1, 0, S_EDIT_ALARM_2, &screen_edit_profile, ITEM_NORMAL )
END_SCREEN_ITEMS()


/* Network menu items */
BEGIN_SCREEN_ITEMS( ITEMS_NETWORK )
ITEM_TOGGLE( ID_NETWORK_DHCP, 0, 0, S_MENU_NETWORK_DHCP, &g_config.net_dhcp, ITEM_NORMAL )
ITEM_IP( ID_NETWORK_IP, 1, 0, S_MENU_NETWORK_IP, &g_config.net_ip, ITEM_EDIT_FULLSCREEN )
ITEM_IP( ID_NETWORK_MASK, 2, 0, S_MENU_NETWORK_MASK, &g_config.net_mask, ITEM_EDIT_FULLSCREEN )
ITEM_IP( ID_NETWORK_GATEWAY, 3, 0, S_MENU_NETWORK_GATEWAY, &g_config.net_gateway, ITEM_EDIT_FULLSCREEN )
ITEM_IP( ID_NETWORK_DNS, 4, 0, S_MENU_NETWORK_DNS, &g_config.net_dns, ITEM_EDIT_FULLSCREEN )
END_SCREEN_ITEMS()


/* Edit profile screen items */
BEGIN_SCREEN_ITEMS( ITEMS_EDIT_PROFILE )
ITEM_LIST( ID_PROFILE_FILENAME, 0, 0, S_EDIT_PROFILE_FILENAME, NULL,
           &g_alarm.profile.filename, 0, 0, 12,
           ITEM_LIST_SRAM_POINTER | ITEM_EDIT_FULLSCREEN )

ITEM_BAR( ID_PROFILE_VOLUME, 1, 0, S_EDIT_PROFILE_VOLUME, &g_alarm.profile.volume,
          20, 100, 10, ITEM_EDIT_FULLSCREEN )

ITEM_NUMBER( ID_PROFILE_SNOOZE, 2, 0, S_EDIT_PROFILE_SNOOZE, &g_alarm.profile.snoozeDelay, 0, 30,
             ITEM_NUMBER_INC_WHOLE | ITEM_EDIT_FULLSCREEN | ITEM_NOCURSOR )

ITEM_TOGGLE( ID_PROFILE_GRADUAL, 3, 0, S_EDIT_PROFILE_GRADUAL, &g_alarm.profile.gradual, ITEM_NORMAL )

ITEM_LINK( ID_PROFILE_VISUAL, 4, 0, S_EDIT_PROFILE_VISUAL, &screen_edit_alarm_visual, ITEM_NORMAL )

ITEM_LINK( ID_PROFILE_LAMP, 5, 0, S_EDIT_PROFILE_LAMP, &screen_edit_alarm_lamp, ITEM_NORMAL )

ITEM_TEXT( ID_PROFILE_MSG, 6, 0, S_EDIT_PROFILE_MESSAGE, &g_alarm.profile.message, ALARM_MESSAGE_LENGTH,
           ITEM_EDIT_FULLSCREEN )

ITEM_LINK( ID_PROFILE_TEST, 7, 0, S_EDIT_PROFILE_TEST, NULL, ITEM_NORMAL )
END_SCREEN_ITEMS()


/* Alarm profile visual settings */
BEGIN_SCREEN_ITEMS( ITEMS_EDIT_PROFILE_VISUAL )
ITEM_LIST( ID_PROFILE_VISUAL_MODE, 0, 0, S_EDIT_PROFILE_VISUAL_MODE, &g_alarm.profile.visualMode,
           _ALARM_VISUAL, 0, MAX_ALARM_VISUALS - 1, ALARM_VISUAL_NAME_LENGTH,
           ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN )

ITEM_BAR( ID_PROFILE_VISUAL_SPEED, 1, 0, S_EDIT_PROFILE_VISUAL_SPEED, &g_alarm.profile.effectSpeed,
          2, 12, 10, ITEM_EDIT_FULLSCREEN )
END_SCREEN_ITEMS()


/* Edit night lamp settings */
BEGIN_SCREEN_ITEMS( ITEMS_EDIT_NIGHT_LAMP )
ITEM_LIST( ID_LAMP_COLOR, 0, 0, S_NIGHT_LAMP_COLOR, &g_config.lamp.color,
           _COLOR_NAMES, 0, COLOR_TABLE_MAX_COLORS - 1, COLOR_NAME_MAX_LENGTH,
           ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN )

ITEM_BAR( ID_LAMP_BRIGHTNESS, 1, 0, S_NIGHT_LAMP_BRIGHTNESS, &g_config.lamp.brightness,
          20, 100, 12, ITEM_EDIT_FULLSCREEN )

ITEM_NUMBER( ID_LAMP_DELAY, 2, 0, S_NIGHT_LAMP_DELAY, &g_config.lamp.delay_off,
             0, 90, ITEM_NUMBER_INC_WHOLE | ITEM_EDIT_FULLSCREEN | ITEM_NOCURSOR )
END_SCREEN_ITEMS()


/* Edit alarm lamp settings */
BEGIN_SCREEN_ITEMS( ITEMS_EDIT_ALARM_LAMP )
ITEM_LIST( ID_LAMP_MODE, 0, 0, S_NIGHT_LAMP_EFFECT, &g_alarm.profile.lamp.mode,
           _ALARM_LAMP_MODES, 0, MAX_ALARM_LAMP_MODES - 1, ALARM_LAMP_MODES_NAME_LENGTH,
           ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN )

ITEM_LIST( ID_LAMP_COLOR, 1, 0, S_NIGHT_LAMP_COLOR, &g_alarm.profile.lamp.color,
           _COLOR_NAMES, 0, COLOR_TABLE_MAX_COLORS - 1, COLOR_NAME_MAX_LENGTH,
           ITEM_LIST_PROGMEM_POINTER | ITEM_EDIT_FULLSCREEN )

ITEM_BAR( ID_LAMP_BRIGHTNESS, 2, 0, S_NIGHT_LAMP_BRIGHTNESS, &g_alarm.profile.lamp.brightness,
          1, 100, 12, ITEM_EDIT_FULLSCREEN )

ITEM_BAR( ID_LAMP_EFFECT_SPEED, 3, 0, S_EDIT_PROFILE_VISUAL_SPEED, &g_alarm.profile.lamp.speed,
          2, 12, 10, ITEM_EDIT_FULLSCREEN )
END_SCREEN_ITEMS()


/* Main menu items */
BEGIN_SCREEN_ITEMS( ITEMS_MAIN_MENU )
ITEM_LINK( ID_MAIN_SET_ALARMS, 0, 0, S_MAIN_MENU_SET_ALARMS, &screen_set_alarms, ITEM_NORMAL )
ITEM_LINK( ID_MAIN_SET_TIME, 1, 0, S_MAIN_MENU_SET_TIME, &screen_set_time, ITEM_NORMAL )
ITEM_LINK( ID_MAIN_LIST_PROFILES, 2, 0, S_MAIN_MENU_PROFILES, &screen_list_profiles, ITEM_NORMAL )
ITEM_LINK( ID_MAIN_EDIT_DISPLAY, 3, 0, S_MAIN_MENU_DISPLAY, &screen_display, ITEM_NORMAL )
ITEM_LINK( ID_MAIN_EDIT_LAMP, 4, 0, S_MAIN_MENU_LAMP, &screen_edit_night_lamp, ITEM_NORMAL )
ITEM_LINK( ID_MAIN_EDIT_NETWORK, 5, 0, S_MAIN_MENU_NETWORK, &screen_network, ITEM_NORMAL )
END_SCREEN_ITEMS()



#endif /* UI_H */