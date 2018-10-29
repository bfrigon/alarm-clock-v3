//******************************************************************************
//
// Project : Alarm Clock V3
// File    : main.ino
// Author  : Benoit Frigon <www.bfrigon.com>
//
// -----------------------------------------------------------------------------
//
// This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
// International License. To view a copy of this license, visit
//
// http://creativcommons.org/licenses/by-sa/4.0/
//
// or send a letter to Creative Commons,
// PO Box 1866, Mountain View, CA 94042, USA.
//
//******************************************************************************


#include <Arduino.h>
#include <SD.h>
#include <Sodaq_DS3231.h>
#include <winc1500_async.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/wdt.h>

#include "main.h"
#include "lamp.h"
#include "alarm.h"
#include "resources.h"
#include "screen.h"
#include "config.h"
#include "qt1070.h"
#include "us2066.h"
#include "neoclock.h"
#include "tpa2016.h"


Alarm g_alarm( PIN_VS1053_RESET, PIN_VS1053_CS, PIN_VS1053_XDCS, PIN_VS1053_DREQ, PIN_VS1053_SDCS, PIN_SD_DETECT, PIN_ALARM_SW );
TPA2016 g_amplifier( PIN_AMP_SHDN );
NeoClock g_clock( PIN_NEOCLOCK );
Lamp g_lamp( PIN_PIX_LAMP );
QT1070 g_keypad( PIN_INT_KEYPAD );
US2066 g_lcd( I2C_ADDR_OLED, PIN_OLED_RESET, PIN_OLED_VEN );

uint8_t test = 0;

bool g_clockUpdate = true;
uint8_t selectedProfile = 0;
uint8_t selectedAlarm = 0;
bool edit_alarm_lamp_settings = false;

volatile bool rtc_event = false;


struct Time adjTime;
struct Date adjDate;

Screen screen_root ( SCREEN_ID_ROOT, NULL, NULL, &enterScreen, &exitScreen );
Screen screen_main_menu( SCREEN_ID_MAIN_MENU, ITEMS_MAIN_MENU, eventValueChange, &enterScreen, &exitScreen );
Screen screen_display( SCREEN_ID_SETTINGS, ITEMS_DISPLAY_SETTINGS, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_network( SCREEN_ID_NETWORK, ITEMS_NETWORK, NULL, &enterScreen, &exitScreen );
Screen screen_set_time( SCREEN_ID_SET_TIME, ITEMS_SET_TIME, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_set_alarms( SCREEN_ID_SET_ALARMS, ITEMS_SET_ALARM, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_edit_alarm( SCREEN_ID_EDIT_ALARM, ITEMS_EDIT_ALARM, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_show_alarms( SCREEN_ID_SHOW_ALARMS, NULL, NULL, &enterScreen, &exitScreen );
Screen screen_edit_profile( SCREEN_ID_EDIT_PROFILE, ITEMS_EDIT_PROFILE, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_list_profiles( SCREEN_ID_LIST_PROFILES, ITEMS_LIST_PROFILES, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_edit_night_lamp( SCREEN_ID_EDIT_NIGHT_LAMP, ITEMS_EDIT_NIGHT_LAMP, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_edit_alarm_lamp( SCREEN_ID_EDIT_ALARM_LAMP, ITEMS_EDIT_ALARM_LAMP, &eventValueChange, &enterScreen, &exitScreen );
Screen screen_edit_alarm_visual( SCREEN_ID_EDIT_ALARM_VISUAL, ITEMS_EDIT_PROFILE_VISUAL, &eventValueChange, &enterScreen, &exitScreen );

void initScreens() {

    /* Root screen */
    screen_root.eventDrawScreen = &root_eventDrawScreen;
    screen_root.eventKeypress = &root_eventKeypress;

    /* Set time screen */
    screen_set_time.confirmChanges = true;
    screen_set_time.eventSelectionChanged = &eventSelectionChange;

    /* Set alarm screen */
    screen_set_alarms.eventDrawItem = &eventDrawItem;

    /* Show alarms screen */
    screen_show_alarms.timeout = 3000;
    screen_show_alarms.eventKeypress = &show_alarm_eventKeypress;
    screen_show_alarms.eventDrawScreen = &show_alarm_eventDrawScreen;

    /* Network settings screen */
    screen_network.confirmChanges = true;

    /* List profile screen */
    screen_edit_profile.eventSelectionChanged = &eventSelectionChange;
    screen_edit_profile.eventDrawItem = &eventDrawItem;

    /* Edit alarm lamp settings */
    screen_edit_alarm_lamp.eventSelectionChanged = &eventSelectionChange;

    /* Edit night lamp settings */
    screen_edit_night_lamp.eventSelectionChanged = &eventSelectionChange;
    screen_edit_night_lamp.eventDrawItem = &eventDrawItem;

    /* Edit alarm lamp settings */
    screen_edit_alarm_visual.eventSelectionChanged = &eventSelectionChange;
}


void configWiFi() {

    IPAddress net_ip( &g_config.net_ip[0] );
    IPAddress net_mask( &g_config.net_mask[0] );
    IPAddress net_gateway( &g_config.net_gateway[0] );
    IPAddress net_dns( &g_config.net_dns[0] );

    WiFi.config( g_config.net_dhcp, net_ip, net_dns, net_gateway, net_mask );


}


void setup() {

    pinMode( PIN_PIX_SHDN, OUTPUT );
    digitalWrite( PIN_PIX_SHDN, LOW );

    /* Initialize clock display */
    g_clock.begin();

    g_lamp.begin();


    /* Check for factory reset sequence */
    if ( checkFactoryResetBtn() == false ) {

        /* Load configuration from EEPROM */
        loadConfig();
    }

    /* Set clock display color and brightness */
    g_clock.setColorFromTable( g_config.clock_color );
    g_clock.setBrightness( g_config.clock_brightness );
    g_clock.update();


    /* Enable watchdog timer, 4 seconds timeout */
    wdt_enable(WDTO_4S);

    /* Setup serial */
    Serial.begin( 115200 );


    /* Setup audio codec */
    g_alarm.begin();


    /* Initialize I2C Bus and DS3231 RTC */
    pinMode( PIN_INT_RTC, INPUT_PULLUP );
    attachInterrupt( digitalPinToInterrupt( PIN_INT_RTC ), isr_rtc, FALLING );

    rtc.begin();
    rtc.enableInterrupts(EveryMinute);

    rtc_event = true;   /* Force rtc event to update clock display */

    Wire.setClock(100000);


    /* Initialize OLED display */
    g_lcd.begin();
    g_lcd.setCustomCharacters( CUSTOM_CHARACTERS_ROOT );
    g_lcd.setContrast( g_config.lcd_contrast );


    initScreens();
    g_currentScreen = &screen_root;

    g_keypad.begin();

    /* Init wifi module */
    WiFi.setPins( PIN_WIFI_CS, PIN_WIFI_IRQ, PIN_WIFI_RESET, PIN_WIFI_ENABLE );
    WiFi.init(); 

    configWiFi();

    WiFi.connect( g_config.ssid, g_config.wkey );


    /* Configure audio amp */
    g_amplifier.begin();
    g_amplifier.setFixedGain( 25 );  
}




void loop() {



    /* Reset watchdog timer */
    wdt_reset();


    /* Process Keypad event if available */
    uint8_t key = g_keypad.processEvents();
    
    if ( key != KEY_NONE ) {

        g_currentScreen->processKeypadEvent( key );
    }

    /* Process RTC event if available */
    if ( rtc_event == true ) {
        
        rtc_event = false;
        rtc.clearINTStatus();

        DateTime now = rtc.now();

        if ( g_currentScreen->id == SCREEN_ID_ROOT ) {
            g_screenUpdate = true;
        }

        if ( g_currentScreen->id != SCREEN_ID_SET_TIME ) {

            g_clock.hour = now.hour();
            g_clock.minute = now.minute();

            g_clockUpdate = true;
        }

        g_alarm.checkForAlarms( &now );
    }


    /* Detect if the SD card is present, if so, initialize it */
    if ( g_alarm.isSDCardPresent() != g_alarm.DetectSDCard() ) {
        g_screenUpdate = true;
    }

    /* Detect alarm switch state */
    if ( g_alarm.isAlarmSwitchOn() != g_alarm.detectAlarmSwitchState()) {
        g_clockUpdate = true;
    }

    /* Feed alarm audio buffer and process visual effect */
    g_alarm.processAlarmEvents();


    /* Exit the current screen if it has reached its timeout value */
    if ( g_currentScreen->isTimeout() ) {

        if ( g_currentScreen->eventTimeout != NULL ) {
            g_currentScreen->eventTimeout( g_currentScreen );
            g_currentScreen->resetTimeout();

        } else {
            g_currentScreen->exitScreen();
        }
    }


    /* Update the current screen if requested */
    if ( g_screenUpdate == true ) {

        g_currentScreen->update();
        g_screenUpdate = false;
    }


    /* Update the Clock display if requested */
    g_clock.processUpdateEvents();

    /* Process lamp effect if lamp is active */
    g_lamp.processEvents();


    /* Handle pending WiFi module events */
    WiFi.handleEvents();

    if ( WiFi.statusChanged() == true ) {

        if ( g_currentScreen->id == SCREEN_ID_ROOT )  {
            g_screenUpdate = true;
        }
    }


    
}


//--------------------------------------------------------------------------
//
// Interrupts service
//
//--------------------------------------------------------------------------

void isr_rtc() {

    rtc_event = true;
}




bool checkFactoryResetBtn() {
    unsigned long start = millis();
    bool resetConfig = false;
    uint8_t i;

    while ( digitalRead ( PIN_FACTORY_RESET ) == LOW ) {


        /* Sets the config reset flag in eeprom after 1 seconds */
        if (( millis() - start ) > 1000 ) {

            g_clock.setColorFromTable( COLOR_RED );
            g_clock.setBrightness( 60 );
            g_clock.setTestMode( true );

            g_clock.status_set = true;
            g_clock.update();


            restoreDefaultConfig();

            for ( i = 0; i < 4; i++ ) {

                g_clock.status_set = ( i % 2 ) > 0;
                g_clock.update();
                delay( 200 );
            }

            resetConfig = true;
            break;
        }
    }

    g_clock.status_set = false;
    g_clock.setTestMode( false );
    g_clock.update();

    return resetConfig;
}


void resetClockDisplay() {

    DateTime now = rtc.now();

    g_clock.hour = now.hour();
    g_clock.minute = now.minute();
    g_clock.hourFlashing = false;
    g_clock.minutesFlashing = false;

    g_clock.status_set = false;

    g_clockUpdate = true;
}



//--------------------------------------------------------------------------
//
// Screen events
//
//--------------------------------------------------------------------------

bool eventDrawItem( Screen *screen, Item *item, bool isSelected, uint8_t row, uint8_t col ) {
    uint8_t length;

    switch ( item->id ) {
        case ID_PROFILE_FILENAME:

            g_lcd.print( isSelected ? CHAR_SELECT : CHAR_FIELD_BEGIN );

            if ( g_alarm.profile.filename[0] == 0x00 ) {
                length = g_lcd.print_P( S_PROFILE_DEF_FILENAME );
            } else {
                length = g_lcd.print( g_alarm.profile.filename );
            }

            g_lcd.fill ( CHAR_SPACE, item->length - length );
            g_lcd.print( isSelected ? CHAR_SELECT_REV : CHAR_FIELD_END );

            return false;

        case ID_ALARM_EDIT_1:
        case ID_ALARM_EDIT_2:
            uint8_t alarm_id;
            alarm_id = item->id - ID_ALARM_EDIT_1;

            Time time;
            g_alarm.readProfileAlarmTime( alarm_id , &time, NULL );

            char bufTime[9];
            length = timeToBuf( bufTime, g_config.clock_24h, &time);

            g_lcd.print( bufTime );
            if ( length < 8 ) {
                g_lcd.fill( CHAR_SPACE, 8 - length );
            }

            return false;

        case ID_LAMP_DELAY:
        case ID_PROFILE_SNOOZE:

            uint8_t minutes;
            minutes = *(( uint8_t* )item->value );

            if (minutes > 1 ) {
                length = g_lcd.printf_P( S_NEXT_ALARM_M, minutes);
            } else if (minutes == 1 ) {
                length = g_lcd.print_P( S_NEXT_ALARM_1MIN );

            } else {
                length = g_lcd.print_P( S_OFF );
            }
            g_lcd.fill( CHAR_SPACE, DISPLAY_WIDTH - length );


            return false;
    }

    return true;
}


void eventSelectionChange( Screen *screen, Item *item, uint8_t fieldPos, bool fullscreen ) {
    
    switch ( item->id ) {
        case ID_PROFILE_FILENAME:
        case ID_PROFILE_VOLUME:

            if ( fullscreen ) {
                g_alarm.play( ALARM_MODE_TEST_AUDIO, SELECT_FILE_PLAY_DELAY );
            } else {
                g_alarm.stop();
            }

            break;

        case ID_PROFILE_VISUAL_SPEED:
        case ID_PROFILE_VISUAL_MODE:

            if ( fullscreen ) {
                g_alarm.play( ALARM_MODE_TEST_VISUAL );
            } else {
                g_alarm.stop();
            }

            break;

        case ID_LAMP_MODE:
            if ( g_alarm.profile.lamp.mode == LAMP_MODE_OFF ) {
                g_lamp.deactivate();
                break;
            }

            /* Fall through */

        case ID_LAMP_EFFECT_SPEED:
        case ID_LAMP_COLOR:
        case ID_LAMP_BRIGHTNESS:

            NightLampSettings *settings;
            settings = edit_alarm_lamp_settings ? &g_alarm.profile.lamp : &g_config.lamp;
        
        
            if ( fullscreen ) {
                g_lamp.activate( settings, ( item->id != ID_LAMP_EFFECT_SPEED ));
            } else {
                g_lamp.deactivate();
            }
            break;

        


        case ID_SET_TIME:

            g_clock.hourFlashing = ( fieldPos == 0);
            g_clock.minutesFlashing = ( fieldPos == 1);

            g_clockUpdate = true;

            break;

        default:

            if ( g_clock.hourFlashing == true || g_clock.minutesFlashing == true ) {

                g_clock.hourFlashing = false;
                g_clock.minutesFlashing = false;

                g_clockUpdate = true;
            }

            break;

    }
}

bool eventValueChange( Screen *screen, Item *item ) {

    switch (item->id) {

        case ID_CLOCK_24H:

            g_clockUpdate = true;
            break;

        case ID_CLOCK_COLOR:
            g_clock.setColorFromTable( g_config.clock_color );

            g_clockUpdate = true;
            break;

        case ID_CLOCK_BRIGHTNESS:
            g_clock.setBrightness( g_config.clock_brightness );

            g_clockUpdate = true;
            break;

        case ID_LCD_CONTRAST:
            g_lcd.setContrast( g_config.lcd_contrast );
            break;


        case ID_PROFILE_FILENAME:
            g_alarm.openNextFile();
            g_alarm.play( ALARM_MODE_TEST_AUDIO, SELECT_FILE_PLAY_DELAY );

            break;

        case ID_SET_DATE_DAY:
        case ID_SET_DATE_YEAR:
        case ID_SET_DATE_MONTH:

            uint8_t month_days;
            month_days = getMonthNumDays( adjDate.month, adjDate.year );

            if ( adjDate.day > month_days ) {

                if ( item->id == ID_SET_DATE_DAY) {
                    adjDate.day = 1;
                } else {
                    adjDate.day = month_days;
                }
            }
            break;            

        case ID_PROFILE_VOLUME:
            g_alarm.setVolume( g_alarm.profile.volume );

            if ( g_alarm.isPlaying() == false ) {
                
                g_alarm.play( ALARM_MODE_TEST_AUDIO, SELECT_FILE_PLAY_DELAY );
            }

            break;


        case ID_PROFILE_TEST:
            g_alarm.play( ALARM_MODE_TEST_PROFILE );
            break;

        
        case ID_PROFILE_VISUAL_MODE:
            g_alarm.play( ALARM_MODE_TEST_VISUAL );
            break;

        case ID_ALARM_ON_1:
        case ID_ALARM_ON_2:
            g_clockUpdate = true;
            break;

        case ID_ALARM_EDIT_1:
        case ID_ALARM_EDIT_2:
            selectedProfile = item->id - ID_ALARM_EDIT_1;

            g_alarm.loadProfile( selectedProfile );
            break;

        case ID_SET_TIME:
            g_clock.hour = adjTime.hour;
            g_clock.minute = adjTime.minute;
            
            g_clockUpdate = true;
            break;

        case ID_LAMP_DELAY:
        case ID_PROFILE_SNOOZE:
            uint8_t minutes;
            minutes = *(( uint8_t* )item->value );

            if ( minutes > 10 ) {
                minutes = ( minutes % 5 ) ? (( minutes / 5 ) * 5 ) + 5  : minutes;
            }

            if ( minutes > 30 ) {
                minutes = ( minutes % 10 ) ? (( minutes / 10 ) * 10 ) + 10  : minutes;
            }

            *(( uint8_t* )item->value ) = minutes;
            break;


        case ID_LAMP_MODE:
            if (( uint8_t* )item->value == LAMP_MODE_OFF ) {
                g_lamp.deactivate();
            } else {
                g_lamp.activate( edit_alarm_lamp_settings == true ? &g_alarm.profile.lamp : &g_config.lamp );
            }
            break;

        case ID_LAMP_COLOR:
            g_lamp.setColorFromTable( *(( uint8_t* )item->value ));
            break;

        case ID_LAMP_BRIGHTNESS:
            g_lamp.setBrightness( *(( uint8_t* )item->value ));
            break;

        case ID_LAMP_EFFECT_SPEED:
            g_lamp.setEffectSpeed( *(( uint8_t* )item->value ));
            break;
    }

    return true;
}


bool enterScreen( Screen *screen ) {

    DateTime now;

    switch ( screen->id ) {

        case SCREEN_ID_SET_TIME:
            now = rtc.now();

            adjTime.hour = now.hour();
            adjTime.minute = now.minute();
            adjDate.day = now.date();
            adjDate.month = now.month();
            adjDate.year = (uint8_t)( now.year() - 2000 );

            g_clock.status_set = true;
            g_clockUpdate = true;

            break;

        case SCREEN_ID_SHOW_ALARMS:

            now = rtc.now();

            int8_t alarm_id;
            alarm_id = g_alarm.getNextAlarmID( &now, false );

            Time time;
            g_alarm.readProfileAlarmTime( alarm_id, &time, NULL );

            if ( alarm_id != -1 ) {

                g_clock.hour = time.hour;
                g_clock.minute = time.minute;

                g_clockUpdate = true;
            }

        case SCREEN_ID_EDIT_ALARM_LAMP:
            edit_alarm_lamp_settings = true;
            break;

        case SCREEN_ID_EDIT_NIGHT_LAMP:
            edit_alarm_lamp_settings = false;
            break;

        case SCREEN_ID_ROOT:
            g_lcd.setCustomCharacters( CUSTOM_CHARACTERS_ROOT );
            break;

        

    }

    return true;
}

bool exitScreen( Screen *currentScreen, Screen *newScreen ) {

    bool save;
    if ( currentScreen->confirmChanges == true ) {
        save = ( currentScreen->returnValue == RETURN_YES );
    } else {

        save = currentScreen->itemChanged;
    }



    switch ( currentScreen->id ) {

        case SCREEN_ID_SET_TIME:

            

            if ( save == true ) {

                uint8_t dow;
                dow = getDayOfWeek( adjDate.year, adjDate.month, adjDate.day );
                

                DateTime nDate( 2000 + adjDate.year,
                                adjDate.month,
                                adjDate.day,
                                adjTime.hour,
                                adjTime.minute,
                                0,
                                dow );

                /* Update the RTC */
                rtc.setDateTime( nDate );
            }

            resetClockDisplay();
            break;

        case SCREEN_ID_NETWORK:
            if ( save == true ) {
                saveConfig();

                configWiFi();

            } else {
                loadConfig();
            }
            break;

        case SCREEN_ID_EDIT_NIGHT_LAMP:
            if ( save == true ) {
                saveConfig();
            } else {
                loadConfig();
            }

            g_lamp.deactivate();
            break;

        case SCREEN_ID_EDIT_ALARM_LAMP:
        case SCREEN_ID_EDIT_ALARM:
        case SCREEN_ID_EDIT_PROFILE:
            if ( save == true ) {
                g_alarm.saveProfile( &g_alarm.profile, selectedProfile );
            }

            break;

        case SCREEN_ID_SET_ALARMS:
            ( save == true ) ? saveConfig() : loadConfig();

            g_clockUpdate = true;

            break;

        case SCREEN_ID_SHOW_ALARMS:
            resetClockDisplay();
            break;

        case SCREEN_ID_SETTINGS:
            if ( save == true ) {
                saveConfig();
            }
            break;

        case SCREEN_ID_ROOT:
            g_lamp.deactivate();

            g_lcd.setCustomCharacters( CUSTOM_CHARACTERS_MENUS );
            break;
    }

    return true;
}




//--------------------------------------------------------------------------
//
// Root screen events
//
//--------------------------------------------------------------------------

bool root_eventKeypress( Screen *screen, uint8_t key ) {

    switch (key) {
        case KEY_MENU:
            gotoScreen( &screen_main_menu, true, &screen_root );
            break;

        case KEY_SET | KEY_SHIFT:
            gotoScreen( &screen_set_time, true, &screen_root );
            break;

        case KEY_ALARM | KEY_SHIFT:
            gotoScreen( &screen_set_alarms, true, &screen_root );
            break;

        case KEY_ALARM:
            gotoScreen( &screen_show_alarms, true, &screen_root );
            break;

        case KEY_SWIPE | KEY_RIGHT:
            g_config.lamp.mode = LAMP_MODE_ON;
            g_lamp.activate( &g_config.lamp );
            break;

        case KEY_SWIPE | KEY_LEFT:
            g_lamp.deactivate();
            break;

        case KEY_SET:
            /* temporary */
            g_alarm.loadProfile( 0 );
            g_alarm.play( ALARM_MODE_NORMAL );

            break;
    }

    return false;
}

bool root_eventDrawScreen( Screen *screen ) {

    char buffer[16];
    DateTime now = rtc.now();


    /* Print status icons */
    g_lcd.setPosition(0, 13);

    g_lcd.print(( g_alarm.isSnoozing() == true ) ? CHAR_ALARM : CHAR_SPACE);
    g_lcd.print(( WiFi.connected() == true) ? CHAR_WIFI_ON : CHAR_SPACE );
    g_lcd.print(( g_alarm.isSDCardPresent() == false ) ? CHAR_NO_SD : CHAR_SPACE);


    /* Print date */
    dateToBuf( buffer, g_config.date_format, &now );

    g_lcd.setPosition(1, 0);
    uint8_t length = g_lcd.print( buffer );
    g_lcd.fill( CHAR_SPACE, DISPLAY_WIDTH - length );

    return false;
}



//--------------------------------------------------------------------------
//
// Show next alarm screen events
//
//--------------------------------------------------------------------------

bool show_alarm_eventKeypress( Screen *screen, uint8_t key ) {

    gotoScreen( &screen_root, true, NULL );
    return false;
}


bool show_alarm_eventDrawScreen( Screen *screen ) {

    /* Get the next alarm id closest from now. */
    DateTime now = rtc.now();
    int8_t alarm_id = g_alarm.getNextAlarmID( &now, false );

    if ( alarm_id == -1 ) {

        g_lcd.print_P( S_ALARMS_OFF );
        return false;
    }

    /* Get alarm next trigger time */
    int16_t alarm_time = g_alarm.getNextAlarmOffset( alarm_id, &now, false );

    g_lcd.printf_P( S_ALARM_IN, alarm_id + 1 );
    g_lcd.setPosition(1, 0);

    if ( alarm_time > 1440 ) {
        g_lcd.printf_P( S_NEXT_ALARM_DHM, alarm_time / 1440, ( alarm_time % 1440 ) / 60, alarm_time % 60 );

    } else if (( alarm_time == 1440 )) {
        g_lcd.print_P( S_NEXT_ALARM_1DAY );

    } else if (( alarm_time % 1440 ) == 0 ) {
        g_lcd.printf_P( S_NEXT_ALARM_DAYS, alarm_time / 1440 );

    } else if ( alarm_time > 60 ) {
        g_lcd.printf_P( S_NEXT_ALARM_HM, alarm_time / 60, alarm_time % 60 );

    } else if ( alarm_time > 1 ) {
        g_lcd.printf_P( S_NEXT_ALARM_M, alarm_time % 60 );

    } else {
        g_lcd.print_P( S_NEXT_ALARM_1MIN );
    }

    return false;
}