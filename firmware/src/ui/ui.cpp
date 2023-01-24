//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui.cpp
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

#include <services/ntpclient.h>
#include <services/mqtt.h>
#include <services/telnet_console.h>
#include <services/homeassistant.h>
#include <time.h>

#include "ui.h"


uint8_t selectedProfile = 0;
uint8_t selectedAlarm = 0;


struct Time adjTime;
struct Date adjDate;


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when a key press occurs
 *
 * @param   screen    Pointer to the screen where the event occured.
 * @param   key       Detected key press.
 *
 * @return  TRUE to allow default key press processing, FALSE to override.
 * 
 */
bool onKeypress( Screen* screen, uint8_t key ) {

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when drawing an item.
 *
 * @param   screen        Pointer to the screen where the event occured.
 * @param   item          Item being drawn.
 * @param   isSelected    TRUE if the item is currently selectedor False otherwise.
 * @param   index         Currently selected index for list item
 * @param   row           Zero-based Y position where the item is located.
 * @param   col           Zero-based X position where the item is located.
 *
 * @return  TRUE to allow default item drawing, FALSE to override.
 * 
 */
bool onDrawItem( Screen* screen, ScreenItem* item, uint16_t index, bool isSelected, uint8_t row, uint8_t col ) {
    uint8_t length;

    switch( item->getId() ) {
        case ID_PROFILE_FILENAME:

            if( g_alarm.profile.filename[0] == 0x00 ) {
                length = g_lcd.print_P( S_PROFILE_DEF_FILENAME );

            } else {
                length = g_lcd.print( g_alarm.profile.filename );
            }

            g_lcd.fill( CHAR_SPACE, item->getLength() - length );

            return false;

        case ID_ALARM_EDIT_1:
        case ID_ALARM_EDIT_2:
            uint8_t alarm_id;
            alarm_id = item->getId() - ID_ALARM_EDIT_1;

            Time time;
            g_alarm.readProfileAlarmTime( alarm_id, &time, NULL );

            char bufTime[9];
            length = timeToBuf( bufTime, g_config.clock.display_24h, &time );

            g_lcd.print( bufTime );

            if( length < 8 ) {
                g_lcd.fill( CHAR_SPACE, 8 - length );
            }

            return false;

        case ID_LAMP_DELAY:
        case ID_PROFILE_SNOOZE:

            uint8_t minutes;
            minutes = item->getValue();

            if( minutes > 1 ) {
                length = g_lcd.printf_P( S_DATETIME_M, minutes );

            } else if( minutes == 1 ) {
                length = g_lcd.print_P( S_DATETIME_1M );

            } else {
                length = g_lcd.print_P( S_OFF );
            }

            g_lcd.fill( CHAR_SPACE, DISPLAY_WIDTH - length );


            return false;
    }

    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when the cursor on the currently selected 
 *          item has changed position or when another item is selected.
 *
 * @param   screen        Pointer to the screen where the event occured.
 * @param   item          Item currently selected.
 * @param   fieldPos      Cursor position within the selected item.
 * @param   fullscreen    TRUE if the item is shown full screenor False otherwise.
 * 
 */
void onSelectionChange( Screen* screen, ScreenItem* item, uint8_t fieldPos, bool fullscreen ) {

    switch( item->getId() ) {
        case ID_PROFILE_FILENAME:
        case ID_PROFILE_VOLUME:

            if( fullscreen ) {
                g_alarm.play( ALARM_MODE_TEST_AUDIO, SELECT_FILE_PLAY_DELAY );

            } else {
                g_alarm.stop();
            }

            break;

        case ID_PROFILE_VISUAL_SPEED:
        case ID_PROFILE_VISUAL_MODE:

            if( fullscreen ) {
                g_alarm.play( ALARM_MODE_TEST_VISUAL );

            } else {
                g_alarm.stop();
            }

            break;

        
            if( g_alarm.profile.lamp.mode == LAMP_MODE_OFF ) {
                g_lamp.deactivate();
                break;
            }

        
        case ID_LAMP_MODE:
            if( g_alarm.profile.lamp.mode != LAMP_MODE_OFF && fullscreen ) {

                g_lamp.activate( &g_alarm.profile.lamp, true );
                break;
            } else {
                g_lamp.deactivate();                
            }

            break;


        case ID_LAMP_EFFECT_SPEED:
        case ID_LAMP_COLOR:
        case ID_LAMP_BRIGHTNESS:

            if( fullscreen ) {

                if ( screen->getId() == SCREEN_ID_EDIT_ALARM_LAMP ) {
                    g_lamp.activate( &g_alarm.profile.lamp, true );

                } else {
                    g_lamp.activate( &g_config.clock.lamp, true, true );
                }

            } else {
                if ( screen->getId() == SCREEN_ID_EDIT_NIGHT_LAMP ) {
                    /* Restore delay off */
                    g_lamp.setDelayOff( g_config.clock.lamp.delay_off );
                }

                g_lamp.deactivate();
            }

            break;


        case ID_SET_TIME:

            g_clock.hourFlashing = ( fieldPos == 0 );
            g_clock.minutesFlashing = ( fieldPos == 1 );

            g_clock.requestClockUpdate( true );

            break;

        default:

            if( g_clock.hourFlashing == true || g_clock.minutesFlashing == true ) {

                g_clock.hourFlashing = false;
                g_clock.minutesFlashing = false;

                g_clock.requestClockUpdate( true );
            }

            break;

    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Raised when a screen item value changes.
 *
 * @param   screen    Pointer to the screen where the event occured.
 * @param   item      Item currently selected.
 * 
 */
void onValueChange( Screen* screen, ScreenItem* item ) {

    

    switch( item->getId() ) {

        case ID_CLOCK_24H:

            g_clock.requestClockUpdate();
            break;

        case ID_CLOCK_COLOR:
            g_clock.setColorFromTable( g_config.clock.clock_color );

            g_clock.requestClockUpdate();
            break;

        case ID_CLOCK_BRIGHTNESS:
            g_clock.setBrightness( g_config.clock.clock_brightness );

            g_clock.requestClockUpdate();
            break;

        case ID_LCD_CONTRAST:
            g_lcd.setContrast( g_config.clock.lcd_contrast );
            break;


        case ID_PROFILE_FILENAME:
            g_alarm.openNextFile();
            g_alarm.play( ALARM_MODE_TEST_AUDIO, SELECT_FILE_PLAY_DELAY );

            break;

        case ID_SET_DATE_DAY:
        case ID_SET_DATE_YEAR:
        case ID_SET_DATE_MONTH:

            uint8_t month_days;
            month_days = getMonthNumDays( adjDate.month, adjDate.year + 2000 );

            if( adjDate.day > month_days ) {

                if( item->getId() == ID_SET_DATE_DAY ) {
                    adjDate.day = 1;

                } else {
                    adjDate.day = month_days;
                }
            }

            break;

        case ID_PROFILE_VOLUME:
            g_alarm.setVolume( g_alarm.profile.volume );

            if( g_alarm.isPlaying() == false ) {

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
            g_clock.requestClockUpdate();
            g_homeassistant.updateSensor( SENSOR_ID_NEXT_ALARM );
            g_homeassistant.updateSensor( SENSOR_ID_NEXT_ALARM_AVAILABLE );
            break;

        case ID_ALARM_EDIT_1:
        case ID_ALARM_EDIT_2:
            selectedProfile = item->getId() - ID_ALARM_EDIT_1;

            g_alarm.loadProfile( selectedProfile );
            break;

        case ID_SET_TIME:
            g_clock.hour = adjTime.hour;
            g_clock.minute = adjTime.minute;

            g_clock.requestClockUpdate( true );
            break;

        case ID_LAMP_DELAY:
        case ID_PROFILE_SNOOZE:
            uint8_t minutes;
            minutes = item->getValue();

            if( minutes > 10 ) {
                minutes = ( minutes % 5 ) ? ( ( minutes / 5 ) * 5 ) + 5  : minutes;
            }

            if( minutes > 30 ) {
                minutes = ( minutes % 10 ) ? ( ( minutes / 10 ) * 10 ) + 10  : minutes;
            }

            item->setValue( minutes );

            if( item->getId() == ID_LAMP_DELAY && screen->getId() == SCREEN_ID_EDIT_NIGHT_LAMP ) {
                g_lamp.setDelayOff( minutes );
            }
            break;


        case ID_LAMP_MODE:
            if( item->getValue() == LAMP_MODE_OFF ) {
                g_lamp.deactivate();

            } else {
                g_lamp.activate( screen->getId() == SCREEN_ID_EDIT_ALARM_LAMP ? &g_alarm.profile.lamp : &g_config.clock.lamp, true );
            }

            break;

        case ID_LAMP_COLOR:
            g_lamp.setColorFromTable( item->getValue(), screen->getId() == SCREEN_ID_EDIT_NIGHT_LAMP );
            break;

        case ID_LAMP_BRIGHTNESS:
            g_lamp.setBrightness( item->getValue(), screen->getId() == SCREEN_ID_EDIT_NIGHT_LAMP );
            break;

        case ID_LAMP_EFFECT_SPEED:
            g_lamp.setEffectSpeed( item->getValue() );
            break;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when entering the screen
 *
 * @param   screen    Pointer to the new screen.
 * 
 */
void onEnterScreen( Screen* screen, uint8_t prevScreenID ) {

    DateTime now;

    switch( screen->getId() ) {

        case SCREEN_ID_SET_TIME:
            screen->setConfirmChanges( true );

            now = g_rtc.now();
            g_timezone.toLocal( &now );

            adjTime.hour = now.hour();
            adjTime.minute = now.minute();
            adjDate.day = now.day();
            adjDate.month = now.month();
            adjDate.year = ( uint8_t )( now.year() - 2000 );

            g_clock.status_set = true;
            g_clock.requestClockUpdate();

            break;

        case SCREEN_ID_NETWORK:
        case SCREEN_ID_SERVICES:
            screen->setConfirmChanges( true );
            break;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when leaving the screen
 *
 * @param   screen    Current screen.
 *
 * @return  TRUE to allow leaving the screen, FALSE to override.
 * 
 */
bool onExitScreen( Screen* screen  ) {

    bool save;

    if( screen->getConfirmChanges() == true ) {
        save = ( screen->getReturnValue() == RETURN_YES );

    } else {

        save = screen->hasItemsChanged();
    }


    switch( screen->getId() ) {

        case SCREEN_ID_MAIN_MENU:

            if( save == true ) {
                g_config.save( EEPROM_SECTION_CLOCK );
                g_config.apply( EEPROM_SECTION_CLOCK );
                
            } else {
                g_config.load( EEPROM_SECTION_CLOCK );
            }
            break;

        case SCREEN_ID_SET_TIME:

            if( save == true ) {

                DateTime nDate( (uint16_t)adjDate.year + 2000, adjDate.month, adjDate.day,
                                adjTime.hour, adjTime.minute, 0 );

                /* Disable ntp auto-sync when setting the time manually */
                g_config.clock.use_ntp = false;
                g_ntp.setAutoSync( false );
                g_config.save( EEPROM_SECTION_CLOCK );

                /* Convert local time to UTC */
                g_timezone.toUTC( &nDate );

                /* Update the RTC */
                g_rtc.writeTime( &nDate );

                /* Update the time on the wifi module */
                g_wifi.setSystemTime( &nDate );
            }

            g_clock.restoreClockDisplay();
            break;

        case SCREEN_ID_NETWORK:
            if( save == true ) {
                g_config.save( EEPROM_SECTION_ALL );
                g_config.apply( EEPROM_SECTION_ALL );

            } else {
                g_config.load( EEPROM_SECTION_ALL );
            }

            break;

        case SCREEN_ID_SERVICES:
            if( save == true ) {
                g_config.save( EEPROM_SECTION_ALL );

                g_ntp.setAutoSync( g_config.clock.use_ntp );
                g_telnetConsole.enableServer( g_config.network.telnetEnabled );
                g_mqtt.enableClient( g_config.network.mqtt_enabled );

            } else {
                g_config.load( EEPROM_SECTION_ALL );
            }

            break;

        case SCREEN_ID_EDIT_NIGHT_LAMP:
            if( save == true ) {
                g_config.save( EEPROM_SECTION_CLOCK );

            } else {
                g_config.load( EEPROM_SECTION_CLOCK );
            }

            break;

        case SCREEN_ID_EDIT_ALARM_LAMP:
        case SCREEN_ID_EDIT_ALARM:
        case SCREEN_ID_EDIT_PROFILE:
        case SCREEN_ID_EDIT_ALARM_VISUAL:
            if( save == true ) {
                g_alarm.saveProfile( &g_alarm.profile, selectedProfile );
                g_homeassistant.updateSensor( SENSOR_ID_NEXT_ALARM );
                g_homeassistant.updateSensor( SENSOR_ID_NEXT_ALARM_AVAILABLE );
            }

            break;

        case SCREEN_ID_SET_ALARMS:
            if( save == true ) {
                g_config.save( EEPROM_SECTION_CLOCK );
                g_homeassistant.updateSensor( SENSOR_ID_NEXT_ALARM );
                g_homeassistant.updateSensor( SENSOR_ID_NEXT_ALARM_AVAILABLE );

            } else {
                g_config.load( EEPROM_SECTION_CLOCK );
            }

            g_clock.requestClockUpdate();

            break;

        case SCREEN_ID_DISPLAY_SETTINGS:
            if( save == true ) {
                g_config.save( EEPROM_SECTION_CLOCK );
            }

            break;
    }

    return true;
}