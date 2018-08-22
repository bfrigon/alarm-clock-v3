//******************************************************************************
//
// Project : Alarm Clock V3
// File    : screen.h
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

#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include "us2066.h"


#define DISPLAY_HEIGHT      2
#define DISPLAY_WIDTH       16


/* Characters : Menus */
#define CHAR_BRACE_OPEN         0xFA
#define CHAR_BRACE_CLOSE        0xFC
#define CHAR_SPACE              0x20
#define CHAR_PAGE_UP            0x1A
#define CHAR_PAGE_DOWN          0x1B
#define CHAR_BAR_FULL           0xD1
#define CHAR_BAR_HALF           0xD3

#define CHAR_UNCHECKED          0x00
#define CHAR_CHECKED            0x01
#define CHAR_SELECT             0x02
#define CHAR_SELECT_REV         0x03
#define CHAR_FIELD_BEGIN        0x05
#define CHAR_FIELD_END          0x06
#define CHAR_HALF_BLOCK_RIGHT   0x07

#define CHAR_TITLEBAR_FILL      0xD6
#define CHAR_TITLEBAR_LEFT      0xD8
#define CHAR_TITLEBAR_RIGHT     CHAR_HALF_BLOCK_RIGHT


/* Characters : Root screen */
#define CHAR_WIFI_ON            0x00
#define CHAR_NO_SD              0x01
#define CHAR_ALARM              0x02
#define CHAR_DEGREE             0x03
#define CHAR_CELCIUS            0x04
#define CHAR_FARENHEIGHT        0x05




#define ITEM_LIST_SRAM_POINTER          0x01
#define ITEM_LIST_PROGMEM_POINTER       0x00
#define ITEM_LIST_POINTER_CALLBACK      0x02

#define ITEM_CAPTION_SRAM_POINTER       0x04
#define ITEM_CAPTION_PROGMEM_POINTER    0x00

#define ITEM_NUMBER_INC_DIGITS          0x00
#define ITEM_NUMBER_INC_WHOLE           0x02


#define ITEM_NOCURSOR                   0x08
#define ITEM_BREAK                      0x10
#define ITEM_EDIT_FULLSCREEN            0x20
#define ITEM_NORMAL                     0x00
#define ITEM_COMPACT                    0x40
#define ITEM_READONLY                   0x80




#define ITEM_TYPE_NULL      0
#define ITEM_TYPE_LINK      1
#define ITEM_TYPE_NUMBER    2
#define ITEM_TYPE_IP        3
#define ITEM_TYPE_TIME      4
#define ITEM_TYPE_MONTH     5
#define ITEM_TYPE_YEAR      6
#define ITEM_TYPE_TOGGLE    7
#define ITEM_TYPE_STATIC    8
#define ITEM_TYPE_LIST      9
#define ITEM_TYPE_TEXT      10
#define ITEM_TYPE_BAR       11
#define ITEM_TYPE_DOW       12






#define RETURN_NONE     0
#define RETURN_NO       1
#define RETURN_YES      2



//--------------------------------------------------------------------------
//
// Macro
//
//--------------------------------------------------------------------------

#define BEGIN_SCREEN_ITEMS( name )  \
    PROGMEM const Item name[] = {

#define END_SCREEN_ITEMS()  \
    { ITEM_TYPE_NULL, 0, 0, 0, NULL, NULL, 0, 0 } };

#define ITEM_TOGGLE( id, row, col, caption, value, options ) \
    { ITEM_TYPE_TOGGLE, id, row, col, caption, value, 0, 0, 0, options },

#define ITEM_LINK( id, row, col, caption, screen, options ) \
    { ITEM_TYPE_LINK, id, row, col, caption, screen, 0, 0, 0, options },

#define ITEM_NUMBER( id, row, col, caption, value, min, max, options) \
    { ITEM_TYPE_NUMBER, id, row, col, caption, value, min, max, 0, options },

#define ITEM_IP( id, row, col, caption, value, options ) \
    { ITEM_TYPE_IP, id, row, col, caption, value, 0, 0, 0, options },

#define ITEM_TIME( id, row, col, caption, value, options ) \
    { ITEM_TYPE_TIME, id, row, col, caption, value, 0, 0, 0, options },

#define ITEM_MONTH( id, row, col, caption, value, options ) \
        { ITEM_TYPE_MONTH, id, row, col, caption, value, 1, 12, 0, options },

#define ITEM_YEAR( id, row, col, caption, value, options ) \
    { ITEM_TYPE_YEAR, id, row, col, caption, value, 0, 255, 0, options },

#define ITEM_STATIC( row, col, caption, options ) \
    { ITEM_TYPE_STATIC, 0, row, col, caption, NULL, 0, 0, 0, options },

#define ITEM_LIST( id, row, col, caption, value, list, min, max, length, options ) \
    { ITEM_TYPE_LIST, id, row, col, caption, value, min, max, length, options, list },

#define ITEM_TEXT( id, row, col, caption, value, length, options ) \
    { ITEM_TYPE_TEXT, id, row, col, caption, value, 0, 0, length, options },

#define ITEM_BAR( id, row, col, caption, value, min, max, length, options ) \
    { ITEM_TYPE_BAR, id, row, col, caption, value, min, max, length, options },

#define ITEM_DOW( id, row, col, caption, value, options ) \
    { ITEM_TYPE_DOW, id, row, col, caption, value, 0, 0, 0, options },




//--------------------------------------------------------------------------
//
// Screen items
//
//--------------------------------------------------------------------------

struct Item {
    uint8_t type;
    uint8_t id;
    uint8_t row;
    uint8_t col;
    const void *caption;
    void *value;
    uint16_t min;
    uint16_t max;
    uint8_t length;
    uint8_t options;
    const void *list;
};



class Screen;

void gotoScreen( Screen *screen, bool selectFirstItem, Screen *parent );
inline uint8_t numDigits( uint8_t number );



extern Screen *g_currentScreen;
extern Item g_currentItem;
extern unsigned long g_enterScreenTime;

extern bool g_screenUpdate;
extern bool g_screenClear;
extern US2066 g_lcd;


//--------------------------------------------------------------------------
//
// Events callback function pointers
//
//--------------------------------------------------------------------------


typedef bool ( *pfEventKeypress )( Screen *screen, uint8_t key ) ;
typedef bool ( *pfEventValueChange )( Screen *screen, Item *item );
typedef void ( *pfEventSelectionChanged )( Screen *screen, Item *item, uint8_t fieldPos, bool fullscreen );
typedef bool ( *pfEventDrawScreen )( Screen *screen );
typedef bool ( *pfEventExitScreen )( Screen *currentScreen, Screen *newScreen );
typedef bool ( *pfEventEnterScreen )( Screen *screen );
typedef bool ( *pfEventDrawItem ) ( Screen *screen, Item *item, bool isSelected, uint8_t row, uint8_t col );
typedef void ( *pfEventTimeout ) ( Screen *screen );


//--------------------------------------------------------------------------
//
// Screen
//
//--------------------------------------------------------------------------
class Screen {

  public:

    Screen( uint8_t _id );
    Screen( uint8_t _id, const Item *_items, pfEventValueChange _eventValueChange,
            pfEventEnterScreen _eventEnterScreen, pfEventExitScreen _eventExitScreen );

    const Item *items = NULL;
    Screen *parent = NULL;


    pfEventKeypress eventKeypress = NULL;
    pfEventValueChange eventValueChange = NULL;
    pfEventDrawScreen eventDrawScreen = NULL;
    pfEventExitScreen eventExitScreen = NULL;
    pfEventEnterScreen eventEnterScreen = NULL;
    pfEventDrawItem eventDrawItem = NULL;
    pfEventSelectionChanged eventSelectionChanged = NULL;
    pfEventTimeout eventTimeout = NULL;


    uint8_t id = 0;
    uint8_t selected = 0;
    uint8_t scroll = 0;
    uint8_t fieldPos = 0;
    bool itemChanged = false;
    bool confirmChanges = false;
    uint8_t returnValue = 0;
    bool upperCase = false;
    uint16_t timeout = 0;

    void init( bool selectFirstItem );
    void update();
    void selectFirstItem();
    void clearSelection();
    void processKeypadEvent( uint8_t key );

    void resetTimeout();
    bool isTimeout();
    bool isFullscreen();

    void exitScreen();

  private:

    bool _isShowConfirmDialog = false;
    bool _itemFullscreen = false;


    void drawItem( Item *item, bool isSelected, uint8_t row, uint8_t col );
    uint8_t printItemCaption( Item *item );
    void incrementItemValue( Item *item, bool shift );
    void clearItemValue( Item *item );
    void updateCursorPosition();
    void selectItem( uint8_t id );
    bool isItemFullScreenEditable( Item *item );
    void updateKeypadRepeatMode();

    void incDigit(uint8_t *value, uint8_t pos, uint8_t max, uint8_t min);
    uint8_t calcFieldLength( Item *item );
    char nextValidCharacter( char current );
    uint8_t itemValueToBars( Item *item );

};




#endif /* SCREEN_H */
