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


/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor for ScreenItem class.
 *
 */
ScreenItem::ScreenItem() {
    this->unload();  
}


/*! ------------------------------------------------------------------------
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


/*! ------------------------------------------------------------------------
 *
 * @brief   Unload the screen item
 * 
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