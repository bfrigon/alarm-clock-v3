//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/ui/screen_item.cpp
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

#include "screen_item.h"



/*******************************************************************************
 *
 * @brief   Class constructor for ScreenItem class.
 *
 */
ScreenItem::ScreenItem() {
    this->unload();  
}


/*******************************************************************************
 *
 * @brief   Load a screen item from program memory.
 *
 * @param   item    Pointer to the screen item structure contained in 
 *                  program memory
 * 
 */
void ScreenItem::loadFromProgmem( const struct ScreenItemBase* item ) {

    struct ScreenItemBase memItem;
    memcpy_P( &memItem, item, sizeof( ScreenItemBase ) );

    _type = memItem._type;
    _id = memItem._id;
    _row = memItem._row;
    _col = memItem._col;
    _caption = memItem._caption;
    _value = memItem._value;
    _min = memItem._min;
    _max = memItem._max;
    _length = memItem._length;
    _options = memItem._options;
    _list = memItem._list;
}


/*******************************************************************************
 *
 * @brief   Unload the screen item
 * 
 */
void ScreenItem::unload() {

    _type = ITEM_TYPE_NULL;
    _id = 0;
    _row = 0;
    _col = 0;
    _caption = NULL;
    _value = NULL;
    _min = 0;
    _max = 0;
    _length = 0;
    _options = 0;
    _list = NULL;
}