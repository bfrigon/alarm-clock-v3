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



bool g_clockUpdate = true;
uint8_t selectedProfile = 0;
uint8_t selectedAlarm = 0;
bool edit_alarm_lamp_settings = false;


struct Time adjTime;
struct Date adjDate;


Screen screen_root( SCREEN_ID_ROOT, NULL, NULL, &onEnterScreen, &onExitScreen );
Screen screen_main_menu( SCREEN_ID_MAIN_MENU, ITEMS_MAIN_MENU,
                         &onValueChange, &onEnterScreen, &onExitScreen );

Screen screen_display( SCREEN_ID_SETTINGS, ITEMS_DISPLAY_SETTINGS,
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

    screen_alarm.setCbDrawScreen( &alarmScreen_onDrawScreen );
    screen_alarm.setCbKeypress( &alarmScreen_onKeypress );
    screen_alarm.setCbTimeout( &alarmScreen_onTimeout );
    screen_alarm.setTimeout( 3000 );

    /* Root screen */
    screen_root.setCbDrawScreen( &rootScreen_onDrawScreen );
    screen_root.setCbKeypress( &rootScreen_onKeypress );
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

    /* Edit night lamp settings */
    screen_edit_night_lamp.setCbSelectionChange( &onSelectionChange );
    screen_edit_night_lamp.setCbDrawItem( &onDrawItem );

    /* Edit alarm visual settings */
    screen_edit_alarm_visual.setCbSelectionChange( &onSelectionChange );


    g_currentScreen = &screen_root;
    g_screenUpdate = true;
    g_screenClear = true;
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
            length = timeToBuf( bufTime, g_config.clock_24h, &time );

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

        case ID_LAMP_MODE:
            if( g_alarm.profile.lamp.mode == LAMP_MODE_OFF ) {
                g_lamp.deactivate();
                break;
            }

        /* Fall through */

        case ID_LAMP_EFFECT_SPEED:
        case ID_LAMP_COLOR:
        case ID_LAMP_BRIGHTNESS:

            NightLampSettings* settings;
            settings = edit_alarm_lamp_settings ? &g_alarm.profile.lamp : &g_config.lamp;


            if( fullscreen ) {
                g_lamp.activate( settings, true );

            } else {
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
            break;


        case ID_LAMP_MODE:
            if( item->getValue() == LAMP_MODE_OFF ) {
                g_lamp.deactivate();

            } else {
                g_lamp.activate( edit_alarm_lamp_settings == true ? &g_alarm.profile.lamp : &g_config.lamp );
            }

            break;

        case ID_LAMP_COLOR:
            g_lamp.setColorFromTable( item->getValue() );
            break;

        case ID_LAMP_BRIGHTNESS:
            g_lamp.setBrightness( item->getValue() );
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

            adjTime.hour = now.hour();
            adjTime.minute = now.minute();
            adjDate.day = now.date();
            adjDate.month = now.month();
            adjDate.year = ( uint8_t )( now.year() - 2000 );

            g_clock.status_set = true;
            g_clockUpdate = true;

            break;

        case SCREEN_ID_SHOW_ALARMS:

            now = g_rtc.now();

            int8_t alarm_id;
            alarm_id = g_alarm.getNextAlarmID( &now, false );

            Time time;
            g_alarm.readProfileAlarmTime( alarm_id, &time, NULL );

            if( alarm_id != -1 ) {

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

        case SCREEN_ID_SET_TIME:

            if( save == true ) {

                uint8_t dow;
                dow = getDayOfWeek( adjDate.year, adjDate.month, adjDate.day );

                DateTime nDate( adjDate.year + 2000, adjDate.month, adjDate.day,
                                adjTime.hour, adjTime.minute, 0, dow );

                /* Update the RTC */
                g_rtc.setDateTime( nDate );
            }

            resetClockDisplay();
            break;

        case SCREEN_ID_NETWORK:
            if( save == true ) {
                saveConfig();

                updateWifiConfig();

            } else {
                loadConfig();
            }

            break;

        case SCREEN_ID_EDIT_NIGHT_LAMP:
            if( save == true ) {
                saveConfig();

            } else {
                loadConfig();
            }

            g_lamp.deactivate();
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
            ( save == true ) ? saveConfig() : loadConfig();

            g_clockUpdate = true;

            break;

        case SCREEN_ID_SHOW_ALARMS:
            resetClockDisplay();
            break;

        case SCREEN_ID_SETTINGS:
            if( save == true ) {
                saveConfig();
            }

            break;

        case SCREEN_ID_ROOT:
            g_lamp.deactivate();
            break;
    }

    return true;
}