//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_settings_manager.cpp
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
#include "../drivers/power.h"
#include "../alarm.h"


uint8_t menuSettings_selectedItem = 0;
bool confirm_action = false;


void settingsMenu_onValueChange( Screen* screen, ScreenItem* item ) {
    menuSettings_selectedItem = item->getId();
}



bool settingsManager_onEnterScreen( Screen* screen ) {

    screen->setTimeout( 0 );
    confirm_action = false;

    if( menuSettings_selectedItem == ID_SETTINGS_BACKUP ) {
        g_config.startBackup( false );

        if( g_config.getTaskError() != TASK_SUCCESS && g_config.getTaskError() != TASK_ERROR_CANT_OPEN ) {
            confirm_action = true;
        }
    }

    return true;
}


bool settingsManager_onDrawScreen( Screen* screen ) {

    if( confirm_action == false ) {
        switch( menuSettings_selectedItem ) {
            case ID_SETTINGS_RESTORE:
                g_lcd.print_P( S_CONFIRM_SETTINGS_RESTORE );
                return true;

            case ID_SETTINGS_BACKUP:
                g_lcd.print_P( S_CONFIRM_OVERWRITE );
                return true;

            case ID_SETTINGS_FACTORY_RESET:
                g_lcd.print_P( S_CONFIRM_SETTINGS_RESET );
                return true;
        }

    } else {
        g_lcd.clear();

        switch( g_config.getCurrentTask() ) {
            case TASK_RESTORE_CONFIG:
                g_lcd.print_P( S_STATUS_RESTORING, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                break;

            case TASK_BACKUP_CONFIG:
                g_lcd.print_P( S_STATUS_SAVING, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                break;

            case TASK_NONE:

                /* Print the task result */
                switch( g_config.getTaskError() ) {
                    case TASK_ERROR_NO_SDCARD:
                        g_lcd.print_P( S_STATUS_ERROR_NO_SDCARD, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                        break;

                    case TASK_ERROR_CANT_OPEN:
                    case TASK_ERROR_WRITE:
                        g_lcd.print_P( S_STATUS_ERROR_WRITE, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                        break;

                    case TASK_ERROR_READ:
                        g_lcd.print_P( S_STATUS_ERROR_READ, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                        break;

                    case TASK_ERROR_NOT_FOUND:
                        g_lcd.print_P( S_STATUS_ERROR_NOTFOUND, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                        break;

                    case TASK_SUCCESS:
                        g_lcd.print_P( S_STATUS_DONE, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                        break;

                    default:
                        g_lcd.print_P( S_STATUS_ERROR_UNKNOWN, DISPLAY_WIDTH, TEXT_ALIGN_CENTER );
                        break;
                }

                screen->setTimeout( 3000 );
                break;
        }
    }

    return false;
}

bool settingsManager_onKeypress( Screen* screen, uint8_t key ) {

    if( g_config.isBusy() ) {
        return false;
    }

    if( confirm_action == true ) {
        screen->exitScreen();
        return true;
    }

    if( key != KEY_SET ) {
        return true;
    }

    uint8_t dialogSelectedId = screen->getSelectedItemId();

    if( dialogSelectedId == ID_DIALOG_NO ) {
        screen->exitScreen();
        return false;
    }

    switch( menuSettings_selectedItem ) {

        case ID_SETTINGS_BACKUP:
            g_config.startBackup( true );
            break;

        case ID_SETTINGS_RESTORE:
            g_config.startRestore();
            break;


        case ID_SETTINGS_FACTORY_RESET:
            g_config.formatEeprom();
            g_power.cpuReset();
            break;

    }

    confirm_action = true;
    g_screenUpdate = true;
    g_screenClear = true;

    return false;
}