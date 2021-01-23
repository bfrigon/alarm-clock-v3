//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/screen.cpp
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
#include "screen_item.h"
#include "../config.h"
#include "../resources.h"
#include "ui.h"


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor for Screen class.
 * 
 */
Screen::Screen() {

    _breadCrumbIndex = 0;
    _breadCrumbSelection[ 0 ] = 0;
    _isShowConfirmDialog = false;
    _itemFullscreen = false;
    _itemChanged = false;
    _confirmChanges = false;
    _selected = 0;
    _returnValue = 0;
    _fieldPos = 0;
    _scroll = 0;
    _timeout = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Run task for this screen
 *
 */
void Screen::processEvents() {

    /* Process Keypad event if available */
    uint8_t key;
    key = g_keypad.processEvents();

    if( key != KEY_NONE ) {
        this->processKeypadEvent( key );
    }

    /* Exit the screen if the timeout timer has elapsed. */
    if( this->hasScreenTimedOut() ) {

        if( _currentScreen.eventTimeout != NULL ) {
            _currentScreen.eventTimeout( this );
            this->resetTimeout();

        } else {
            this->exitScreen();
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Update the screen contents on the LCD.
 * 
 */
void Screen::update() {

    if( _updateRequested == false ) {
        return;
    }

    if( _clearScreenRequested == true )  {
        g_lcd.clear();
    }

    _updateRequested = false;
    _clearScreenRequested = false;

    /* Disable blinking cursor */
    g_lcd.setCursor( false, false );

    if( _isShowConfirmDialog == true ) {
        g_lcd.setPosition( 0, 0 );
        g_lcd.print_P( S_QUESTION_SAVE );

        g_lcd.setPosition( 1, 0 );
        g_lcd.print( ( _fieldPos == 0 ) ? CHAR_SELECT : CHAR_SPACE );
        g_lcd.print_P( S_NO );

        g_lcd.setPosition( 1, 8 );
        g_lcd.print( ( _fieldPos == 1 ) ? CHAR_SELECT : CHAR_SPACE );
        g_lcd.print_P( S_YES );
        return;
    }


    /* Allow the callback to override the default screen drawing */
    if( _currentScreen.eventDrawScreen != NULL ) {

        g_lcd.setPosition( 0, 0 );

        if( _currentScreen.eventDrawScreen( this ) == false ) {
            return;
        }
    }

    if( _currentScreen.items == NULL ) {
        return;
    }

    ScreenItem item;

    /* If the current selected item is full screen, only draw this item */
    if( _itemFullscreen == true ) {

        item.loadFromProgmem( &_currentScreen.items[ _selected ] );
        this->drawItem( &item, true, 0, 0 );


    } else {

        /* Draw all visible items on the screen */
        for( uint8_t i = 0; true; i++ ) {

            item.loadFromProgmem( &_currentScreen.items[ i ] );

            bool isSelected = ( _selected == i );
            uint8_t row = item.getPositionRow() - _scroll;
            uint8_t col = item.getPositionCol();

            /* No more items to draw */
            if( item.getType() == ITEM_TYPE_NULL ) {
                break;
            }

            /* Ignore items above the scroll window, go to the next item. */
            if( item.getPositionRow() < _scroll ) {
                continue;
            }

            /* Draw page up symbol if there are items above the scroll window. */
            if( _scroll > 0 ) {

                g_lcd.setPosition( 0, DISPLAY_WIDTH - 1 );
                g_lcd.print( CHAR_PAGE_UP );
            }

            /* Don't draw items below the scroll window, no need to continue. */
            if( item.getPositionRow() > _scroll + DISPLAY_HEIGHT - 1 ) {

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


/*! ------------------------------------------------------------------------
 *
 * @brief   Process key press events.
 *
 * @param   key    detected key press
 * 
 */
void Screen::processKeypadEvent( uint8_t key ) {

    /* Allow the callback to override the keypress event */
    if( _currentScreen.eventKeypress != NULL ) {

        if( _currentScreen.eventKeypress( this, key ) == false ) {
            return;
        }
    }

    switch( key ) {

        /* Exit current menu */
        case KEY_MENU:

            if( _itemFullscreen == true ) {

                if( _currentScreen.eventSelectionChanged != NULL ) {
                    _currentScreen.eventSelectionChanged( this, &_currentItem, _fieldPos, false );
                }

                _itemFullscreen = false;

                this->updateKeypadRepeatMode();

                _clearScreenRequested = true;
                _updateRequested = true;

                return;
            }


            this->exitScreen();

            break;

        /* Increment/select current item.*/
        case KEY_SET:

            if( _isShowConfirmDialog ) {

                _returnValue = _fieldPos + 1;

                if( _breadCrumbIndex > 0 ) {
                    this->activate( NULL, false );
                }

                return;
            }

            /* Or increment the value of the item */
            this->incrementItemValue( &_currentItem, false );

            break;

        case KEY_SHIFT | KEY_MENU:
            this->clearItemValue( &_currentItem );
            break;

        /* Go to next item or next digit position.*/
        case KEY_NEXT:

            _fieldPos++;


            if( _isShowConfirmDialog == true ) {

                if( _fieldPos > 1 ) {
                    _fieldPos = 0;
                }

                _updateRequested = true;
                return;
            }


            /* increment cursor position within the current item, if
            it reaches the item length, skip to the next item */
            if( _fieldPos >= this->calcFieldLength( &_currentItem ) ) {

                _fieldPos = 0;

                if( _itemFullscreen == false ) {

                    this->selectItem( _selected + 1 );
                }

                _updateRequested = true;
            }


            if( _currentScreen.eventSelectionChanged != NULL ) {
                _currentScreen.eventSelectionChanged( this, &_currentItem, _fieldPos, _itemFullscreen );
            }

            _updateRequested = true;
            break;
    }

}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reset the timeout timer.
 * 
 */
void Screen::resetTimeout( int16_t timeout ) {

    if( timeout >= 0 ) {
        _timeout = timeout;
    }

    _enterScreenTime = millis();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns whether or not the timeout delay has elapsed.
 *
 * @return  TRUE if the screen has timed out, FALSE otherwise.
 * 
 */
bool Screen::hasScreenTimedOut() {

    if( _timeout == 0 ) {
        return false;
    }

    return ( ( millis() - _enterScreenTime ) > _timeout );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Exits the current screen and returns to the parent screen.
 * 
 */
void Screen::exitScreen() {
    if( _breadCrumbIndex == 0 ) {
        return;
    }

    /* If items changed, show the confirm changes dialog */
    if( _confirmChanges && _itemChanged ) {

        _isShowConfirmDialog = true;

        this->clearSelection();

        return;
    }

    /* Goto back to parent screen */
    this->activate( NULL, false );

    return;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Leave the current screen and activate this one.
 *
 * @param   selectFirstItem    TRUE to select the first item or False to leave 
 *                             the previously selected item the last time the 
 *                             screen was active
 * @param   parent             Sets which screen will be activated when 
 *                             leaving this screen.
 * 
 */
void Screen::activate( const ScreenData* screen, bool selectFirstItem ) {

    /* Trigger exit screen event. If callback return false, cancel screen change */
    if( _currentScreen.eventExitScreen != NULL ) {
        if( _currentScreen.eventExitScreen( this ) == false ) {
            return;
        }
    }

    /* Return to previous screen */
    if( screen == NULL ) {

        /* Already at the root screen */
        if( _breadCrumbIndex == 0 ) {
            return;
        }

        _breadCrumbIndex--;

        screen = _breadCrumbScreen[ _breadCrumbIndex ];

    } else {

        /* Maximum screen depth reached */
        if( _breadCrumbIndex == SCREEN_MAX_BREADCRUMB_ITEMS - 1 ) {
            return;
        }

        _breadCrumbSelection[ _breadCrumbIndex ] = _selected;

        if( _breadCrumbScreen[ 0 ] != NULL ) {
            _breadCrumbIndex++;
        }
        
        _breadCrumbScreen[ _breadCrumbIndex ] = screen;
    }

    /* Load the screen data from the program memory */
    memcpy_P( &_currentScreen, screen, sizeof( ScreenData ) );
    
    _timeout = 0;
    _scroll = 0;
    _confirmChanges = false;
    _returnValue = RETURN_NONE;
    _isShowConfirmDialog = false;
    _itemChanged = false;
    _uppercase = false;

    /* Restore previous selection */
    if( selectFirstItem ) {
        this->selectFirstItem();
    } else {
        this->selectItem( _breadCrumbSelection[ _breadCrumbIndex ] );
    }

    _enterScreenTime = millis();

    /* Trigger enter screen event on new screen */
    if( _currentScreen.eventEnterScreen != NULL ) {
        _currentScreen.eventEnterScreen( this );
    }

    /* Clear screen */
    g_lcd.clear();

    g_lcd.setCustomCharacters( _currentScreen.customCharacterSet );

    /* Redraw the screen */
    _updateRequested = true;
    _clearScreenRequested = false;
    this->update();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Request screen refresh
 *
 * @param   clear    Clear the screen before updating
 *
 */
void Screen::requestScreenUpdate( bool clear ) {

    _updateRequested = true;
    _clearScreenRequested = clear;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Update a screen item display on the LCD
 *
 * @param   item          Pointer to a screen item.
 * @param   isSelected    TRUE if item is currently selectedor False otherwise.
 * @param   row           Zero-based Y position where the item is located 
 *                        on the LCD.
 * @param   col           Zero-based X position where the item is located 
 *                        on the LCD.
 * 
 */
void Screen::drawItem( ScreenItem* item, bool isSelected, uint8_t row, uint8_t col ) {

    uint8_t value;

    g_lcd.setPosition( row, col );

    if( this->isItemFullScreenEditable( item ) ) {

        if( _itemFullscreen == true ) {

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
    if( _currentScreen.eventDrawItem != NULL ) {

        if( _currentScreen.eventDrawItem( this, item, isSelected, row, col ) == false ) {
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
                offset = _fieldPos > ( maxWidth - 1 ) ? _fieldPos - maxWidth + 1 : 0;

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


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns whether or not a given item can be viewed full screen.
 *
 * @param   item    Pointer to a screen item.
 *
 * @return  TRUE if the item can be viewed full screen, FALSE otherwise.
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Prints a given item's caption on the LCD.
 *
 * @param   item    Pointer to a screen item.
 *
 * @return  The number of character printed.
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Selects the first item on the screen.
 * 
 */
void Screen::selectFirstItem() {
    _scroll = 0;
    _itemFullscreen = false;
    _isShowConfirmDialog = false;

    this->selectItem( 0 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Resets the screen selection.
 * 
 */
void Screen::clearSelection() {

    _currentItem.unload();

    _selected = 0;
    _scroll = 0;
    _fieldPos = 0;
    _uppercase = false;
    _itemFullscreen = false;

    this->updateKeypadRepeatMode();

    _clearScreenRequested = true;
    _updateRequested = true;

    if( _currentScreen.eventSelectionChanged != NULL ) {
        _currentScreen.eventSelectionChanged( this, &_currentItem, 0, false );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Selects a given item from it's index position.
 *
 * @param   index    zero-based index of the item to select.
 * 
 */
void Screen::selectItem( uint8_t index ) {

    uint8_t n_listIter = 0;
    _fieldPos = 0;
    _uppercase = false;
    _selected = index;
    _itemFullscreen = false;

    if( _currentScreen.items == NULL ) {

        this->clearSelection();
        return;
    }

    while( true ) {

        _currentItem.loadFromProgmem( &_currentScreen.items[ _selected ] );

        /* Give up looking for a selectable item if scanned list more than 1 time */
        if( n_listIter > 1 ) {
            this->clearSelection();

            return;
        }

        switch( _currentItem.getType() ) {

            /* End of list */
            case ITEM_TYPE_NULL:
                _selected = 0;

                if( _scroll > 0 ) {
                    _scroll = 0;
                    _clearScreenRequested = true;
                }

                n_listIter++;
                break;

            /* Non-selectable item */
            case ITEM_TYPE_STATIC:
                _selected++;
                break;

            /* Selectable item */
            default:

                if( _currentScreen.eventSelectionChanged != NULL ) {
                    _currentScreen.eventSelectionChanged( this, &_currentItem, _fieldPos, false );
                }

                /* Skip over read-only items */
                if( _currentItem.getOptions() & ITEM_READONLY ) {

                    _selected++;
                    break;
                }

                if( _currentItem.getPositionRow() > _scroll + DISPLAY_HEIGHT - 1 ) {

                    if( _currentItem.getOptions() & ITEM_BREAK ) {
                        /* Scroll item to the top of the screen */
                        _scroll = _currentItem.getPositionRow();

                    } else {
                        /* Scroll item to the bottom of the screen */
                        _scroll = _currentItem.getPositionRow() - ( DISPLAY_HEIGHT - 1 );
                    }

                    _clearScreenRequested = true;
                }

                this->updateKeypadRepeatMode();

                return;
        }
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets which keys on the keypad are repeatable when held down 
 *          based on the current screen state and selected item.
 * 
 */
void Screen::updateKeypadRepeatMode() {

    if( _isShowConfirmDialog == true ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    if( _currentScreen.items == NULL ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    if( ( this->isItemFullScreenEditable( &_currentItem ) ) && ( _itemFullscreen == false ) ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    switch( _currentItem.getType() ) {
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns a given item's maximum value length.
 *
 * @param   item    Pointer to a screen item.
 *
 * @return  The field length.
 * 
 */
uint8_t Screen::calcFieldLength( ScreenItem* item ) {
    uint8_t length = 0;

    if( ( this->isItemFullScreenEditable( item ) ) && ( _itemFullscreen == false ) ) {
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Update the cursor position on the LCD.
 * 
 */
void Screen::updateCursorPosition() {


    uint8_t row = _currentItem.getPositionRow() - _scroll;
    uint8_t col = _currentItem.getPositionCol();

    if( _currentItem.getOptions() & ITEM_NOCURSOR ) {
        return;
    }

    if( this->isItemFullScreenEditable( &_currentItem ) ) {

        if( _itemFullscreen == false ) {
            /* No blinking cursor if not currently full screen */
            return;

        } else {
            row = 1;
            col = 0;
        }

    } else {

        if( ( _currentItem.getOptions() & ITEM_COMPACT ) == 0 && _currentItem.getCaption() != NULL ) {

            if( _currentItem.getOptions() & ITEM_CAPTION_SRAM_POINTER ) {

                col += strlen( ( const char* )_currentItem.getCaption() ) + 3;

            } else {
                col += strlen_P( ( const char* )_currentItem.getCaption() ) + 3;
            }
        }
    }

    switch( _currentItem.getType() ) {
        case ITEM_TYPE_TIME:

            if( _fieldPos == 0 ) {
                g_lcd.setPosition( row, col + 1 );

            } else {
                g_lcd.setPosition( row, col + 4 );
            }

            break;

        case ITEM_TYPE_IP:
            g_lcd.setPosition( row, _fieldPos + ( _fieldPos / 3 ) );
            break;

        case ITEM_TYPE_NUMBER:

            if( _currentItem.getOptions() & ITEM_NUMBER_INC_WHOLE ) {
                g_lcd.setPosition( row, col + this->getNumDigits( _currentItem.getMax() ) - 1 );

            } else {
                g_lcd.setPosition( row, col + _fieldPos );
            }

            break;

        case ITEM_TYPE_TEXT:
            uint8_t maxWidth;
            maxWidth = DISPLAY_WIDTH - col - 2;

            g_lcd.setPosition( row, col + ( _fieldPos > ( maxWidth - 1 ) ? maxWidth - 1 : _fieldPos ) + 1 );
            break;

        case ITEM_TYPE_YEAR:
            g_lcd.setPosition( row, col + _fieldPos + 2 );
            break;

        case ITEM_TYPE_MONTH:
            g_lcd.setPosition( row, col );
            break;

        case ITEM_TYPE_TOGGLE:

            if( _currentItem.getCaption() != NULL || ( _currentItem.getOptions() & ITEM_COMPACT ) == 0 ) {
                return;
            }

            g_lcd.setPosition( row, col );
            break;

        case ITEM_TYPE_DOW:

            if( _currentItem.getOptions() & ITEM_COMPACT ) {
                g_lcd.setPosition( row, col + _fieldPos );

            } else {
                g_lcd.setPosition( row, col + ( _fieldPos * 2 ) );
            }

            break;


        /* No blinking cursor for other items */
        default:
            return;

    }

    /* Enable underline cursor */
    g_lcd.setCursor( false, true );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Revert an item to it's default value.
 *
 * @param   item    Pointer to a screen item.
 * 
 */
void Screen::clearItemValue( ScreenItem* item ) {

    if( ( this->isItemFullScreenEditable( item ) ) && ( _itemFullscreen == false ) ) {
        return;
    }

    switch( item->getType() ) {

        case ITEM_TYPE_TEXT:

            for( uint8_t i = _fieldPos; i < item->getLength(); i++ ) {
                ( ( char* )item->getValuePtr() )[ i ] = 0x00;
            }

            _uppercase = false;
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

    _itemChanged = true;

    _updateRequested = true;

    if( _currentScreen.eventValueChange != NULL ) {
        _currentScreen.eventValueChange( this, item );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Converts the item's value to the number of bars to be displayed 
 *          for ITEM_TYPE_BAR items.
 *
 * @param   item    Pointer to a screen item.
 *
 * @return  The number of bars to display
 * 
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Increment a given item's value
 *
 * @param   item     Pointer to a screen item.
 * @param   shift    TRUE to use an alternate increment valueor False to 
 *                   increment by one.
 * 
 */
void Screen::incrementItemValue( ScreenItem* item, bool shift ) {

    uint8_t pos;

    if( item->getOptions() & ITEM_READONLY ) {
        return;
    }

    if( ( this->isItemFullScreenEditable( item ) ) && ( _itemFullscreen == false ) ) {

        if( _currentScreen.eventSelectionChanged != NULL ) {
            _currentScreen.eventSelectionChanged( this, item, _fieldPos, true );
        }

        _itemFullscreen = true;

        _fieldPos = 0;

        this->updateKeypadRepeatMode();

        _clearScreenRequested = true;
        _updateRequested = true;

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

                if( _fieldPos >= pos ) {

                    while( pos < _fieldPos ) {
                        text[ pos ] = CHAR_SPACE;

                        pos++;
                    }

                    text[ pos + 1 ] = 0x00;

                    text[ _fieldPos ] = 0x60;
                }

                if( isalpha( text[ _fieldPos ] ) ) {
                    _uppercase = isupper( text[ _fieldPos ] );
                }

                if( shift ) {

                    if( isalpha( text[ _fieldPos ] ) == false ) {
                        return;
                    }

                    if( isupper( text[ _fieldPos ] ) ) {
                        text[ _fieldPos ] = tolower( text[ _fieldPos ] );

                    } else {
                        text[ _fieldPos ] = toupper( text[ _fieldPos ] );
                    }

                } else {

                    text[ _fieldPos ] = this->nextValidCharacter( text[ _fieldPos ] );
                }

                break;
            }

        case ITEM_TYPE_TOGGLE:
        
            if( item->getValueBoolean() == true ) {
                item->setValueBoolean( false );
            } else {
                item->setValueBoolean( true );
            }

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

            pos = this->calcFieldLength( item ) - _fieldPos - 1;

            this->incDigit( ( uint8_t* )item->getValuePtr(), pos, item->getMax(), item->getMin() );
            break;

        case ITEM_TYPE_TIME:
            Time* time;
            time = ( Time* )item->getValuePtr();

            /* Increment hours */
            if( _fieldPos == 0 ) {
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

            if( bitRead( item->getValue(), _fieldPos ) != 0 ) {

                item->setValue( item->getValue() & ~( 1UL << _fieldPos ) );

            } else {
                item->setValue( item->getValue() | ( 1UL << _fieldPos ) );
            }

            break;

        case ITEM_TYPE_IP:

            uint8_t* ip_seg;
            ip_seg = ( ( uint8_t* )item->getValuePtr() ) + ( _fieldPos / 3 );
            pos = ( 2 - ( _fieldPos % 3 ) );

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
            if( _currentScreen.eventValueChange != NULL ) {
                _currentScreen.eventValueChange( this, item );
            }

            if( _currentItem.getValuePtr() != NULL ) {

                ScreenData* link = ( ScreenData* )_currentItem.getValuePtr();
                this->activate( link, true );
            }

            return;
    }

    _itemChanged = true;
    _updateRequested = true;

    if( _currentScreen.eventValueChange != NULL ) {
        _currentScreen.eventValueChange( this, item );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the next valid character when incrementing a text box item
 *
 * @param   current    Current character to start from
 *
 * @return  The next valid character.
 * 
 */
inline char Screen::nextValidCharacter( char current ) {

    /* Sequence : space => [a-z] => [0-9] => symbols => space */

    /* 0x20-0x2F [symbols] */
    /* 0x30-0x3F [0-9] */
    /* 0x41-0x5A [A-Z] */
    /* 0x61-0x7A [a-z] */

    if( current == 0x20 ) {
        return ( _uppercase ) ? 0x41 : 0x61;
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

    if( current > 0x5A && _uppercase == true ) {
        return 0x30;
    }

    if( current > 0x3F && current < 0x41 ) {
        return 0x21;
    }


    return current;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Increments an item's value while keeping it in the valid range.
 *
 * @param   value    Pointer to the item's value
 * @param   pos      Zero-based digit position
 * @param   max      Maximum value
 * @param   min      Minimum value
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Returns the number of digits in a given number.
 *
 * @return  The number of digits.
 * 
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