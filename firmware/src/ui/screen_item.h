//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/scree_item.h
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
#ifndef SCREEN_ITEM_H
#define SCREEN_ITEM_H

#include <Arduino.h>



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
enum {
    ITEM_TYPE_NULL,
    ITEM_TYPE_LINK,
    ITEM_TYPE_NUMBER,
    ITEM_TYPE_IP,
    ITEM_TYPE_TIME,
    ITEM_TYPE_MONTH,
    ITEM_TYPE_YEAR,
    ITEM_TYPE_TOGGLE,
    ITEM_TYPE_STATIC,
    ITEM_TYPE_LIST,
    ITEM_TYPE_TEXT,
    ITEM_TYPE_BAR,
    ITEM_TYPE_DOW,
};

/* Return values */
enum {
    RETURN_NONE,
    RETURN_NO,
    RETURN_YES,
};


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
    { ITEM_TYPE_LINK, id, row, col, caption, (void*) screen, 0, 0, 0, options, NULL }

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

    ScreenItem();
    void loadFromProgmem( const struct ScreenItemBase* item );
    void unload();


    /* Gets the item type. */
    uint8_t getType()                   { return _type; }

    /* Gets the item ID. */
    uint8_t getId()                     { return _id; }

    /* Gets the zero-based Y position on the LCD. */
    uint8_t getPositionRow()            { return _row; }

    /* Gets the zero-based X position on the LCD. */
    uint8_t getPositionCol()            { return _col; }

    /* Gets a pointer to the caption. */
    const void* getCaption()            { return _caption; }

    /* Gets the item minimum value. */
    uint8_t getMin()                    { return _min; }

    /* Gets the item maximum value. */
    uint8_t getMax()                    { return _max; }

    /* Gets the item options */
    uint8_t getOptions()                { return _options; }

    /* Gets the item field length */
    uint8_t getLength()                 { return _length; }

    /* Gets a pointer to the list items */
    const void* getListPtr()            { return _list; }

    /* Gets a pointer to the value */
    void* getValuePtr()                 { return _value; }

    /* Gets the value (unsigned integer)  */
    uint8_t getValue()                  { return *( ( uint8_t* )_value ); }

    /* Gets the value (boolean)  */
    bool getValueBoolean()              { return *( ( bool* )_value ); }

    /* Sets the value (unsigned integer )  */
    void setValue( uint8_t value )      { *( ( uint8_t* )_value ) = value; }

    /* Sets the value (boolean)  */
    void setValueBoolean( bool value )  { *( ( bool* )_value ) = value; }
};


#endif  /* SCREEN_ITEM_H */