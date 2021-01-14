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
#include "ui.h"
#include "../services/ntpclient.h"


bool g_clockUpdate = true;
uint8_t selectedProfile = 0;
uint8_t selectedAlarm = 0;


struct Time adjTime;
struct Date adjDate;


Screen screen_root( SCREEN_ID_ROOT, NULL, NULL, &onEnterScreen, &onExitScreen );
Screen screen_main_menu( SCREEN_ID_MAIN_MENU, ITEMS_MAIN_MENU,
                         &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_display( SCREEN_ID_DISPLAY_SETTINGS, ITEMS_DISPLAY_SETTINGS,
                       &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_network( SCREEN_ID_NETWORK, ITEMS_NETWORK,
                       NULL, &onEnterScreen, &onExitScreen );

Screen screen_set_time( SCREEN_ID_SET_TIME, ITEMS_SET_TIME,
                        &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_alarm( SCREEN_ID_ALARM, NULL,
                     NULL, &alarmScreen_onEnterScreen, NULL );

Screen screen_set_alarms( SCREEN_ID_SET_ALARMS, ITEMS_SET_ALARM,
                          &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_edit_alarm( SCREEN_ID_EDIT_ALARM, ITEMS_EDIT_ALARM,
                          &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_show_alarms( SCREEN_ID_SHOW_ALARMS, NULL,
                           NULL, &onEnterScreen, &onExitScreen );

Screen screen_edit_profile( SCREEN_ID_EDIT_PROFILE, ITEMS_EDIT_PROFILE,
                            &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_list_profiles( SCREEN_ID_LIST_PROFILES, ITEMS_LIST_PROFILES,
                             &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_edit_night_lamp( SCREEN_ID_EDIT_NIGHT_LAMP, ITEMS_EDIT_NIGHT_LAMP,
                               &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_edit_alarm_lamp( SCREEN_ID_EDIT_ALARM_LAMP, ITEMS_EDIT_ALARM_LAMP,
                               &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_edit_alarm_visual( SCREEN_ID_EDIT_ALARM_VISUAL, ITEMS_EDIT_PROFILE_VISUAL,
                                 &onValueChange, &onEnterScreen, onExitScreen );

Screen screen_menu_settings( SCREEN_ID_MENU_SETTINGS, ITEMS_MENU_SETTINGS,
                             NULL, NULL, NULL );

Screen screen_settings_manager( SCREEN_ID_SETTINGS_MANAGER, ITEMS_DIALOG_YESNO,
                                NULL, &settingsManager_onEnterScreen, NULL );

Screen screen_batt_status( SCREEN_ID_BATT_STATUS, NULL,
                           NULL, &battStatus_onEnterScreen, NULL );

Screen screen_net_status( SCREEN_ID_NET_STATUS, NULL,
                           NULL, &netStatus_onEnterScreen, NULL );

/*--------------------------------------------------------------------------
 *
 * Initialize screens settings.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns :
 */
void initScreens() {

    screen_main_menu.setConfirmChanges( true );

    screen_alarm.setCbDrawScreen( &alarmScreen_onDrawScreen );
    screen_alarm.setCbKeypress( &alarmScreen_onKeypress );
    screen_alarm.setCbTimeout( &alarmScreen_onTimeout );
    screen_alarm.setTimeout( 3000 );

    /* Root screen */
    screen_root.setCbDrawScreen( &rootScreen_onDrawScreen );
    screen_root.setCbKeypress( &rootScreen_onKeypress );
    screen_root.setCbTimeout( &rootScreen_onTimeout );
    screen_root.setTimeout( 1500 );
    screen_root.setCustomCharacterSet( CUSTOM_CHARACTERS_ROOT );

    /* Set time screen */
    screen_set_time.setConfirmChanges( true );
    screen_set_time.setCbSelectionChange( &onSelectionChange );

    /* Set alarm screen */
    screen_set_alarms.setCbDrawItem( &onDrawItem );

    /* Show alarms screen */
    screen_show_alarms.setTimeout( 3000 );
    screen_show_alarms.setCbKeypress( &showAlarmScreen_onKeypress );
    screen_show_alarms.setCbDrawScreen( &showAlarmScreen_onDrawScreen );

    /* Network settings screen */
    screen_network.setConfirmChanges( true );

    /* List profile screen */
    screen_edit_profile.setCbSelectionChange( &onSelectionChange );
    screen_edit_profile.setCbDrawItem( &onDrawItem );

    /* Edit alarm lamp settings */
    screen_edit_alarm_lamp.setCbSelectionChange( &onSelectionChange );
    screen_edit_alarm_lamp.setCbKeypress( &onKeypress );

    /* Edit night lamp settings */
    screen_edit_night_lamp.setCbSelectionChange( &onSelectionChange );
    screen_edit_night_lamp.setCbKeypress( &onKeypress );
    screen_edit_night_lamp.setCbDrawItem( &onDrawItem );

    /* Edit alarm visual settings */
    screen_edit_alarm_visual.setCbSelectionChange( &onSelectionChange );


    screen_menu_settings.setCbValueChange( &settingsMenu_onValueChange );

    screen_settings_manager.setCbDrawScreen( &settingsManager_onDrawScreen );
    screen_settings_manager.setCbKeypress( &settingsManager_onKeypress );

    screen_batt_status.setCbDrawScreen( &battStatus_onDrawScreen );
    screen_batt_status.setCbTimeout( &battStatus_onTimeout );
    screen_batt_status.setCbKeypress( &battStatus_onKeypress );
    screen_batt_status.setCustomCharacterSet( CUSTOM_CHARACTERS_ROOT );

    screen_net_status.setCbDrawScreen( &netStatus_onDrawScreen );
    screen_net_status.setCbKeypress( &netStatus_onKeypress );
    screen_net_status.setCustomCharacterSet( CUSTOM_CHARACTERS_ROOT );

    g_currentScreen = &screen_root;
    g_screenUpdate = true;
    g_screenClear = true;
}

/*--------------------------------------------------------------------------
 *
 * Event raised when a key press occurs
 *
 * Arguments
 * ---------
 *  - screen : Pointer to the screen where the event occured.
 *  - key    : Detected key press.
 *
 * Returns : TRUE to allow default key press processing or False to override.
 */
bool onKeypress( Screen* screen, uint8_t key ) {

    return true;
}

/*--------------------------------------------------------------------------
 *
 * Event raised when drawing an item.
 *
 * Arguments
 * ---------
 *  - screen     : Pointer to the screen where the event occured.
 *  - item       : Item being drawn.
 *  - isSelected : TRUE if the item is currently selectedor False otherwise.
 *  - row        :  Zero-based Y position where the item is located.
 *  - col        :  Zero-based X position where the item is located.
 *
 * Returns : TRUE to allow default item drawingor False to override.
 */
bool onDrawItem( Screen* screen, ScreenItem* item, bool isSelected, uint8_t row, uint8_t col ) {
    uint8_t length;

    switch( item->getId() ) {
        case ID_PROFILE_FILENAME:

            g_lcd.print( isSelected ? CHAR_SELECT : CHAR_FIELD_BEGIN );

            if( g_alarm.profile.filename[0] == 0x00 ) {
                length = g_lcd.print_P( S_PROFILE_DEF_FILENAME );

            } else {
                length = g_lcd.print( g_alarm.profile.filename );
            }

            g_lcd.fill( CHAR_SPACE, item->getLength() - length );
            g_lcd.print( isSelected ? CHAR_SELECT_REV : CHAR_FIELD_END );

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


/*--------------------------------------------------------------------------
 *
 * Event raised when the cursor on the currently selected item has changed
 * position or when another item is selected.
 *
 * Arguments
 * ---------
 *  - screen     : Pointer to the screen where the event occured.
 *  - item       : Item currently selected.
 *  - fieldPos   : Cursor position within the selected item.
 *  - fullscreen : TRUE if the item is shown full screenor False otherwise.
 *
 * Returns : Nothing
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

            g_clockUpdate = true;

            break;

        default:

            if( g_clock.hourFlashing == true || g_clock.minutesFlashing == true ) {

                g_clock.hourFlashing = false;
                g_clock.minutesFlashing = false;

                g_clockUpdate = true;
            }

            break;

    }
}


/*--------------------------------------------------------------------------
 *
 * Raised when a screen item value changes.
 *
 * Arguments
 * ---------
 *  - screen     : Pointer to the screen where the event occured.
 *  - item       : Item currently selected.
 *
 * Returns : Nothing
 */
void onValueChange( Screen* screen, ScreenItem* item ) {

    

    switch( item->getId() ) {

        case ID_CLOCK_24H:

            g_clockUpdate = true;
            break;

        case ID_CLOCK_COLOR:
            g_clock.setColorFromTable( g_config.clock.clock_color );

            g_clockUpdate = true;
            break;

        case ID_CLOCK_BRIGHTNESS:
            g_clock.setBrightness( g_config.clock.clock_brightness );

            g_clockUpdate = true;
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
            g_clockUpdate = true;
            break;

        case ID_ALARM_EDIT_1:
        case ID_ALARM_EDIT_2:
            selectedProfile = item->getId() - ID_ALARM_EDIT_1;

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


/*--------------------------------------------------------------------------
 *
 * Event raised when entering the screen
 *
 * Arguments
 * ---------
 *  - screen     : Pointer to the new screen.
 *
 * Returns : TRUE to allow loading the screenor False to override.
 */
bool onEnterScreen( Screen* screen ) {

    DateTime now;

    switch( screen->getId() ) {

        case SCREEN_ID_SET_TIME:

            now = g_rtc.now();
            g_timezone.toLocal( &now );

            adjTime.hour = now.hour();
            adjTime.minute = now.minute();
            adjDate.day = now.day();
            adjDate.month = now.month();
            adjDate.year = ( uint8_t )( now.year() - 2000 );

            g_clock.status_set = true;
            g_clockUpdate = true;

            break;

        case SCREEN_ID_SHOW_ALARMS:

            int8_t alarm_id;
            alarm_id = g_alarm.getNextAlarmID( g_rtc.now(), false );

            Time time;
            g_alarm.readProfileAlarmTime( alarm_id, &time, NULL );

            if( alarm_id != -1 ) {

                g_clock.hour = time.hour;
                g_clock.minute = time.minute;

                g_clockUpdate = true;
            }
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Event raised when leaving the screen
 *
 * Arguments
 * ---------
 *  - currentScreen : Current screen.
 *  - newScreen     : The new screen to be loaded.
 *
 * Returns : TRUE to allow leaving the screenor False to override.
 */
bool onExitScreen( Screen* currentScreen, Screen* newScreen ) {

    bool save;

    if( currentScreen->getConfirmChanges() == true ) {
        save = ( currentScreen->getReturnValue() == RETURN_YES );

    } else {

        save = currentScreen->hasItemsChanged();
    }


    switch( currentScreen->getId() ) {

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

                uint8_t dow;
                dow = getDayOfWeek( adjDate.year, adjDate.month, adjDate.day );

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
                g_config.save( EEPROM_SECTION_NETWORK );
                g_config.apply( EEPROM_SECTION_NETWORK );

            } else {
                g_config.load( EEPROM_SECTION_NETWORK );
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
            }

            break;

        case SCREEN_ID_SET_ALARMS:
            if( save == true ) {
                g_config.save( EEPROM_SECTION_CLOCK );
            } else {
                g_config.load( EEPROM_SECTION_CLOCK );
            }

            g_clockUpdate = true;

            break;

        case SCREEN_ID_SHOW_ALARMS:
            g_clock.restoreClockDisplay();
            break;

        case SCREEN_ID_DISPLAY_SETTINGS:
            if( save == true ) {
                g_config.save( EEPROM_SECTION_CLOCK );
            }

            break;
    }

    return true;
}

