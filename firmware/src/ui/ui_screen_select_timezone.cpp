//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/ui_screen_select_timezone.cpp
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

#include <timezone.h>
#include <tzdata.h>

#include "ui.h"


uint16_t g_selectedTimezone;

/*! ------------------------------------------------------------------------
 *
 * @brief   Event raised when entering the screen
 *
 * @param   screen    Pointer to the screen where the event occured.
 *
 */
void selectTimezone_onEnterScreen( Screen* screen, uint8_t prevScreenID ) {

    screen->setConfirmChanges( true );
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
void selectTimezone_onSelectionChange( Screen* screen, ScreenItem* item, uint8_t fieldPos, bool fullscreen ) {

    if( screen->hasItemsChanged() == true ) {
        screen->exitScreen();
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
bool selectTimezone_onExitScreen( Screen* screen  ) {

    if( screen->getReturnValue() == RETURN_YES ) {

        g_timezone.setTimezoneByID( g_selectedTimezone );
        strcpy_P( g_config.clock.timezone, g_timezone.getName() );

        g_config.save( EEPROM_SECTION_CLOCK );
    
        /* Request clock display update */
        g_clock.requestClockUpdate();

    } else if( screen->getReturnValue() == RETURN_NO ) {

        return false;
    }

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
bool selectTimezone_onDrawItem( Screen* screen, ScreenItem* item, uint16_t index, bool isSelected, uint8_t row, uint8_t col ) {

    if( index > MAX_TIMEZONE_ID - 1 ) {
        return true;
    }

    TimeZone tz;
    if( tz.setTimezoneByID( index ) == false ) {
        return true;
    }

    char buffer[40];
    strcpy_P( buffer, tz.getName() );

    /* Convert underscore character to spaces */
    char* ptr;
    while(( ptr = strchr( buffer, '_' )) != NULL )  {
        *ptr = '\x20';
    }

    /* Get only the last segment of the timezone name */
    char* caption = strrchr( buffer, '/' );
    if( caption == NULL ) {
        return true;
    }
    caption++;

    
    if( strlen( caption ) < DISPLAY_WIDTH - 5 ) {

        if( strcasestr_P( buffer, PSTR("North Dakota/")) != NULL ) {
            strcat_P( buffer, PSTR( ", ND" ));
        }

        if( strcasestr_P( buffer, PSTR("Indiana/")) != NULL ) {
            strcat_P( buffer, PSTR( ", IN" ));
        }

        if( strcasestr_P( buffer, PSTR("Kentucky/")) != NULL ) {
            strcat_P( buffer, PSTR( ", KY" ));
        }
    }

    if( strcasecmp_P( caption, PSTR( "DumontDUrville" )) == 0 ) {
        strcpy_P( caption, PSTR( "D'Urville" ));
    }


    g_lcd.print( caption, DISPLAY_WIDTH - 2, TEXT_ALIGN_LEFT );

    return false;
}