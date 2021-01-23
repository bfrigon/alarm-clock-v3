//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/screen.h
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

#include "screen_item.h"
#include "../libs/time.h"




#define SCREEN_MAX_BREADCRUMB_ITEMS     10

#define DISPLAY_HEIGHT                  2
#define DISPLAY_WIDTH                   16

#define CHAR_HEALTH                     0x93

/* Characters : Menus */
#define CHAR_BRACE_OPEN                 0xFA
#define CHAR_BRACE_CLOSE                0xFC
#define CHAR_PAGE_UP                    0x1A
#define CHAR_PAGE_DOWN                  0x1B
#define CHAR_BAR_FULL                   0xD1
#define CHAR_BAR_HALF                   0xD3
#define CHAR_UNCHECKED                  0x00
#define CHAR_CHECKED                    0x01
#define CHAR_SELECT                     0x02
#define CHAR_SELECT_REV                 0x03
#define CHAR_FIELD_BEGIN                0x05
#define CHAR_FIELD_END                  0x06
#define CHAR_HALF_BLOCK_RIGHT           0x07
#define CHAR_TITLEBAR_FILL              0xD6
#define CHAR_TITLEBAR_LEFT              0xD8
#define CHAR_TITLEBAR_RIGHT             CHAR_HALF_BLOCK_RIGHT

/* Characters : Root screen */
#define CHAR_WIFI_ON                    0x00
#define CHAR_NO_SD                      0x01
#define CHAR_DEGREE                     0x02
#define CHAR_BATTERY_LOW                0x03
#define CHAR_BATTERY_HALF               0x04
#define CHAR_BATTERY_FULL               0x05
#define CHAR_BATTERY_CHARGING           0x06
#define CHAR_NO_BATTERY                 0x07
#define CHAR_ALARM                      0x92
#define CHAR_TELNET_SESSION             0x94





class Screen;
class ScreenItem;


//**************************************************************************
//
// Events callback function pointers
//
//**************************************************************************

typedef bool ( *pfcbKeypress )( Screen* screen, uint8_t key ) ;
typedef void ( *pfcbValueChange )( Screen* screen, ScreenItem* item );
typedef void ( *pfcbSelectionChanged )( Screen* screen, ScreenItem* item, uint8_t fieldPos, bool fullscreen );
typedef bool ( *pfcbDrawScreen )( Screen* screen );
typedef bool ( *pfcbExitScreen )( Screen* currentScreen );
typedef void ( *pfcbEnterScreen )( Screen* screen );
typedef bool ( *pfcbDrawItem )( Screen* screen, ScreenItem* item, bool isSelected, uint8_t row, uint8_t col );
typedef void ( *pfcbTimeout )( Screen* screen );


//**************************************************************************
//
// Screen data stored in program memory
//
//**************************************************************************
struct ScreenData {

    uint8_t id;
    const struct ScreenItemBase* items;
    const char* customCharacterSet;
    
    pfcbEnterScreen eventEnterScreen;
    pfcbExitScreen eventExitScreen;
    pfcbValueChange eventValueChange;
    pfcbDrawScreen eventDrawScreen;
    pfcbKeypress eventKeypress;
    pfcbDrawItem eventDrawItem;
    pfcbSelectionChanged eventSelectionChanged;
    pfcbTimeout eventTimeout;
};


//**************************************************************************
//
// Screen class
//
//**************************************************************************
class Screen {

  public:

    Screen();

    void update();
    void selectFirstItem();
    void clearSelection();
    void processKeypadEvent( uint8_t key );
    void resetTimeout( int16_t timeout = -1 );
    bool hasScreenTimedOut();
    void exitScreen();
    void activate( const ScreenData* screen, bool selectFirstItem = true );
    void requestScreenUpdate( bool clear );
    void processEvents();


    /* Gets the screen ID. */
    uint8_t getId()                                     { return _currentScreen.id; }

    /* Gets the current screen data object. */
    const ScreenData* getCurrentScreen()                { return _breadCrumbScreen[ _breadCrumbIndex ]; }

    /* Gets the screen return value. */
    uint8_t getReturnValue()                            { return _returnValue; }

    /* Gets the current selected item index. */
    uint8_t getSelectedItemIndex()                      { return _selected; }

     /* Gets the ID of the currently selected item. */
    uint8_t getSelectedItemId()                         { return _currentItem.getId(); }

    /* Gets the current cursor position within the item. */
    uint8_t getCurrentFieldPos()                        { return _fieldPos; }

    /* Gets the screen timeout delay. */
    uint16_t getTimeout()                               { return _timeout; }

    /* Sets the screen timeout delay. */
    void setTimeout( int16_t timeout )                  { this->resetTimeout( timeout ); }

    /* Gets the current scroll position. */
    uint8_t getScrollPos()                              { return _scroll; }

    /* Returns whether or not any items value has changed. */
    bool hasItemsChanged()                              { return _itemChanged; }

    /* Returns whether the current item is shown full screen. */
    bool isItemFullScreen()                             { return _itemFullscreen; }

    /* Gets whether to changes confirm dialog is enabled when exiting the screen. */
    bool getConfirmChanges()                            { return _confirmChanges; }

    /* Enable/disable confirm changes dialog when exiting the screen. */
    void setConfirmChanges( bool confirm )              { _confirmChanges = confirm; }


  private:
    unsigned long _enterScreenTime;
    bool _isShowConfirmDialog;
    bool _itemFullscreen;
    bool _itemChanged;
    bool _confirmChanges;
    uint8_t _selected;
    uint8_t _returnValue;
    uint8_t _fieldPos;
    uint8_t _scroll;
    int16_t _timeout;
    const ScreenData* _breadCrumbScreen[ SCREEN_MAX_BREADCRUMB_ITEMS ];
    uint8_t _breadCrumbSelection[ SCREEN_MAX_BREADCRUMB_ITEMS ];
    uint8_t _breadCrumbIndex;
    
    bool _uppercase = false;
    ScreenItem _currentItem;
    ScreenData _currentScreen;
    bool _updateRequested;
    bool _clearScreenRequested;

    

    void drawItem( ScreenItem* item, bool isSelected, uint8_t row, uint8_t col );
    uint8_t printItemCaption( ScreenItem* item );
    void incrementItemValue( ScreenItem* item, bool shift );
    void clearItemValue( ScreenItem* item );
    void updateCursorPosition();
    void selectItem( uint8_t id );
    bool isItemFullScreenEditable( ScreenItem* item );
    void updateKeypadRepeatMode();
    void incDigit( uint8_t* value, uint8_t pos, uint8_t max, uint8_t min );
    uint8_t calcFieldLength( ScreenItem* item );
    char nextValidCharacter( char current );
    uint8_t itemValueToBars( ScreenItem* item );

    inline uint8_t getNumDigits( uint8_t number );
};

extern Screen g_screen;

#endif /* SCREEN_H */