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
#include "alarm.h"
#include "resources.h"
#include "screen.h"
#include "config.h"
#include "qt1070.h"
#include "us2066.h"
#include "neoclock.h"
#include "tpa2016.h"


Alarm g_alarm( PIN_VS1053_RESET, PIN_VS1053_CS, PIN_VS1053_XDCS, PIN_VS1053_DREQ, PIN_VS1053_SDCS, PIN_SD_DETECT );
TPA2016 g_amplifier( PIN_AMP_SHDN );
NeoClock g_clock( PIN_NEOCLOCK );
QT1070 g_keypad( PIN_INT_KEYPAD );
US2066 g_lcd( I2C_ADDR_OLED, PIN_OLED_RESET, PIN_OLED_VEN );

uint8_t test = 0;

bool g_clockUpdate = true;
uint8_t selectedProfile = 0;
uint8_t selectedAlarm = 0;

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
Screen screen_list_profiles( SCREEN_ID_LIST_PROFILES, NULL, NULL, &enterScreen, &exitScreen );


void initScreens() {

    /* Root screen */
    screen_root.eventDrawScreen = &root_eventDrawScreen;
    screen_root.eventKeypress = &root_eventKeypress;

    /* Set time screen */
    screen_set_time.confirmChanges = true;
    screen_set_time.eventSelectionChanged = &eventSelectionChange;


    screen_set_alarms.eventDrawItem = &eventDrawItem;
    screen_set_alarms.eventKeypress = &set_alarms_eventKeypress;



    /* Show alarms screen */
    screen_show_alarms.timeout = 3000;
    screen_show_alarms.eventKeypress = &show_alarm_eventKeypress;
    screen_show_alarms.eventDrawScreen = &show_alarm_eventDrawScreen;

    /* Network settings screen */
    screen_network.confirmChanges = true;

    /* List profile screen */
    screen_list_profiles.eventDrawScreen = &listprofiles_eventDrawScreen;
    screen_list_profiles.eventKeypress = &listprofiles_eventKeypress;

    screen_edit_profile.eventSelectionChanged = &eventSelectionChange;
    screen_edit_profile.eventDrawItem = &eventDrawItem;




}

void configWiFi() {

    IPAddress net_ip( &g_config.net_ip[0] );
    IPAddress net_mask( &g_config.net_mask[0] );
    IPAddress net_gateway( &g_config.net_gateway[0] );
    IPAddress net_dns( &g_config.net_dns[0] );

    WiFi.config( g_config.net_dhcp, net_ip, net_dns, net_gateway, net_mask );


}


void setup() {

    Serial.begin( 115200 );

    /* Initialize clock display */
    g_clock.begin();
    


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


    /* Setup audio codec */
    g_alarm.begin();


    /* Initialize I2C Bus and DS3231 RTC */
    pinMode( PIN_INT_RTC, INPUT_PULLUP );
    attachInterrupt( digitalPinToInterrupt( PIN_INT_RTC ), isr_rtc, FALLING );

    rtc.begin();
    rtc.enableInterrupts(EveryMinute);

    rtc_event = true;   /* Force rtc event to update clock display */


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


    /* Feed alarm audio buffer and process clock visual effect */
    g_alarm.processAlarm();


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


    /* Handle pending WiFi module events */
    WiFi.handleEvents();

    if ( WiFi.statusChanged() == true ) {

        if ( g_currentScreen->id == SCREEN_ID_ROOT )  {
            g_screenUpdate = true;
        }
    }


    /* Detect if the SD card is present, if so, initialize it */
    bool sd_present = g_alarm.isSDCardPresent();

    if ( sd_present != g_alarm.DetectSDCard() ) {
        g_screenUpdate = true;
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
        case ID_ALARM_PROFILE_1:
        case ID_ALARM_PROFILE_2:

            char buffer[ ALARM_PROFILE_NAME_LENGTH + 1 ];
            g_alarm.readProfileName( *(( uint8_t* )item->value ), buffer );

            g_lcd.print( isSelected ? CHAR_SELECT : CHAR_FIELD_BEGIN );

            length = g_lcd.print( buffer );

            g_lcd.fill ( CHAR_SPACE, item->length - length );
            g_lcd.print( isSelected ? CHAR_SELECT_REV : CHAR_FIELD_END );

            return false;

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
            g_alarm.readProfileAlarmTime( g_config.alarm_profile_id[ alarm_id ], &time, NULL );

            char bufTime[9];
            length = timeToBuf( bufTime, g_config.clock_24h, &time);

            g_lcd.print( bufTime );
            if ( length < 8 ) {
                g_lcd.fill( CHAR_SPACE, 8 - length );
            }

            return false;

        case ID_PROFILE_SNOOZE:

            uint8_t snooze;
            snooze = *(( uint8_t* )item->value );

            if (snooze > 1 ) {
                length = g_lcd.printf_P( S_NEXT_ALARM_M, snooze);
            } else if (snooze == 1 ) {
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

        case ID_PROFILE_VISUAL:

            if ( fullscreen ) {
                g_alarm.play( ALARM_MODE_TEST_VISUAL );
            } else {
                g_alarm.stop();
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

        case ID_PROFILE_VOLUME:
            g_alarm.setVolume( g_alarm.profile.volume );

            if ( g_alarm.isPlaying() == false ) {
                g_alarm.play( ALARM_MODE_TEST_AUDIO, SELECT_FILE_PLAY_DELAY );
            }
            break;

        case ID_PROFILE_TEST:
            g_alarm.play( ALARM_MODE_TEST_PROFILE );
            break;

        case ID_PROFILE_VISUAL:
            g_alarm.play( ALARM_MODE_TEST_VISUAL );
            break;

        case ID_ALARM_ON_1:
        case ID_ALARM_ON_2:
            g_clockUpdate = true;
            break;

        case ID_ALARM_EDIT_1:
        case ID_ALARM_EDIT_2:
            uint8_t alarm_id;
            alarm_id = item->id - ID_ALARM_EDIT_1;

            selectedProfile = g_config.alarm_profile_id[ alarm_id ];

            g_alarm.loadProfile( selectedProfile );
            break;

        case ID_SET_TIME:
            g_clock.hour = adjTime.hour;
            g_clock.minute = adjTime.minute;
            
            g_clockUpdate = true;
            break;

        case ID_PROFILE_SNOOZE:
            uint8_t snooze;
            snooze = *(( uint8_t* )item->value );

            if ( snooze > 10 ) {
                snooze = ( snooze % 5 ) ? (( snooze / 5 ) * 5 ) + 5  : snooze;
            }

            *(( uint8_t* )item->value ) = snooze;

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
            g_alarm.readProfileAlarmTime( g_config.alarm_profile_id[ alarm_id ], &time, NULL );

            if ( alarm_id != -1 ) {

                g_clock.hour = time.hour;
                g_clock.minute = time.minute;

                g_clockUpdate = true;
            }

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

                DateTime nDate( 2000 + adjDate.year,
                                adjDate.month,
                                adjDate.day,
                                adjTime.hour,
                                adjTime.minute,
                                0,
                                getDayOfWeek( 2000 + adjDate.year, adjDate.month, adjDate.day ));

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

        case KEY_SET:
            /* temporary */
            g_alarm.loadProfile( g_config.alarm_profile_id[ 0 ]);
            g_alarm.play( ALARM_MODE_NORMAL );

            break;
    }

    return false;
}

bool root_eventDrawScreen( Screen *screen ) {

    char buffer[16];
    DateTime now = rtc.now();
    double temperature = rtc.getTemperature();

    /* Print temperature from DS3231 */
    if ( g_config.tempunit_c == false ) {
        temperature = ( temperature * 1.8 ) + 32;
    }

    dtostrf( temperature, 3, 1, buffer );

    g_lcd.setPosition( 0, 0 );
    g_lcd.print( buffer );
    g_lcd.print( CHAR_DEGREE );
    g_lcd.print( g_config.tempunit_c == true ? CHAR_CELCIUS : CHAR_FARENHEIGHT );


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



//--------------------------------------------------------------------------
//
// Set alarms screen events
//
//--------------------------------------------------------------------------

bool set_alarms_eventKeypress( Screen *screen, uint8_t key ) {

    switch ( key ) {
        case KEY_SET | KEY_SHIFT:

            if ( g_currentItem.id == ID_ALARM_PROFILE_1 ) {
                selectedProfile = g_config.alarm_profile_id[ 0 ];

            } else if ( g_currentItem.id == ID_ALARM_PROFILE_2 ) {
                selectedProfile = g_config.alarm_profile_id[ 1 ];

            } else {
                return true;
            }

            g_alarm.loadProfile( selectedProfile );
            gotoScreen( &screen_edit_profile, true, &screen_set_alarms );
            return false;
    }

    return true;
}





//--------------------------------------------------------------------------
//
// List profiles screen events
//
//--------------------------------------------------------------------------

bool listprofiles_eventKeypress( Screen *screen, uint8_t key ) {

    switch ( key ) {
        case KEY_SET:

            selectedProfile = screen->selected;
            g_alarm.loadProfile( selectedProfile );

            gotoScreen( &screen_edit_profile, true, &screen_list_profiles );
            break;

        case KEY_NEXT:
            screen->selected++;

            g_screenUpdate = true;
            g_screenClear = ( screen->selected > DISPLAY_HEIGHT - 1 );

            if ( screen->selected > MAX_ALARM_PROFILES - 1 ) {
                screen->selected = 0;
            }
            break;

        default:

            /* Continue to process event for other keys */
            return true;
    }

    return false;
}


bool listprofiles_eventDrawScreen( Screen *screen ) {
    uint8_t scroll = ( screen->selected > 1 ) ? screen->selected - 1 : 0;

    char buffer[ ALARM_PROFILE_NAME_LENGTH + 1 ];

    for ( uint8_t i = 0; i < DISPLAY_HEIGHT; i++ ) {
        g_lcd.setPosition( i, 0 );

        itoa( scroll + i + 1, buffer, 10 );

        g_lcd.print(( ( scroll + i) == screen->selected ) ? CHAR_SELECT : CHAR_SPACE );
        g_lcd.print( buffer );
        g_lcd.print( "." );

        g_alarm.readProfileName( scroll + i, buffer );
        g_lcd.print( buffer);

    }

    return false;
}
