//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/screen.cpp
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
#include "screen.h"
#include "config.h"
#include "resources.h"
#include "ui/ui.h"


Screen* g_currentScreen;
bool g_screenUpdate = false;
bool g_screenClear = false;
const char* g_currentCustomCharacterSet = NULL;

unsigned long g_enterScreenTime = 0;


/*--------------------------------------------------------------------------
 *
 * Class constructor for ScreenItem class.
 *
 * Arguments
 * ---------
 *  None
 */
ScreenItem::ScreenItem() {
    this->unload();  
}


/*--------------------------------------------------------------------------
 *
 * Load a screen item from program memory.
 *
 * Arguments
 * ---------
 *  - item : Pointer to the screen item structure contained in program
 *           memory
 *
 * Returns : Nothing
 */
void ScreenItem::loadFromProgmem( const struct ScreenItemBase* item ) {

    struct ScreenItemBase memItem;
    memcpy_P( &memItem, item, sizeof( ScreenItemBase ) );

    this->_type = memItem._type;
    this->_id = memItem._id;
    this->_row = memItem._row;
    this->_col = memItem._col;
    this->_caption = memItem._caption;
    this->_value = memItem._value;
    this->_min = memItem._min;
    this->_max = memItem._max;
    this->_length = memItem._length;
    this->_options = memItem._options;
    this->_list = memItem._list;
}


/*--------------------------------------------------------------------------
 *
 * Unload the screen item
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void ScreenItem::unload() {

    this->_type = ITEM_TYPE_NULL;
    this->_id = 0;
    this->_row = 0;
    this->_col = 0;
    this->_caption = NULL;
    this->_value = NULL;
    this->_min = 0;
    this->_max = 0;
    this->_length = 0;
    this->_options = 0;
    this->_list = NULL;
}


/*--------------------------------------------------------------------------
 *
 * Class constructor for Screen class.
 *
 * Arguments
 * ---------
 *  - id : Screen ID
 */
Screen::Screen( uint8_t id ) {
    this->_id = id;
}


/*--------------------------------------------------------------------------
 *
 * Class constructor for Screen class with events pre-defined.
 *
 * Arguments
 * ---------
 *  - id : Screen ID
 *  - items : Screen items
 *  - eventValueChange : Pointer to a function called when an item's value changes.
 *  - eventEnterScreen : Pointer to a function called when entering the screen.
 *  - eventExitScreen  : Pointer to a function called when leaving the screen.
 */
Screen::Screen( uint8_t id, const struct ScreenItemBase* items, pfcbValueChange _eventValueChange,
                pfcbEnterScreen _eventEnterScreen, pfcbExitScreen _eventExitScreen ) {

    this->_id = id;
    this->_items = items;
    this->_eventValueChange = _eventValueChange;
    this->_eventEnterScreen = _eventEnterScreen;
    this->_eventExitScreen = _eventExitScreen;
}


/*--------------------------------------------------------------------------
 *
 * Run task for this screen
 *
 * Arguments
 * ---------
 *  - None
 *
 * Returns : Nothing
 */
void Screen::runTask() {

    /* Process Keypad event if available */
    uint8_t key;
    key = g_keypad.processEvents();

    if( key != KEY_NONE ) {
        this->processKeypadEvent( key );
    }

    /* Exit the screen if the timeout timer has elapsed. */
    if( this->hasScreenTimedOut() ) {

        if( this->_eventTimeout != NULL ) {
            this->_eventTimeout( this );
            this->resetTimeout();

        } else {
            this->exitScreen();
        }
    }
}


/*--------------------------------------------------------------------------
 *
 * Update the screen contents on the LCD.
 *
 * Arguments
 * ---------
 *  - force : Update the screen contents event if it's not the current
 *            active screen.
 *
 * Returns : Nothing
 */
void Screen::update( bool force ) {

    ScreenItem item;

    /* Do not draw the screen if it's not the current active screen */
    if( g_currentScreen != this && force == false ) {
        return;
    }

    if( g_screenClear )  {
        g_lcd.clear();
        g_screenClear = false;
    }

    if( this->_customCharacterSet != g_currentCustomCharacterSet ) {
        g_currentCustomCharacterSet = this->_customCharacterSet;

        g_lcd.setCustomCharacters( this->_customCharacterSet );
    }

    /* Disable blinking cursor */
    g_lcd.setCursor( false, false );

    if( this->_isShowConfirmDialog == true ) {
        g_lcd.setPosition( 0, 0 );
        g_lcd.print_P( S_QUESTION_SAVE );

        g_lcd.setPosition( 1, 0 );
        g_lcd.print( ( this->_fieldPos == 0 ) ? CHAR_SELECT : CHAR_SPACE );
        g_lcd.print_P( S_NO );

        g_lcd.setPosition( 1, 8 );
        g_lcd.print( ( this->_fieldPos == 1 ) ? CHAR_SELECT : CHAR_SPACE );
        g_lcd.print_P( S_YES );
        return;
    }


    /* Allow the callback to override the default screen drawing */
    if( this->_eventDrawScreen != NULL ) {

        g_lcd.setPosition( 0, 0 );

        if( this->_eventDrawScreen( this ) == false ) {
            return;
        }
    }

    if( this->_items == NULL ) {
        return;
    }

    /* If the current selected item is full screen, only draw this item */
    if( this->_itemFullscreen == true ) {

        item.loadFromProgmem( &this->_items[ this->_selected ] );
        this->drawItem( &item, true, 0, 0 );


    } else {

        /* Draw all visible items on the screen */
        for( uint8_t i = 0; true; i++ ) {

            item.loadFromProgmem( &this->_items[ i ] );

            bool isSelected = ( this->_selected == i );
            uint8_t row = item.getPositionRow() - this->_scroll;
            uint8_t col = item.getPositionCol();

            /* No more items to draw */
            if( item.getType() == ITEM_TYPE_NULL ) {
                break;
            }

            /* Ignore items above the scroll window, go to the next item. */
            if( item.getPositionRow() < this->_scroll ) {
                continue;
            }

            /* Draw page up symbol if there are items above the scroll window. */
            if( this->_scroll > 0 ) {

                g_lcd.setPosition( 0, DISPLAY_WIDTH - 1 );
                g_lcd.print( CHAR_PAGE_UP );
            }

            /* Don't draw items below the scroll window, no need to continue. */
            if( item.getPositionRow() > this->_scroll + DISPLAY_HEIGHT - 1 ) {

                /* Draw page down symbol */
                g_lcd.setPosition( DISPLAY_HEIGHT - 1, DISPLAY_WIDTH - 1 );
                g_lcd.print( CHAR_PAGE_DOWN );

                break;
            }

            /* Draw the current item */
            this->drawItem( &item, isSelected, row, col );
        }
    }

    this->updateCursorPosition();
}


/*--------------------------------------------------------------------------
 *
 * Process key press events.
 *
 * Arguments
 * ---------
 *  - key : detected key press
 *
 * Returns : Nothing
 */
void Screen::processKeypadEvent( uint8_t key ) {

    /* Allow the callback to override the keypress event */
    if( this->_eventKeypress != NULL ) {

        if( this->_eventKeypress( this, key ) == false ) {
            return;
        }
    }

    switch( key ) {

        /* Exit current menu */
        case KEY_MENU:

            if( this->_itemFullscreen == true ) {

                if( this->_eventSelectionChanged != NULL ) {
                    this->_eventSelectionChanged( this, &this->_item, this->_fieldPos, false );
                }

                this->_itemFullscreen = false;

                this->updateKeypadRepeatMode();

                g_screenClear = true;
                g_screenUpdate = true;
                return;
            }


            this->exitScreen();

            break;

        /* Increment/select current item.*/
        case KEY_SET:

            if( this->_isShowConfirmDialog ) {

                this->_returnValue = this->_fieldPos + 1;

                if( this->_parent != NULL ) {
                    this->_parent->activate( false );
                }

                return;
            }

            /* Or increment the value of the item */
            this->incrementItemValue( &this->_item, false );

            break;

        case KEY_SHIFT | KEY_MENU:
            this->clearItemValue( &this->_item );
            break;

        /* Go to next item or next digit position.*/
        case KEY_NEXT:

            this->_fieldPos++;


            if( this->_isShowConfirmDialog == true ) {

                if( this->_fieldPos > 1 ) {
                    this->_fieldPos = 0;
                }

                g_screenUpdate = true;
                return;
            }


            /* increment cursor position within the current item, if
            it reaches the item length, skip to the next item */
            if( this->_fieldPos >= this->calcFieldLength( &this->_item ) ) {

                this->_fieldPos = 0;

                if( this->_itemFullscreen == false ) {

                    this->selectItem( this->_selected + 1 );
                }

                g_screenUpdate = true;
            }


            if( this->_eventSelectionChanged != NULL ) {
                this->_eventSelectionChanged( this, &this->_item, this->_fieldPos, this->_itemFullscreen );
            }

            g_screenUpdate = true;
            break;

        case KEY_SWIPE | KEY_RIGHT:
            enableNightLamp();
            break;

        case KEY_SWIPE | KEY_LEFT:
            disableNightLamp();
            break;        
    }

}


/*--------------------------------------------------------------------------
 *
 * Reset the timeout timer.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Screen::resetTimeout( int16_t timeout ) {

    if( timeout >= 0 ) {
        this->_timeout = timeout;
    }

    g_enterScreenTime = millis();
}


/*--------------------------------------------------------------------------
 *
 * Returns whether or not the timeout delay has elapsed.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if the screen has timed out or FALSE otherwise.
 */
bool Screen::hasScreenTimedOut() {

    if( this->_timeout == 0 ) {
        return false;
    }

    return ( ( millis() - g_enterScreenTime ) > this->_timeout );
}


/*--------------------------------------------------------------------------
 *
 * Exits the current screen and returns to the parent screen.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Screen::exitScreen() {
    if( this->_parent == NULL ) {
        return;
    }

    /* If items changed, show the confirm changes dialog */
    if( this->_confirmChanges && this->_itemChanged ) {

        this->_isShowConfirmDialog = true;

        this->clearSelection();

        return;
    }

    /* Goto back to parent screen */
    this->_parent->activate( false );
    this->_parent = NULL;

    return;
}


/*--------------------------------------------------------------------------
 *
 * Leave the current screen and activate this one.
 *
 * Arguments
 * ---------
 *  - selectFirstItem : TRUE to select the first item or False to leave the
 *                      previously selected item the last time the screen
 *                      was active
 *  - parent          : Sets which screen will be activated when leaving
 *                      this screen.
 *
 * Returns : Nothing
 */
void Screen::activate( bool selectFirstItem, Screen* parent ) {

    /* Trigger exit screen event. If callback return false, cancel screen change */
    if( g_currentScreen->_eventExitScreen != NULL ) {
        if( g_currentScreen->_eventExitScreen( g_currentScreen, this ) == false ) {
            return;
        }
    }

    g_currentScreen = this;

    this->_returnValue = RETURN_NONE;
    this->_isShowConfirmDialog = false;
    this->_itemChanged = false;
    this->_uppercase = false;

    if( selectFirstItem ) {
        this->selectFirstItem();
    }

    /* Reselect the item */
    selectItem( this->_selected );

    g_enterScreenTime = millis();


    if( parent != NULL && parent != this ) {
        this->_parent =  parent;
    }


    /* Trigger enter screen event on new screen */
    if( this->_eventEnterScreen != NULL ) {
        this->_eventEnterScreen( this );
    }

    g_screenClear = true;
    g_screenUpdate = true;
}


/*--------------------------------------------------------------------------
 *
 * Update a screen item display on the LCD
 *
 * Arguments
 * ---------
 *  - item       : Pointer to a screen item.
 *  - isSelected : TRUE if item is currently selectedor False otherwise.
 *  - row        :  Zero-based Y position where the item is located on the LCD.
 *  - col        :  Zero-based X position where the item is located on the LCD.
 *
 * Returns : Nothing
 */
void Screen::drawItem( ScreenItem* item, bool isSelected, uint8_t row, uint8_t col ) {

    uint8_t value;

    g_lcd.setPosition( row, col );

    if( this->isItemFullScreenEditable( item ) ) {

        if( this->_itemFullscreen == true ) {

            if( item->getCaption() != NULL ) {
                this->printItemCaption( item );
                g_lcd.print_P( S_SEPARATOR );

                g_lcd.setPosition( row + 1, col );
            }

        } else {

            /* Only print the item caption */
            g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );

            this->printItemCaption( item );


            return;
        }

    } else {

        if( ( item->getOptions() & ITEM_COMPACT ) == 0 ) {

            switch( item->getType() ) {

                case ITEM_TYPE_STATIC:
                case ITEM_TYPE_IP:
                    break;

                case ITEM_TYPE_LINK:
                    g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );

                    this->printItemCaption( item );
                    break;

                case ITEM_TYPE_TIME:
                    this->printItemCaption( item );
                    g_lcd.print_P( S_SEPARATOR );

                    break;

                default:
                    if( item->getCaption() != NULL ) {
                        g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );

                        this->printItemCaption( item );
                        g_lcd.print_P( S_SEPARATOR );
                    }

                    break;
            }
        }
    }

    /* Allow the callback to override the default item drawing. */
    if( this->_eventDrawItem != NULL ) {

        if( this->_eventDrawItem( this, item, isSelected, row, col ) == false ) {
            return;
        }
    }

    switch( item->getType() ) {

        case ITEM_TYPE_BAR:

            uint8_t nbars;
            nbars = this->itemValueToBars( item );

            uint8_t npadding;
            npadding = item->getLength() - ( nbars / 2 );

            g_lcd.print( CHAR_FIELD_BEGIN );
            g_lcd.fill( CHAR_BAR_FULL, nbars / 2 );

            if( nbars % 2 ) {
                g_lcd.print( CHAR_BAR_HALF );
                npadding--;
            }

            g_lcd.fill( CHAR_SPACE, npadding );
            g_lcd.print( CHAR_FIELD_END );

            break;


        case ITEM_TYPE_TEXT:

            uint8_t maxWidth;
            maxWidth = DISPLAY_WIDTH - col - 2;

            g_lcd.print( CHAR_FIELD_BEGIN );

            if( item->getLength() > maxWidth ) {
                uint8_t offset;
                offset = this->_fieldPos > ( maxWidth - 1 ) ? this->_fieldPos - maxWidth + 1 : 0;

                g_lcd.print( ( char* )item->getValuePtr() + offset, maxWidth, TEXT_ALIGN_LEFT );

            } else {
                g_lcd.print( ( char* )item->getValuePtr(), maxWidth, TEXT_ALIGN_LEFT );
            }

            g_lcd.print( CHAR_FIELD_END );


            break;

        case ITEM_TYPE_LIST:

            if( item->getOptions() & ITEM_COMPACT || item->getOptions() & ITEM_EDIT_FULLSCREEN ) {

                g_lcd.print( isSelected ? CHAR_SELECT : CHAR_FIELD_BEGIN );
            }



            if( item->getListPtr() != NULL ) {

                uint8_t offset = 0;

                if( item->getValuePtr() != NULL ) {
                    offset = item->getValue() * ( item->getLength() + 1 );
                }

                if( item->getOptions() & ITEM_LIST_SRAM_POINTER ) {
                    g_lcd.print( ( ( const char* )item->getListPtr() ) + offset, item->getLength(), TEXT_ALIGN_LEFT );

                } else {
                    g_lcd.print_P( ( ( const char* )item->getListPtr() ) + offset, item->getLength(), TEXT_ALIGN_LEFT );
                }

            } else {
                g_lcd.fill( CHAR_SPACE, item->getLength() );
            }

            if( item->getOptions() & ITEM_COMPACT || item->getOptions() & ITEM_EDIT_FULLSCREEN ) {
                g_lcd.print( isSelected ? CHAR_SELECT_REV : CHAR_FIELD_END );
            }

            break;

        case ITEM_TYPE_MONTH:
            g_lcd.print_P( getMonthName( item->getValue(), true ) );
            break;

        case ITEM_TYPE_STATIC:
            this->printItemCaption( item );
            break;

        /* On/off */
        case ITEM_TYPE_TOGGLE:

            if( item->getOptions() & ITEM_COMPACT ) {

                if( item->getCaption() != NULL ) {
                    g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );

                    this->printItemCaption( item );
                    g_lcd.print( CHAR_SPACE );

                    g_lcd.print( item->getValueBoolean() ? CHAR_CHECKED : CHAR_SPACE );

                } else {
                    g_lcd.print( item->getValueBoolean() ? CHAR_CHECKED : CHAR_UNCHECKED );
                }



            } else {

                if( item->getCaption() == NULL ) {
                    g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );
                }

                g_lcd.print_P( item->getValueBoolean() == true ? S_ON : S_OFF, 3, TEXT_ALIGN_LEFT );
            }

            break;


        case ITEM_TYPE_NUMBER:
            value = item->getValue();

            /* Left padding */
            g_lcd.fill( CHAR_SPACE, this->getNumDigits( item->getMax() ) - this->getNumDigits( value ) );

            g_lcd.printf( "%d", item->getValue() );

            break;

        case ITEM_TYPE_TIME:
            Time* time;
            time = ( Time* )item->getValuePtr();

            uint8_t hour;
            hour = time->hour;

            if( g_config.clock.display_24h == false ) {

                hour = time->hour % 12;

                if( hour == 0 ) {
                    hour = 12;
                }
            }

            g_lcd.printf( "%2d:%02d ", hour, time->minute );

            if( g_config.clock.display_24h == false ) {
                g_lcd.print_P( ( time->hour > 11 ) ? S_PM : S_AM );
            }

            break;

        case ITEM_TYPE_YEAR:
            uint8_t year;
            year = item->getValue();

            g_lcd.printf( "20%02d", year );

            break;

        case ITEM_TYPE_IP:

            uint8_t address[4];
            memcpy( &address, item->getValuePtr(), 4 );

            g_lcd.printf( "%3d.%3d.%3d.%3d",
                          address[ 0 ],
                          address[ 1 ],
                          address[ 2 ],
                          address[ 3 ]
                        );
            break;

        case ITEM_TYPE_DOW:

            uint8_t i;

            for( i = 0; i < 7; i++ ) {

                if( bitRead( item->getValue(), i ) != 0 ) {

                    char c;
                    c = pgm_read_byte( S_DOW + i );

                    g_lcd.print( c );

                } else {
                    g_lcd.print( "-" );
                }

                if( ( item->getOptions() & ITEM_COMPACT ) == 0 ) {
                    g_lcd.print( CHAR_SPACE );
                }
            }

            break;
    }

}


/*--------------------------------------------------------------------------
 *
 * Returns whether or not a given item can be viewed full screen.
 *
 * Arguments
 * ---------
 *  - item : Pointer to a screen item.
 *
 * Returns : TRUE if the item can be viewed full screenor False otherwise.
 */
bool Screen::isItemFullScreenEditable( ScreenItem* item ) {

    switch( item->getType() ) {

        /* These items types cannot be editable full screen */
        case ITEM_TYPE_LINK:
        case ITEM_TYPE_STATIC:
        case ITEM_TYPE_TOGGLE:
            return false;
    }

    return item->getOptions() & ITEM_EDIT_FULLSCREEN;
}


/*--------------------------------------------------------------------------
 *
 * Prints a given item's caption on the LCD.
 *
 * Arguments
 * ---------
 *  - item : Pointer to a screen item.
 *
 * Returns : The number of character printed.
 */
uint8_t Screen::printItemCaption( ScreenItem* item ) {

    uint8_t nchr = 0;

    if( item->getCaption() == NULL ) {
        return 0;
    }

    if( item->getOptions() & ITEM_CAPTION_SRAM_POINTER ) {

        /* String stored in SRAM */
        nchr = g_lcd.print( ( const char* )item->getCaption() );

    } else {

        /* String stored in program memory */
        nchr = g_lcd.print_P( ( const char* )item->getCaption() );
    }

    return nchr;
}


/*--------------------------------------------------------------------------
 *
 * Selects the first item on the screen.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Screen::selectFirstItem() {
    this->_scroll = 0;
    this->_itemFullscreen = false;
    this->_isShowConfirmDialog = false;

    this->selectItem( 0 );
}


/*--------------------------------------------------------------------------
 *
 * Resets the screen selection.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Screen::clearSelection() {

    this->_item.unload();

    this->_selected = 0;
    this->_scroll = 0;
    this->_fieldPos = 0;
    this->_uppercase = false;
    this->_itemFullscreen = false;

    this->updateKeypadRepeatMode();

    g_screenUpdate = true;
    g_screenClear = true;

    if( this->_eventSelectionChanged != NULL ) {
        this->_eventSelectionChanged( this, &this->_item, 0, false );
    }
}


/*--------------------------------------------------------------------------
 *
 * Selects a given item from it's index position.
 *
 * Arguments
 * ---------
 *  - index : zero-based index of the item to select.
 *
 * Returns : Nothing
 */
void Screen::selectItem( uint8_t index ) {

    uint8_t n_listIter = 0;
    this->_fieldPos = 0;
    this->_uppercase = false;
    this->_selected = index;
    this->_itemFullscreen = false;

    if( this->_items == NULL ) {

        this->clearSelection();
        return;
    }

    while( true ) {

        this->_item.loadFromProgmem( &this->_items[ this->_selected ] );

        /* Give up looking for a selectable item if scanned list more than 1 time */
        if( n_listIter > 1 ) {
            this->clearSelection();

            return;
        }

        switch( this->_item.getType() ) {

            /* End of list */
            case ITEM_TYPE_NULL:
                this->_selected = 0;

                if( this->_scroll > 0 ) {
                    this->_scroll = 0;
                    g_screenClear = true;
                }

                n_listIter++;
                break;

            /* Non-selectable item */
            case ITEM_TYPE_STATIC:
                this->_selected++;
                break;

            /* Selectable item */
            default:

                if( this->_eventSelectionChanged != NULL ) {
                    this->_eventSelectionChanged( this, &this->_item, this->_fieldPos, false );
                }

                /* Skip over read-only items */
                if( this->_item.getOptions() & ITEM_READONLY ) {

                    this->_selected++;
                    break;
                }

                if( this->_item.getPositionRow() > this->_scroll + DISPLAY_HEIGHT - 1 ) {

                    if( this->_item.getOptions() & ITEM_BREAK ) {
                        /* Scroll item to the top of the screen */
                        this->_scroll = this->_item.getPositionRow();

                    } else {
                        /* Scroll item to the bottom of the screen */
                        this->_scroll = this->_item.getPositionRow() - ( DISPLAY_HEIGHT - 1 );
                    }

                    g_screenClear = true;
                }

                this->updateKeypadRepeatMode();

                return;
        }
    }
}


/*--------------------------------------------------------------------------
 *
 * Sets which keys on the keypad are repeatable when held down based on the
 * current screen state and selected item.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Screen::updateKeypadRepeatMode() {

    if( this->_isShowConfirmDialog == true ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    if( this->_items == NULL ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    if( ( this->isItemFullScreenEditable( &this->_item ) ) && ( this->_itemFullscreen == false ) ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    switch( this->_item.getType() ) {
        case ITEM_TYPE_STATIC:
        case ITEM_TYPE_TOGGLE:
        case ITEM_TYPE_LINK:
            g_keypad.repeatMask = KEY_NONE;
            break;

        default:
            g_keypad.repeatMask = KEY_SET;
            break;
    }

}


/*--------------------------------------------------------------------------
 *
 * Returns a given item's maximum value length.
 *
 * Arguments
 * ---------
 *  - item : Pointer to a screen item.
 *
 * Returns : The field length.
 */
uint8_t Screen::calcFieldLength( ScreenItem* item ) {
    uint8_t length = 0;

    if( ( this->isItemFullScreenEditable( item ) ) && ( this->_itemFullscreen == false ) ) {
        return 0;
    }


    /* Determine item length */
    switch( item->getType() ) {
        case ITEM_TYPE_IP:
            length = 12;
            break;

        case ITEM_TYPE_TIME:
            length = 2;
            break;

        case ITEM_TYPE_NUMBER:

            if( item->getOptions() & ITEM_NUMBER_INC_WHOLE ) {
                length = 1;

            } else {
                length = this->getNumDigits( item->getMax() );
            }

            break;

        case ITEM_TYPE_YEAR:
            length = 2;
            break;

        case ITEM_TYPE_DOW:
            length = 7;
            break;

        case ITEM_TYPE_TEXT:
            length = item->getLength();
            break;

    }

    return length;
}


/*--------------------------------------------------------------------------
 *
 * Update the cursor position on the LCD.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void Screen::updateCursorPosition() {


    uint8_t row = this->_item.getPositionRow() - this->_scroll;
    uint8_t col = this->_item.getPositionCol();

    if( this->_item.getOptions() & ITEM_NOCURSOR ) {
        return;
    }

    if( this->isItemFullScreenEditable( &this->_item ) ) {

        if( this->_itemFullscreen == false ) {
            /* No blinking cursor if not currently full screen */
            return;

        } else {
            row = 1;
            col = 0;
        }

    } else {

        if( ( this->_item.getOptions() & ITEM_COMPACT ) == 0 && this->_item.getCaption() != NULL ) {

            if( this->_item.getOptions() & ITEM_CAPTION_SRAM_POINTER ) {

                col += strlen( ( const char* )this->_item.getCaption() ) + 3;

            } else {
                col += strlen_P( ( const char* )this->_item.getCaption() ) + 3;
            }
        }
    }

    switch( this->_item.getType() ) {
        case ITEM_TYPE_TIME:

            if( this->_fieldPos == 0 ) {
                g_lcd.setPosition( row, col + 1 );

            } else {
                g_lcd.setPosition( row, col + 4 );
            }

            break;

        case ITEM_TYPE_IP:
            g_lcd.setPosition( row, this->_fieldPos + ( this->_fieldPos / 3 ) );
            break;

        case ITEM_TYPE_NUMBER:

            if( this->_item.getOptions() & ITEM_NUMBER_INC_WHOLE ) {
                g_lcd.setPosition( row, col + this->getNumDigits( this->_item.getMax() ) - 1 );

            } else {
                g_lcd.setPosition( row, col + this->_fieldPos );
            }

            break;

        case ITEM_TYPE_TEXT:
            uint8_t maxWidth;
            maxWidth = DISPLAY_WIDTH - col - 2;

            g_lcd.setPosition( row, col + ( this->_fieldPos > ( maxWidth - 1 ) ? maxWidth - 1 : this->_fieldPos ) + 1 );
            break;

        case ITEM_TYPE_YEAR:
            g_lcd.setPosition( row, col + this->_fieldPos + 2 );
            break;

        case ITEM_TYPE_MONTH:
            g_lcd.setPosition( row, col );
            break;

        case ITEM_TYPE_TOGGLE:

            if( this->_item.getCaption() != NULL || ( this->_item.getOptions() & ITEM_COMPACT ) == 0 ) {
                return;
            }

            g_lcd.setPosition( row, col );
            break;

        case ITEM_TYPE_DOW:

            if( this->_item.getOptions() & ITEM_COMPACT ) {
                g_lcd.setPosition( row, col + this->_fieldPos );

            } else {
                g_lcd.setPosition( row, col + ( this->_fieldPos * 2 ) );
            }

            break;


        /* No blinking cursor for other items */
        default:
            return;

    }

    /* Enable underline cursor */
    g_lcd.setCursor( false, true );
}


/*--------------------------------------------------------------------------
 *
 * Revert an item to it's default value.
 *
 * Arguments
 * ---------
 *  - item : Pointer to a screen item.
 *
 * Returns : Nothing
 */
void Screen::clearItemValue( ScreenItem* item ) {

    if( ( this->isItemFullScreenEditable( item ) ) && ( this->_itemFullscreen == false ) ) {
        return;
    }

    switch( item->getType() ) {

        case ITEM_TYPE_TEXT:

            for( uint8_t i = this->_fieldPos; i < item->getLength(); i++ ) {
                ( ( char* )item->getValuePtr() )[ i ] = 0x00;
            }

            this->_uppercase = false;
            break;

        case ITEM_TYPE_NUMBER:
        case ITEM_TYPE_BAR:
        case ITEM_TYPE_LIST:
        case ITEM_TYPE_YEAR:
            item->setValue( item->getMin() );
            break;

        case ITEM_TYPE_MONTH:
            item->setValue( 1 );
            break;

        case ITEM_TYPE_DOW:
            if( item->getValue() != 0 ) {
                item->setValue( 0 );

            } else {
                item->setValue( 0x7F );
            }

            break;

        default:
            return;

    }

    this->_itemChanged = true;

    g_screenUpdate = true;

    if( this->_eventValueChange != NULL ) {
        this->_eventValueChange( this, item );
    }
}


/*--------------------------------------------------------------------------
 *
 * Converts the item's value to the number of bars to be displayed for
 * ITEM_TYPE_BAR items.
 *
 * Arguments
 * ---------
 *  - item : Pointer to a screen item.
 *
 * Returns : The number of bars to display
 */
uint8_t Screen::itemValueToBars( ScreenItem* item ) {

    uint8_t value;
    value = item->getValue();

    if( value < item->getMin() ) {
        value = item->getMin();
    }

    if( value > item->getMax() ) {
        value = item->getMax();
    }

    double nbars;
    nbars = ( value - item->getMin() ) * ( item->getLength() * 2 );
    nbars /= ( item->getMax() - item->getMin() );

    return ( uint8_t )lround( nbars );
}


/*--------------------------------------------------------------------------
 *
 * Increment a given item's value
 *
 * Arguments
 * ---------
 *  - item  : Pointer to a screen item.
 *  - shift : TRUE to use an alternate increment valueor False to increment by one.
 *
 * Returns : Nothing
 */
void Screen::incrementItemValue( ScreenItem* item, bool shift ) {

    uint8_t pos;

    if( item->getOptions() & ITEM_READONLY ) {
        return;
    }

    if( ( this->isItemFullScreenEditable( item ) ) && ( this->_itemFullscreen == false ) ) {

        if( this->_eventSelectionChanged != NULL ) {
            this->_eventSelectionChanged( this, item, this->_fieldPos, true );
        }

        this->_itemFullscreen = true;

        this->_fieldPos = 0;

        this->updateKeypadRepeatMode();

        g_screenUpdate = true;
        g_screenClear = true;
        return;
    }

    switch( item->getType() ) {

        case ITEM_TYPE_BAR:

            uint8_t nbars;
            nbars = this->itemValueToBars( item );

            if( ( item->getMax() - item->getMin() ) / ( item->getLength() * 2 ) > 0 ) {

                nbars++;

                if( nbars > ( item->getLength() * 2 ) ) {
                    nbars = 0;
                }

                item->setValue( item->getMin() + nbars * ( item->getMax() - item->getMin() ) / ( item->getLength() * 2 ) );

            } else {

                item->setValue( item->getValue() + 1 );

                if( item->getValue() > item->getMax() ) {
                    item->setValue( item->getMin() );
                }
            }

            break;

        case ITEM_TYPE_TEXT: {

                char* text = ( char* )item->getValuePtr();
                pos = strlen( text );

                if( this->_fieldPos >= pos ) {

                    while( pos < this->_fieldPos ) {
                        text[ pos ] = CHAR_SPACE;

                        pos++;
                    }

                    text[ pos + 1 ] = 0x00;

                    text[ this->_fieldPos ] = 0x60;
                }

                if( isalpha( text[ this->_fieldPos ] ) ) {
                    this->_uppercase = isupper( text[ this->_fieldPos ] );
                }

                if( shift ) {

                    if( isalpha( text[ this->_fieldPos ] ) == false ) {
                        return;
                    }

                    if( isupper( text[ this->_fieldPos ] ) ) {
                        text[ this->_fieldPos ] = tolower( text[ this->_fieldPos ] );

                    } else {
                        text[ this->_fieldPos ] = toupper( text[ this->_fieldPos ] );
                    }

                } else {

                    text[ this->_fieldPos ] = this->nextValidCharacter( text[ this->_fieldPos ] );
                }

                break;
            }

        case ITEM_TYPE_TOGGLE:

            item->setValueBoolean( ! item->getValueBoolean() );
            break;

        case ITEM_TYPE_NUMBER:

            if( item->getOptions() & ITEM_NUMBER_INC_WHOLE ) {

                item->setValue( item->getValue() + 1 );

                if( item->getValue() > item->getMax() ) {
                    item->setValue( item->getMin() );
                }

                break;
            }

        /* Fall-through */

        case ITEM_TYPE_YEAR:

            pos = this->calcFieldLength( item ) - this->_fieldPos - 1;

            this->incDigit( ( uint8_t* )item->getValuePtr(), pos, item->getMax(), item->getMin() );
            break;

        case ITEM_TYPE_TIME:
            Time* time;
            time = ( Time* )item->getValuePtr();

            /* Increment hours */
            if( this->_fieldPos == 0 ) {
                time->hour++;

                if( time->hour > 23 ) {
                    time->hour = 0;
                }

                /* Increment minutes */

            } else {

                time->minute++;

                if( time->minute > 59 ) {
                    time->minute = 0;
                }
            }

            break;

        case ITEM_TYPE_DOW:

            if( bitRead( item->getValue(), this->_fieldPos ) != 0 ) {

                item->setValue( item->getValue() & ~( 1UL << this->_fieldPos ) );

            } else {
                item->setValue( item->getValue() | ( 1UL << this->_fieldPos ) );
            }

            break;

        case ITEM_TYPE_IP:

            uint8_t* ip_seg;
            ip_seg = ( ( uint8_t* )item->getValuePtr() ) + ( this->_fieldPos / 3 );
            pos = ( 2 - ( this->_fieldPos % 3 ) );

            this->incDigit( ip_seg, pos, 255, 1 );

            break;

        case ITEM_TYPE_MONTH:
        case ITEM_TYPE_LIST:

            if( item->getValuePtr() == NULL ) {
                break;
            }

            item->setValue( item->getValue() + 1 );

            if( item->getValue() > item->getMax() ) {
                item->setValue( item->getMin() );
            }

            break;

        case ITEM_TYPE_LINK:
            if( this->_eventValueChange != NULL ) {
                this->_eventValueChange( this, item );
            }

            if( this->_item.getValuePtr() != NULL ) {

                Screen* link = ( Screen* )this->_item.getValuePtr();
                link->activate( true, this );
            }

            return;
    }

    this->_itemChanged = true;
    g_screenUpdate = true;

    if( this->_eventValueChange != NULL ) {
        this->_eventValueChange( this, item );
    }
}


/*--------------------------------------------------------------------------
 *
 * Get the next valid character when incrementing a text box item
 *
 * Arguments
 * ---------
 *  - current : Current character to start from
 *
 * Returns : The next valid character.
 */
inline char Screen::nextValidCharacter( char current ) {

    /* Sequence : space => [a-z] => [0-9] => symbols => space */

    /* 0x20-0x2F [symbols] */
    /* 0x30-0x3F [0-9] */
    /* 0x41-0x5A [A-Z] */
    /* 0x61-0x7A [a-z] */

    if( current == 0x20 ) {
        return ( this->_uppercase ) ? 0x41 : 0x61;
    }

    if( current == 0x23 ) {
        return 0x25;
    }

    if( current == 0x2F ) {
        return 0x20;
    }

    current++;

    if( current > 0x7A ) {
        return 0x30;
    }

    if( current > 0x5A && this->_uppercase == true ) {
        return 0x30;
    }

    if( current > 0x3F && current < 0x41 ) {
        return 0x21;
    }


    return current;
}


/*--------------------------------------------------------------------------
 *
 * Increments an item's value while keeping it in the valid range.
 *
 * Arguments
 * ---------
 *  - value : Pointer to the item's value
 *  - pos   : Zero-based digit position
 *  - max   : Maximum value
 *  - min   : Minimum value
 *
 * Returns : Nothing
 */
void Screen::incDigit( uint8_t* value, uint8_t pos, uint8_t max, uint8_t min ) {

    static uint8_t powLookup[] = { 1, 10, 100 };
    uint8_t pow = powLookup[ pos ];
    uint8_t digit = ( *value / pow ) % 10;

    *value -= ( pow * digit );
    digit++;

    if( digit > 9 ) {
        digit = 0;
    }

    if( ( ( uint16_t )( *value ) ) + ( digit * pow ) > max ) {

        if( ( ( max / pow ) % 10 ) == digit ) {
            *value = max;
            return;

        } else {
            digit = 0;
        }
    }

    *value += ( digit * pow );

    if( *value < min ) {
        *value = min;
    }
}


/*--------------------------------------------------------------------------
 *
 * Returns the number of digits in a given number.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : The number of digits.
 */
inline uint8_t Screen::getNumDigits( uint8_t number ) {
    if( number < 10 ) {
        return 1;
    }

    if( number < 100 ) {
        return 2;
    }

    return 3;
}