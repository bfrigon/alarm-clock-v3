//******************************************************************************
//
// Project : Alarm Clock V3
// File    : screen.cpp
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



Screen *g_currentScreen;
Item g_currentItem = {};
bool g_screenUpdate = false;
bool g_screenClear = false;
const char *g_currentCustomCharacterSet = NULL;

unsigned long g_enterScreenTime = 0;



Screen::Screen( uint8_t _id ) {
    this->id = _id;
}

Screen::Screen( uint8_t _id, const Item *_items, pfEventValueChange _eventValueChange,
                pfEventEnterScreen _eventEnterScreen, pfEventExitScreen _eventExitScreen ) {

    this->id = _id;
    this->items = _items;
    this->eventValueChange = _eventValueChange;
    this->eventEnterScreen = _eventEnterScreen;
    this->eventExitScreen = _eventExitScreen;
}


void Screen::init( bool selectFirstItem ) {

    this->returnValue = RETURN_NONE;
    this->_isShowConfirmDialog = false;
    this->itemChanged = false;
    this->upperCase = false;

    if( selectFirstItem ) {
        this->selectFirstItem();
    }

    /* Reselect the item */
    selectItem( this->selected );
}


void Screen::update( bool force = false ) {

    Item item;

    /* Do not draw the screen if it's not the current active screen */
    if( g_currentScreen != this && force == false ) {
        return;
    }

    if( g_screenClear )  {
        g_lcd.clear();
        g_screenClear = false;
    }

    if( this->customCharacterSet != g_currentCustomCharacterSet ) {
        g_currentCustomCharacterSet = this->customCharacterSet;

        g_lcd.setCustomCharacters( this->customCharacterSet );
    }

    /* Disable blinking cursor */
    g_lcd.setCursor( false, false );

    if( this->_isShowConfirmDialog == true ) {
        g_lcd.setPosition( 0, 0 );
        g_lcd.print_P( S_QUESTION_SAVE );

        g_lcd.setPosition( 1, 0 );
        g_lcd.print( ( this->fieldPos == 0 ) ? CHAR_SELECT : CHAR_SPACE );
        g_lcd.print_P( S_NO );

        g_lcd.setPosition( 1, 8 );
        g_lcd.print( ( this->fieldPos == 1 ) ? CHAR_SELECT : CHAR_SPACE );
        g_lcd.print_P( S_YES );
        return;
    }


    /* Allow the callback to override the default screen drawing */
    if( this->eventDrawScreen != NULL ) {

        g_lcd.setPosition( 0, 0 );

        if( this->eventDrawScreen( this ) == false ) {
            return;
        }
    }

    if( this->items == NULL ) {
        return;
    }

    /* If the current selected item is full screen, only draw this item */
    if( this->_itemFullscreen == true ) {

        memcpy_P( &item, &this->items[ this->selected ], sizeof( Item ) );
        this->drawItem( &item, true, 0, 0 );


    } else {

        /* Draw all visible items on the screen */
        for( uint8_t i = 0; true; i++ ) {

            memcpy_P( &item, &this->items[ i ], sizeof( Item ) );

            bool isSelected = ( this->selected == i );
            uint8_t row = item.row - this->scroll;
            uint8_t col = item.col;

            /* No more items to draw */
            if( item.type == ITEM_TYPE_NULL ) {
                break;
            }

            /* Ignore items above the scroll window, go to the next item. */
            if( item.row < this->scroll ) {
                continue;
            }

            /* Draw page up symbol if there are items above the scroll window. */
            if( this->scroll > 0 ) {

                g_lcd.setPosition( 0, DISPLAY_WIDTH - 1 );
                g_lcd.print( CHAR_PAGE_UP );
            }

            /* Don't draw items below the scroll window, no need to continue. */
            if( item.row > this->scroll + DISPLAY_HEIGHT - 1 ) {

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


void Screen::processKeypadEvent( uint8_t key ) {

    /* Allow the callback to override the keypress event */
    if( this->eventKeypress != NULL ) {

        if( this->eventKeypress( this, key ) == false ) {
            return;
        }
    }

    switch( key ) {

        /* Exit current menu */
        case KEY_MENU:
        case KEY_SWIPE | KEY_LEFT:

            if( this->_itemFullscreen == true ) {

                if( this->eventSelectionChanged != NULL ) {
                    this->eventSelectionChanged( this, &g_currentItem, this->fieldPos, false );
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

                this->returnValue = this->fieldPos + 1;

                gotoScreen( this->parent, false, NULL );
                return;
            }

            /* Or increment the value of the item */
            this->incrementItemValue( &g_currentItem, false );

            break;

        case KEY_SWIPE | KEY_RIGHT:

            this->incrementItemValue( &g_currentItem, true );
            break;

        case KEY_SHIFT | KEY_MENU:
            this->clearItemValue( &g_currentItem );
            break;

        /* Go to next item or next digit position.*/
        case KEY_NEXT:

            this->fieldPos++;


            if( this->_isShowConfirmDialog == true ) {

                if( this->fieldPos > 1 ) {
                    this->fieldPos = 0;
                }

                g_screenUpdate = true;
                return;
            }


            /* increment cursor position within the current item, if
            it reaches the item length, skip to the next item */
            if( this->fieldPos >= this->calcFieldLength( &g_currentItem ) ) {

                this->fieldPos = 0;

                if( this->_itemFullscreen == false ) {

                    this->selectItem( this->selected + 1 );
                }

                g_screenUpdate = true;
            }


            if( this->eventSelectionChanged != NULL ) {
                this->eventSelectionChanged( this, &g_currentItem, this->fieldPos, this->_itemFullscreen );
            }

            g_screenUpdate = true;
            break;
    }

}

void Screen::resetTimeout() {

    g_enterScreenTime = millis();
}

bool Screen::isTimeout() {

    if( this->timeout == 0 ) {
        return false;
    }

    return ( ( millis() - g_enterScreenTime ) > this->timeout );
}

void Screen::exitScreen() {
    if( this->parent == NULL ) {
        return;
    }

    /* If items changed, show the confirm changes dialog */
    if( this->confirmChanges && this->itemChanged ) {

        this->_isShowConfirmDialog = true;

        this->clearSelection();

        return;
    }

    /* Goto back to parent screen */
    gotoScreen( this->parent, false, NULL );
    this->parent = NULL;

    return;
}


void Screen::drawItem( Item *item, bool isSelected, uint8_t row, uint8_t col ) {

    uint8_t value;

    g_lcd.setPosition( row, col );

    if( this->isItemFullScreenEditable( item ) ) {

        if( this->_itemFullscreen == true ) {

            if( item->caption != NULL ) {
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

        if( ( item->options & ITEM_COMPACT ) == 0 ) {

            switch( item->type ) {

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
                    if( item->caption != NULL ) {
                        g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );

                        this->printItemCaption( item );
                        g_lcd.print_P( S_SEPARATOR );
                    }

                    break;
            }
        }
    }

    /* Allow the callback to override the default item drawing. */
    if( this->eventDrawItem != NULL ) {

        if( this->eventDrawItem( this, item, isSelected, row, col ) == false ) {
            return;
        }
    }

    switch( item->type ) {

        case ITEM_TYPE_BAR:

            uint8_t nbars;
            nbars = this->itemValueToBars( item );

            uint8_t npadding;
            npadding = item->length - ( nbars / 2 );

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

            if( item->length > maxWidth ) {
                uint8_t offset;
                offset = this->fieldPos > ( maxWidth - 1 ) ? this->fieldPos - maxWidth + 1 : 0;

                g_lcd.print( ( char * )item->value + offset, maxWidth, TEXT_ALIGN_LEFT );

            } else {
                g_lcd.print( ( char * )item->value, maxWidth, TEXT_ALIGN_LEFT );

            }

            g_lcd.print( CHAR_FIELD_END );


            break;

        case ITEM_TYPE_LIST:

            if( item->options & ITEM_COMPACT || item->options & ITEM_EDIT_FULLSCREEN ) {

                g_lcd.print( isSelected ? CHAR_SELECT : CHAR_FIELD_BEGIN );
            }



            if( item->list != NULL ) {

                uint8_t offset = 0;

                if( item->value != NULL ) {
                    offset = *( ( uint8_t * )item->value ) * ( item->length + 1 );
                }

                if( item->options & ITEM_LIST_SRAM_POINTER ) {
                    g_lcd.print( ( ( const char * )item->list ) + offset, item->length, TEXT_ALIGN_LEFT );

                } else {
                    g_lcd.print_P( ( ( const char * )item->list ) + offset, item->length, TEXT_ALIGN_LEFT );
                }

            } else {
                g_lcd.fill( CHAR_SPACE, item->length );
            }

            if( item->options & ITEM_COMPACT || item->options & ITEM_EDIT_FULLSCREEN ) {
                g_lcd.print( isSelected ? CHAR_SELECT_REV : CHAR_FIELD_END );
            }

            break;

        case ITEM_TYPE_MONTH:
            g_lcd.print_P( getMonthName( *( ( uint8_t * )item->value ), true ) );
            break;

        case ITEM_TYPE_STATIC:
            this->printItemCaption( item );
            break;

        /* On/off */
        case ITEM_TYPE_TOGGLE:

            if( item->options & ITEM_COMPACT ) {

                if( item->caption != NULL ) {
                    g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );

                    this->printItemCaption( item );
                    g_lcd.print( CHAR_SPACE );

                    g_lcd.print( *( ( bool * )item->value ) ? CHAR_CHECKED : CHAR_SPACE );

                } else {
                    g_lcd.print( *( ( bool * )item->value ) ? CHAR_CHECKED : CHAR_UNCHECKED );
                }



            } else {

                if( item->caption == NULL ) {
                    g_lcd.print( isSelected ? CHAR_SELECT : CHAR_SPACE );
                }

                g_lcd.print_P( *( ( bool * )item->value ) == true ? S_ON : S_OFF, 3, TEXT_ALIGN_LEFT );
            }

            break;


        case ITEM_TYPE_NUMBER:
            value = *( ( uint8_t * )item->value );

            /* Left padding */
            g_lcd.fill( CHAR_SPACE, numDigits( item->max ) - numDigits( value ) );

            g_lcd.printf( "%d", *( uint8_t * )item->value );

            break;

        case ITEM_TYPE_TIME:
            Time *time;
            time = ( Time * )item->value;

            uint8_t hour;
            hour = time->hour;

            if( g_config.clock_24h == false ) {

                hour = time->hour % 12;

                if( hour == 0 ) {
                    hour = 12;
                }
            }

            g_lcd.printf( "%2d:%02d ", hour, time->minute );

            if( g_config.clock_24h == false ) {
                g_lcd.print_P( ( time->hour > 11 ) ? S_PM : S_AM );
            }

            break;

        case ITEM_TYPE_YEAR:
            uint8_t year;
            year = *( ( uint8_t * )item->value );

            g_lcd.printf( "20%02d", year );

            break;

        case ITEM_TYPE_IP:

            g_lcd.printf( "%3d.%3d.%3d.%3d",
                          *( ( uint8_t * )item->value ),
                          *( ( uint8_t * )item->value + 1 ),
                          *( ( uint8_t * )item->value + 2 ),
                          *( ( uint8_t * )item->value + 3 )
                        );

        case ITEM_TYPE_DOW:

            uint8_t i;

            for( i = 0; i < 7; i++ ) {

                if( bitRead( *( ( uint8_t * )item->value ), i ) != 0 ) {

                    char c;
                    c = pgm_read_byte( S_DOW + i );

                    g_lcd.print( c );

                } else {
                    g_lcd.print( "-" );
                }

                if( ( item->options & ITEM_COMPACT ) == 0 ) {
                    g_lcd.print( CHAR_SPACE );
                }
            }

            break;
    }

}


bool Screen::isItemFullScreenEditable( Item *item ) {

    switch( item->type ) {

        /* These items types cannot be editable full screen */
        case ITEM_TYPE_LINK:
        case ITEM_TYPE_STATIC:
        case ITEM_TYPE_TOGGLE:
            return false;
    }

    return item->options & ITEM_EDIT_FULLSCREEN;
}

bool Screen::isFullscreen() {

    return this->_itemFullscreen;
}


uint8_t Screen::printItemCaption( Item *item ) {

    uint8_t nchr = 0;

    if( item->caption == NULL ) {
        return 0;
    }

    if( item->options & ITEM_CAPTION_SRAM_POINTER ) {

        /* String stored in SRAM */
        nchr = g_lcd.print( ( const char * )item->caption );

    } else {

        /* String stored in program memory */
        nchr = g_lcd.print_P( ( const char * )item->caption );
    }

    return nchr;
}


void Screen::selectFirstItem() {
    this->scroll = 0;
    this->_itemFullscreen = false;
    this->_isShowConfirmDialog = false;

    this->selectItem( 0 );
}

void Screen::clearSelection() {

    memset( &g_currentItem, 0, sizeof( Item ) );

    this->selected = 0;
    this->scroll = 0;
    this->fieldPos = 0;
    this->upperCase = false;
    this->_itemFullscreen = false;

    this->updateKeypadRepeatMode();

    g_screenUpdate = true;
    g_screenClear = true;

    if( this->eventSelectionChanged != NULL ) {
        this->eventSelectionChanged( this, &g_currentItem, 0, false );
    }
}

void Screen::selectItem( uint8_t index ) {

    uint8_t n_listIter = 0;
    this->fieldPos = 0;
    this->upperCase = false;
    this->selected = index;
    this->_itemFullscreen = false;

    if( this->items == NULL ) {

        this->clearSelection();
        return;
    }

    while( true ) {

        memcpy_P( &g_currentItem, &this->items[ this->selected ], sizeof( Item ) );

        /* Give up looking for a selectable item if scanned list more than 1 time */
        if( n_listIter > 1 ) {
            this->clearSelection();

            return;
        }

        switch( g_currentItem.type ) {

            /* End of list */
            case ITEM_TYPE_NULL:
                this->selected = 0;

                if( this->scroll > 0 ) {
                    this->scroll = 0;
                    g_screenClear = true;
                }

                n_listIter++;
                break;

            /* Non-selectable item */
            case ITEM_TYPE_STATIC:
                this->selected++;
                break;

            /* Selectable item */
            default:

                if( this->eventSelectionChanged != NULL ) {
                    this->eventSelectionChanged( this, &g_currentItem, this->fieldPos, false );
                }

                /* Skip over read-only items */
                if( g_currentItem.options & ITEM_READONLY ) {

                    this->selected++;
                    break;
                }

                if( g_currentItem.row > this->scroll + DISPLAY_HEIGHT - 1 ) {

                    if( g_currentItem.options & ITEM_BREAK ) {
                        /* Scroll item to the top of the screen */
                        this->scroll = g_currentItem.row;

                    } else {
                        /* Scroll item to the bottom of the screen */
                        this->scroll = g_currentItem.row - ( DISPLAY_HEIGHT - 1 );
                    }

                    g_screenClear = true;
                }

                this->updateKeypadRepeatMode();

                return;
        }
    }
}


void Screen::updateKeypadRepeatMode() {

    if( this->_isShowConfirmDialog == true ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    if( this->items == NULL ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    if( ( this->isItemFullScreenEditable( &g_currentItem ) ) && ( this->_itemFullscreen == false ) ) {
        g_keypad.repeatMask = KEY_NONE;
        return;
    }

    switch( g_currentItem.type ) {
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


uint8_t Screen::calcFieldLength( Item *item ) {
    uint8_t length = 0;

    if( ( this->isItemFullScreenEditable( item ) ) && ( this->_itemFullscreen == false ) ) {
        return 0;
    }


    /* Determine item length */
    switch( item->type ) {
        case ITEM_TYPE_IP:
            length = 12;
            break;

        case ITEM_TYPE_TIME:
            length = 2;
            break;

        case ITEM_TYPE_NUMBER:

            if( item->options & ITEM_NUMBER_INC_WHOLE ) {
                length = 1;

            } else {
                length = numDigits( item->max );
            }

            break;

        case ITEM_TYPE_YEAR:
            length = 2;
            break;

        case ITEM_TYPE_DOW:
            length = 7;
            break;

        case ITEM_TYPE_TEXT:
            length = item->length;
            break;

    }

    return length;
}


void Screen::updateCursorPosition() {


    uint8_t row = g_currentItem.row - this->scroll;
    uint8_t col = g_currentItem.col;

    if( g_currentItem.options & ITEM_NOCURSOR ) {
        return;
    }

    if( this->isItemFullScreenEditable( &g_currentItem ) ) {

        if( this->_itemFullscreen == false ) {
            /* No blinking cursor if not currently full screen */
            return;

        } else {
            row = 1;
            col = 0;
        }

    } else {

        if( ( g_currentItem.options & ITEM_COMPACT ) == 0 && g_currentItem.caption != NULL ) {

            if( g_currentItem.options & ITEM_CAPTION_SRAM_POINTER ) {

                col += strlen( ( const char * )g_currentItem.caption ) + 3;

            } else {
                col += strlen_P( ( const char * )g_currentItem.caption ) + 3;
            }
        }
    }

    switch( g_currentItem.type ) {
        case ITEM_TYPE_TIME:

            if( this->fieldPos == 0 ) {
                g_lcd.setPosition( row, col + 1 );

            } else {
                g_lcd.setPosition( row, col + 4 );
            }

            break;

        case ITEM_TYPE_IP:
            g_lcd.setPosition( row, this->fieldPos + ( this->fieldPos / 3 ) );
            break;

        case ITEM_TYPE_NUMBER:

            if( g_currentItem.options & ITEM_NUMBER_INC_WHOLE ) {
                g_lcd.setPosition( row, col + numDigits( g_currentItem.max ) - 1 );

            } else {
                g_lcd.setPosition( row, col + this->fieldPos );
            }

            break;

        case ITEM_TYPE_TEXT:
            uint8_t maxWidth;
            maxWidth = DISPLAY_WIDTH - col - 2;

            g_lcd.setPosition( row, col + ( this->fieldPos > ( maxWidth - 1 ) ? maxWidth - 1 : this->fieldPos ) + 1 );
            break;

        case ITEM_TYPE_YEAR:
            g_lcd.setPosition( row, col + this->fieldPos + 2 );
            break;

        case ITEM_TYPE_MONTH:
            g_lcd.setPosition( row, col );
            break;

        case ITEM_TYPE_TOGGLE:

            if( g_currentItem.caption != NULL || ( g_currentItem.options & ITEM_COMPACT ) == 0 ) {
                return;
            }

            g_lcd.setPosition( row, col );
            break;

        case ITEM_TYPE_DOW:

            if( g_currentItem.options & ITEM_COMPACT ) {
                g_lcd.setPosition( row, col + this->fieldPos );

            } else {
                g_lcd.setPosition( row, col + ( this->fieldPos * 2 ) );
            }

            break;


        /* No blinking cursor for other items */
        default:
            return;

    }

    /* Enable underline cursor */
    g_lcd.setCursor( false, true );
}


void Screen::clearItemValue( Item *item ) {

    if( ( this->isItemFullScreenEditable( item ) ) && ( this->_itemFullscreen == false ) ) {
        return;
    }

    switch( item->type ) {

        case ITEM_TYPE_TEXT:

            for( uint8_t i = this->fieldPos; i < item->length; i++ ) {
                ( ( char * )item->value )[ i ] = 0x00;
            }

            this->upperCase = false;
            break;

        case ITEM_TYPE_NUMBER:
        case ITEM_TYPE_BAR:
        case ITEM_TYPE_LIST:
        case ITEM_TYPE_YEAR:
            *( ( uint8_t * )item->value ) = item->min;
            break;

        case ITEM_TYPE_MONTH:
            *( ( uint8_t * )item->value ) = 1;
            break;

        case ITEM_TYPE_DOW:
            if( *( ( uint8_t * )item->value ) != 0 ) {
                *( ( uint8_t * )item->value ) = 0;

            } else {
                *( ( uint8_t * )item->value ) = 0x7F;
            }

            break;

        default:
            return;

    }

    this->itemChanged = true;

    g_screenUpdate = true;

    if( this->eventValueChange != NULL ) {
        this->eventValueChange( this, item );
    }
}


uint8_t Screen::itemValueToBars( Item *item ) {

    uint8_t value;
    value = *( ( uint8_t * )item->value );

    if( value < item->min ) {
        value = item->min;
    }

    if( value > item->max ) {
        value = item->max;
    }

    double nbars;
    nbars = ( value - item->min ) * ( item->length * 2 );
    nbars /= ( item->max - item->min );

    return ( uint8_t )lround( nbars );
}


void Screen::incrementItemValue( Item *item, bool shift ) {

    uint8_t pos;
    char chr;

    if( item->options & ITEM_READONLY ) {
        return;
    }

    if( ( this->isItemFullScreenEditable( item ) ) && ( this->_itemFullscreen == false ) ) {

        if( this->eventSelectionChanged != NULL ) {
            this->eventSelectionChanged( this, item, this->fieldPos, true );
        }

        this->_itemFullscreen = true;

        this->fieldPos = 0;

        this->updateKeypadRepeatMode();

        g_screenUpdate = true;
        g_screenClear = true;
        return;
    }

    switch( item->type ) {

        case ITEM_TYPE_BAR:

            uint8_t nbars;
            nbars = this->itemValueToBars( item );

            if( ( item->max - item->min ) / ( item->length * 2 ) > 0 ) {

                nbars++;

                if( nbars > ( item->length * 2 ) ) {
                    nbars = 0;
                }

                ( *( ( uint8_t * )item->value ) ) = item->min + nbars * ( item->max - item->min ) / ( item->length * 2 );

            } else {

                ( *( ( uint8_t * )item->value ) )++;

                if( *( ( uint8_t * )item->value ) > item->max ) {
                    *( ( uint8_t * )item->value ) = item->min;
                }
            }

            break;


        case ITEM_TYPE_TEXT:

            chr = ( ( char * )item->value )[ this->fieldPos ];

            pos = strlen( ( char * )item->value );

            if( this->fieldPos >= pos ) {

                while( pos < this->fieldPos ) {
                    ( ( char * )item->value )[ pos ] = CHAR_SPACE;

                    pos++;
                }

                ( ( char * )item->value )[ pos + 1 ] = 0x00;

                chr = 0x60;
            }

            if( isalpha( chr ) ) {
                this->upperCase = isupper( chr );
            }

            if( shift ) {

                if( isalpha( chr ) == false ) {
                    return;
                }

                if( isupper( chr ) ) {
                    chr = tolower( chr );

                } else {
                    chr = toupper( chr );
                }

            } else {

                chr = this->nextValidCharacter( chr );
            }

            ( ( char * )item->value )[ this->fieldPos ] = chr;
            break;


        case ITEM_TYPE_TOGGLE:

            *( bool * )item->value = !( *( bool * ) item->value );
            break;

        case ITEM_TYPE_NUMBER:

            if( item->options & ITEM_NUMBER_INC_WHOLE ) {

                ( *( ( uint8_t * )item->value ) )++;

                if( *( ( uint8_t * )item->value ) > item->max ) {
                    *( ( uint8_t * )item->value ) = item->min;
                }

                break;
            }

        /* Fall-through */

        case ITEM_TYPE_YEAR:

            pos = this->calcFieldLength( item ) - this->fieldPos - 1;

            this->incDigit( ( uint8_t * )item->value, pos, item->max, item->min );
            break;

        case ITEM_TYPE_TIME:
            Time *time;
            time = ( Time * )item->value;

            /* Increment hours */
            if( this->fieldPos == 0 ) {
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

            if( bitRead( *( ( uint8_t * )item->value ), this->fieldPos ) != 0 ) {

                bitClear( *( ( uint8_t * )item->value ), this->fieldPos );

            } else {
                bitSet( *( ( uint8_t * )item->value ), this->fieldPos );
            }

            break;

        case ITEM_TYPE_IP:

            uint8_t *ip_seg;
            ip_seg = ( ( uint8_t * )item->value ) + ( this->fieldPos / 3 );
            pos = ( 2 - ( this->fieldPos % 3 ) );

            this->incDigit( ip_seg, pos, 255, 1 );

            break;

        case ITEM_TYPE_MONTH:
        case ITEM_TYPE_LIST:

            if( item->value == NULL ) {
                break;
            }

            ( *( ( uint8_t * )item->value ) )++;

            if( *( ( uint8_t * )item->value ) > item->max ) {
                *( ( uint8_t * )item->value ) = item->min;
            }

            break;

        case ITEM_TYPE_LINK:

            if( this->eventValueChange != NULL ) {
                this->eventValueChange( this, item );
            }

            if( item->value != NULL ) {
                gotoScreen( ( Screen * )item->value, true, this );
            }

            return;
    }

    this->itemChanged = true;
    g_screenUpdate = true;

    if( this->eventValueChange != NULL ) {
        this->eventValueChange( this, item );
    }
}

inline char Screen::nextValidCharacter( char current ) {

    /* Sequence : space => [a-z] => [0-9] => symbols => space */

    /* 0x20-0x2F [symbols] */
    /* 0x30-0x3F [0-9] */
    /* 0x41-0x5A [A-Z] */
    /* 0x61-0x7A [a-z] */

    if( current == 0x20 ) {
        return ( this->upperCase ) ? 0x41 : 0x61;
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

    if( current > 0x5A && this->upperCase == true ) {
        return 0x30;
    }

    if( current > 0x3F && current < 0x41 ) {
        return 0x21;
    }


    return current;
}


void Screen::incDigit( uint8_t *value, uint8_t pos, uint8_t max, uint8_t min ) {

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


inline uint8_t numDigits( uint8_t number ) {
    if( number < 10 ) {
        return 1;
    }

    if( number < 100 ) {
        return 2;
    }

    return 3;
}


void gotoScreen( Screen *screen, bool selectFirstItem, Screen *parent ) {
    if( screen == NULL ) {
        return;
    }

    /* Trigger exit screen event. If callback return false, cancel screen change */
    if( g_currentScreen->eventExitScreen != NULL ) {
        if( g_currentScreen->eventExitScreen( g_currentScreen, screen ) == false ) {
            return;
        }
    }

    g_currentScreen = screen;
    screen->init( selectFirstItem );

    g_enterScreenTime = millis();


    if( parent != NULL && parent != screen ) {
        screen->parent = parent;
    }


    /* Trigger enter screen event on new screen */
    if( screen->eventEnterScreen != NULL ) {
        screen->eventEnterScreen( screen );
    }

    g_screenClear = true;
    g_screenUpdate = true;
}