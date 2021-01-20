//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/screen.h
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
#include "libs/time.h"
#include "libs/itask.h"
#include "drivers/qt1070.h"
#include "drivers/us2066.h"



#define DISPLAY_HEIGHT      2
#define DISPLAY_WIDTH       16

#define CHAR_HEALTH             0x93

/* Characters : Menus */
#define CHAR_BRACE_OPEN         0xFA
#define CHAR_BRACE_CLOSE        0xFC
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
#define CHAR_DEGREE             0x02
#define CHAR_BATTERY_LOW        0x03
#define CHAR_BATTERY_HALF       0x04
#define CHAR_BATTERY_FULL       0x05
#define CHAR_BATTERY_CHARGING   0x06
#define CHAR_NO_BATTERY         0x07
#define CHAR_ALARM              0x92
#define CHAR_TELNET_SESSION     0x94




/* Item attributes */
#define ITEM_NOCURSOR                   0x08
#define ITEM_BREAK                      0x10
#define ITEM_EDIT_FULLSCREEN            0x20
#define ITEM_NORMAL                     0x00
#define ITEM_COMPACT                    0x40
#define ITEM_READONLY                   0x80
#define ITEM_LIST_SRAM_POINTER          0x01
#define ITEM_LIST_PROGMEM_POINTER       0x00
#define ITEM_LIST_POINTER_CALLBACK      0x02
#define ITEM_CAPTION_SRAM_POINTER       0x04
#define ITEM_CAPTION_PROGMEM_POINTER    0x00
#define ITEM_NUMBER_INC_DIGITS          0x00
#define ITEM_NUMBER_INC_WHOLE           0x02

/* Item types */
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

/* Return values */
#define RETURN_NONE     0
#define RETURN_NO       1
#define RETURN_YES      2



//**************************************************************************
//
// Screen item definition macros
//
//**************************************************************************

#define ITEM_END() \
    { ITEM_TYPE_NULL, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL }

#define ITEM_TOGGLE( id, row, col, caption, value, options ) \
    { ITEM_TYPE_TOGGLE, id, row, col, caption, value, 0, 0, 0, options, NULL }

#define ITEM_LINK( id, row, col, caption, screen, options ) \
    { ITEM_TYPE_LINK, id, row, col, caption, screen, 0, 0, 0, options, NULL }

#define ITEM_NUMBER( id, row, col, caption, value, min, max, options) \
    { ITEM_TYPE_NUMBER, id, row, col, caption, value, min, max, 0, options, NULL }

#define ITEM_IP( id, row, col, caption, value, options ) \
    { ITEM_TYPE_IP, id, row, col, caption, value, 0, 0, 0, options, NULL }

#define ITEM_TIME( id, row, col, caption, value, options ) \
    { ITEM_TYPE_TIME, id, row, col, caption, value, 0, 0, 0, options, NULL }

#define ITEM_MONTH( id, row, col, caption, value, options ) \
    { ITEM_TYPE_MONTH, id, row, col, caption, value, 1, 12, 0, options, NULL }

#define ITEM_YEAR( id, row, col, caption, value, options ) \
    { ITEM_TYPE_YEAR, id, row, col, caption, value, 0, 99, 0, options, NULL }

#define ITEM_STATIC( row, col, caption, options ) \
    { ITEM_TYPE_STATIC, 0, row, col, caption, NULL, 0, 0, 0, options, NULL }

#define ITEM_LIST( id, row, col, caption, value, list, min, max, length, options ) \
    { ITEM_TYPE_LIST, id, row, col, caption, value, min, max, length, options, list }

#define ITEM_TEXT( id, row, col, caption, value, length, options ) \
    { ITEM_TYPE_TEXT, id, row, col, caption, value, 0, 0, length, options, NULL }

#define ITEM_BAR( id, row, col, caption, value, min, max, length, options ) \
    { ITEM_TYPE_BAR, id, row, col, caption, value, min, max, length, options, NULL }

#define ITEM_DOW( id, row, col, caption, value, options ) \
    { ITEM_TYPE_DOW, id, row, col, caption, value, 0, 0, 0, options, NULL }



//**************************************************************************
//
//
//
//**************************************************************************
class Screen;
class ScreenItem;

extern Screen* g_currentScreen;
extern bool g_screenUpdate;
extern bool g_screenClear;
extern const char* g_currentCustomCharacterSet;


//**************************************************************************
//
// Events callback function pointers
//
//**************************************************************************

typedef bool ( *pfcbKeypress )( Screen* screen, uint8_t key ) ;
typedef void ( *pfcbValueChange )( Screen* screen, ScreenItem* item );
typedef void ( *pfcbSelectionChanged )( Screen* screen, ScreenItem* item, uint8_t fieldPos, bool fullscreen );
typedef bool ( *pfcbDrawScreen )( Screen* screen );
typedef bool ( *pfcbExitScreen )( Screen* currentScreen, Screen* newScreen );
typedef bool ( *pfcbEnterScreen )( Screen* screen );
typedef bool ( *pfcbDrawItem )( Screen* screen, ScreenItem* item, bool isSelected, uint8_t row, uint8_t col );
typedef void ( *pfcbTimeout )( Screen* screen );


//**************************************************************************
//
// Screen items class
//
//**************************************************************************

struct ScreenItemBase {
    uint8_t _type;
    uint8_t _id;
    uint8_t _row;
    uint8_t _col;
    const void* _caption;
    void* _value;
    uint16_t _min;
    uint16_t _max;
    uint8_t _length;
    uint8_t _options;
    const void* _list;
};

class ScreenItem : protected ScreenItemBase {

  public:

    //----------------------------------------------------------------------
    // Class constructors
    //----------------------------------------------------------------------
    ScreenItem();


    //----------------------------------------------------------------------
    // Methods
    //----------------------------------------------------------------------
    void loadFromProgmem( const struct ScreenItemBase* item );
    void unload();


    //----------------------------------------------------------------------
    // Properties
    //----------------------------------------------------------------------

    /* Gets the item type. */
    uint8_t getType()                   { return this->_type; }

    /* Gets the item ID. */
    uint8_t getId()                     { return this->_id; }

    /* Gets the zero-based Y position on the LCD. */
    uint8_t getPositionRow()            { return this->_row; }

    /* Gets the zero-based X position on the LCD. */
    uint8_t getPositionCol()            { return this->_col; }

    /* Gets a pointer to the caption. */
    const void* getCaption()            { return this->_caption; }

    /* Gets the item minimum value. */
    uint8_t getMin()                    { return this->_min; }

    /* Gets the item maximum value. */
    uint8_t getMax()                    { return this->_max; }

    /* Gets the item options */
    uint8_t getOptions()                { return this->_options; }

    /* Gets the item field length */
    uint8_t getLength()                 { return this->_length; }

    /* Gets a pointer to the list items */
    const void* getListPtr()            { return this->_list; }

    /* Gets a pointer to the value */
    void* getValuePtr()                 { return this->_value; }

    /* Gets the value (unsigned integer)  */
    uint8_t getValue()                  { return *( ( uint8_t* )this->_value ); }

    /* Gets the value (boolean)  */
    bool getValueBoolean()              { return *( ( bool* )this->_value ); }

    /* Sets the value (unsigned integer )  */
    void setValue( uint8_t value )      { *( ( uint8_t* )this->_value ) = value; }

    /* Sets the value (boolean)  */
    void setValueBoolean( bool value )  { *( ( bool* )this->_value ) = value; }
};



//**************************************************************************
//
// Screen class
//
//**************************************************************************
class Screen {

  public:

    //----------------------------------------------------------------------
    // Class constructors
    //----------------------------------------------------------------------

    Screen( uint8_t _id );
    Screen( uint8_t _id, const struct ScreenItemBase* _items, pfcbValueChange _eventValueChange,
            pfcbEnterScreen _eventEnterScreen, pfcbExitScreen _eventExitScreen );


    //----------------------------------------------------------------------
    // Methods
    //----------------------------------------------------------------------

    void update( bool force = false );
    void selectFirstItem();
    void clearSelection();
    void processKeypadEvent( uint8_t key );
    void resetTimeout( int16_t timeout = -1 );
    bool hasScreenTimedOut();
    void exitScreen();
    void activate( bool selectFirstItem, Screen* parent = NULL );
    void processEvents();


    //----------------------------------------------------------------------
    // Properties
    //----------------------------------------------------------------------

    /* Gets the screen ID. */
    uint8_t getId()                                     { return this->_id; }

    /* Gets the screen return value. */
    uint8_t getReturnValue()                            { return this->_returnValue; }

    /* Gets/set the parent screen. */
    Screen* getParent()                                 { return this->_parent; }
    void setParent( Screen* parent )                    { this->_parent = parent; }

    /* Gets the current selected item index. */
    uint8_t getSelectedItemIndex()                      { return this->_selected; }

    /* Gets the ID of the currently selected item. */
    uint8_t getSelectedItemId()                         { return this->_item.getId(); }

    /* Gets the current cursor position within the item. */
    uint8_t getCurrentFieldPos()                        { return this->_fieldPos; }

    /* Gets/sets the screen timeout delay. */
    uint16_t getTimeout()                               { return this->_timeout; }
    void setTimeout( int16_t timeout )                  { this->resetTimeout( timeout ); }

    /* Gets the current scroll position. */
    uint8_t getScrollPos()                              { return this->_scroll; }

    /* Returns whether or not any items value has changed. */
    bool hasItemsChanged()                              { return this->_itemChanged; }

    /* Returns whether the current item is shown full screen. */
    bool isItemFullScreen()                             { return this->_itemFullscreen; }

    /* Gets/sets whether to confirm changes when exiting the screen. */
    bool getConfirmChanges()                            { return this->_confirmChanges; }
    void setConfirmChanges( bool confirm )              { this->_confirmChanges = confirm; }

    /* Sets which custom character set to use. */
    void setCustomCharacterSet( const char* cset )      { this->_customCharacterSet = cset; }

    /* Sets a callback for when a key is pressed. */
    void setCbKeypress( pfcbKeypress p )                { this->_eventKeypress = p; }

    /* Sets a callback for when a value has changed. */
    void setCbValueChange( pfcbValueChange p )          { this->_eventValueChange = p; }

    /* Sets a callback for when the screen is redrawn. */
    void setCbDrawScreen( pfcbDrawScreen p )            { this->_eventDrawScreen = p; }

    /* Sets a callback for when exiting the screen. */
    void setCbExitScreen( pfcbExitScreen p )            { this->_eventExitScreen = p; }

    /* Sets a callback for when entering the screen. */
    void setCbEnterScreen( pfcbEnterScreen p )          { this->_eventEnterScreen = p; }

    /* Sets a callback for when an item is redrawn. */
    void setCbDrawItem( pfcbDrawItem p )                { this->_eventDrawItem = p; }

    /* Sets a callback for when an item is selected or the
       cursor change position. */
    void setCbSelectionChange( pfcbSelectionChanged p ) { this->_eventSelectionChanged = p; }

    /* Sets callback for when the timeout delay has elapsed */
    void setCbTimeout( pfcbTimeout p )                  { this->_eventTimeout  = p; }




  private:

    bool _isShowConfirmDialog = false;
    bool _itemFullscreen = false;
    bool _itemChanged = false;
    bool _confirmChanges = false;
    uint8_t _selected = 0;
    uint8_t _id;
    uint8_t _returnValue = 0;
    uint8_t _fieldPos = 0;
    uint8_t _scroll = 0;
    int16_t _timeout = 0;
    bool _uppercase = false;
    Screen* _parent = NULL;
    ScreenItem _item;
    const struct ScreenItemBase* _items = NULL;
    const char* _customCharacterSet = CUSTOM_CHARACTERS_DEFAULT;

    pfcbKeypress _eventKeypress = NULL;
    pfcbValueChange _eventValueChange = NULL;
    pfcbDrawScreen _eventDrawScreen = NULL;
    pfcbExitScreen _eventExitScreen = NULL;
    pfcbEnterScreen _eventEnterScreen = NULL;
    pfcbDrawItem _eventDrawItem = NULL;
    pfcbSelectionChanged _eventSelectionChanged = NULL;
    pfcbTimeout _eventTimeout = NULL;


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

#endif /* SCREEN_H */