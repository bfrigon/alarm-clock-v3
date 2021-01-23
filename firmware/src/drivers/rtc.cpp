//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/rtc.cpp
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
#include "rtc.h"
#include "power.h"
#include "neoclock.h"
#include "wifi/wifi.h"

#include "../libs/time.h"


volatile bool rtc_event = false;



/*! ------------------------------------------------------------------------
 *
 * @brief   Class constructor
 *
 * @param   pin_irq     DS3231 interrupt pin
 * 
 */
DS3231::DS3231( int8_t pin_irq ) {
    _pin_irq = pin_irq;
    _adjustDelay = -1;
    _delayStart = 0;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Initialize the RTC IC.
 * 
 */
void DS3231::begin() {

    Wire.begin();

    uint8_t control = ( DS3231_CTRL_INTCN );
    this->write( DS3231_REG_CONTROL, control );


    uint8_t reg_hour = this->read( DS3231_REG_HOUR );

    reg_hour &= ~DS3231_HOUR_24H; /* Set 24-hour mode */
    this->write( DS3231_REG_HOUR, reg_hour );

    this->clearAlarmFlag();
    this->enableInterrupt();

    this->readTime( &_now );
    this->resetMillis();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Enable the alarm interrupt.
 * 
 */
void DS3231::enableInterrupt() {
    uint8_t control = this->read( DS3231_REG_CONTROL );

    control |= DS3231_CTRL_A1IE; /* Enable alarm 1 interrupt */
    this->write( DS3231_REG_CONTROL, control );

    pinMode( this->_pin_irq, INPUT_PULLUP );
    attachInterrupt( digitalPinToInterrupt( this->_pin_irq ), isr_ds3231, LOW );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Disable the alarm interrupt.
 * 
 */
void DS3231::disableInterrupt() {
    detachInterrupt( digitalPinToInterrupt( this->_pin_irq ) );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Clear the alarm flag.
 * 
 */
void DS3231::clearAlarmFlag() {
    uint8_t status = this->read( DS3231_REG_STATUS );

    status &= ~DS3231_STATUS_ALARM1; /* Reset alarm 1 flag */
    this->write( DS3231_REG_STATUS, status );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Set the frequency at which the alarm interrupt is raised.
 *
 * @param   freq    RTC_ALARM_EVERY_HOUR, RTC_ALARM_EVERY_MINUTE 
 *                  or RTC_ALARM_EVERY_SECOND
 * 
 */
void DS3231::setAlarmFrequency( uint8_t freq ) {

    switch( freq ) {
        case RTC_ALARM_EVERY_HOUR:
            /* A1M1=0, A1M2=0, A1M3=1, A1M4=1 */
            this->write( DS3231_REG_AL1SEC, DS3231_SECOND_MATCH | 0 );  /* Match second 0 */
            this->write( DS3231_REG_AL1MIN, DS3231_MINUTE_MATCH | 0 );  /* Match minute 0 */
            this->write( DS3231_REG_AL1HOUR, DS3231_HOUR_IGNORE );     /* Ignore hours */
            this->write( DS3231_REG_AL1WDAY, DS3231_WDAY_IGNORE );     /* Ignore week day */

            break;

        case RTC_ALARM_EVERY_SECOND:
            /* A1M1=1, A1M2=1, A1M3=1, A1M4=1 */
            this->write( DS3231_REG_AL1SEC, DS3231_SECOND_IGNORE );  /* Ignore seconds */
            this->write( DS3231_REG_AL1MIN, DS3231_MINUTE_IGNORE );  /* Ignore minutes */
            this->write( DS3231_REG_AL1HOUR, DS3231_HOUR_IGNORE );  /* Ignore hours */
            this->write( DS3231_REG_AL1WDAY, DS3231_WDAY_IGNORE );  /* Ignore week day */

            break;

        /* RTC_ALARM_EVERY_MINUTE */
        default:
            /* A1M1=0, A1M2=1, A1M3=1, A1M4=1 */
            this->write( DS3231_REG_AL1SEC, DS3231_SECOND_MATCH | 0 );  /* Match second 0 */
            this->write( DS3231_REG_AL1MIN, DS3231_MINUTE_IGNORE );    /* Ignore minutes */
            this->write( DS3231_REG_AL1HOUR, DS3231_HOUR_IGNORE );     /* Ignore hours */
            this->write( DS3231_REG_AL1WDAY, DS3231_WDAY_IGNORE );     /* Ignore week day */

            break;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Check if an alarm event occured. If so, it will reset the 
 *          alarm flag and rearm the interrupt.
 *
 * @return  TRUE if an alarm event occured or False otherwise.
 * 
 */
bool DS3231::processEvents() {

    if( _adjustDelay >= 0 && millis() - _delayStart >= _adjustDelay ) {
        
        this->writeTime( &_adjust );
        _now = _adjust;
        _adjustDelay = -1;

        /* Request clock display update */
        g_clock.requestClockUpdate();

        /* Update the time on the wifi module */
        g_wifi.setSystemTime( &_now );
    }

    if( rtc_event == false ) {
        return false;
    }

    this->clearAlarmFlag();
    this->enableInterrupt();
    rtc_event = false;

    this->readTime( &_now );
    return true;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   For debugging purposes. Prints the contents of all registers
 * 
 */
void DS3231::dumpRegs() {
    Wire.beginTransmission( I2C_ADDR_DS3231 );
    Wire.write( DS3231_REG_SEC );
    Wire.endTransmission();

    Wire.requestFrom( I2C_ADDR_DS3231, 18 );

    uint8_t i;

    for( i = 0; i < 18; i++ ) {
        Serial.print( F( "Register 0x" ) );
        Serial.print( i, HEX );
        Serial.print( F( ": " ) );
        Serial.print( Wire.read(), BIN );
        Serial.println( "" );
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the current date and time.
 *
 * @param   dt    Pointer to the DateTime structure where the date and time 
 *                will be written to.
 * 
 */
void DS3231::readTime( DateTime *dt ) {

    Wire.beginTransmission( I2C_ADDR_DS3231 );
    Wire.write( DS3231_REG_SEC );
    Wire.endTransmission();

    Wire.requestFrom( I2C_ADDR_DS3231, 8 );

    uint8_t ss = bcd2bin( Wire.read() );
    uint8_t mm = bcd2bin( Wire.read() );

    uint8_t hrreg = Wire.read();
    uint8_t hh = bcd2bin( ( hrreg & ~DS3231_HOUR_24H ) );  //Ignore 24 Hour bit

    uint8_t  wd = Wire.read();
    uint8_t  d = bcd2bin( Wire.read() );
    uint8_t  m = bcd2bin( Wire.read() );
    uint16_t y = bcd2bin( Wire.read() ) + 2000;

    _now.set( y, m, d, hh, mm, ss );

    if( dt != NULL ) {
        *dt = _now;
    }
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the current unix time. 
 * 
 * @details This function interpolate the unix time based on the date and 
 *          time stored in the RTC. It accounts for leap year but Ignores
 *          time zone. It always assume the time zone is UTC.
 *
 * @return  The unix time.
 * 
 */
unsigned long DS3231::getEpoch() {
    return this->_now.getEpoch();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Sets the date and time on the RTC IC.
 *
 * @param   ndt    Structure containing the date and time.
 * 
 */
void DS3231::writeTime( DateTime *new_dt ) {
    Wire.beginTransmission( I2C_ADDR_DS3231 );

    Wire.write( DS3231_REG_SEC );

    Wire.write( bin2bcd( new_dt->second() ) );
    Wire.write( bin2bcd( new_dt->minute() ) );
    Wire.write( bin2bcd( new_dt->hour() & ~DS3231_HOUR_24H ) );
    Wire.write( bin2bcd( new_dt->dow() ) );
    Wire.write( bin2bcd( new_dt->day() ) );
    Wire.write( bin2bcd( new_dt->month() ) );
    Wire.write( bin2bcd( new_dt->year() % 100 ) );

    Wire.endTransmission();

    _now = new_dt;
    this->resetMillis();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Read data from the RTC IC.
 *
 * @param   reg    Register address to read data from.
 * 
 */
uint8_t DS3231::read( uint8_t reg ) {

    Wire.beginTransmission( I2C_ADDR_DS3231 );

    Wire.write( reg );

    Wire.endTransmission();
    delayMicroseconds( 10 );

    Wire.requestFrom( I2C_ADDR_DS3231, 1 );
    return Wire.read();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Write data to the RTC IC.
 *
 * @param   reg      Register address to write data to.
 * @param   value    Data to write
 * 
 */
void DS3231::write( uint8_t reg, uint8_t value ) {
    Wire.beginTransmission( I2C_ADDR_DS3231 );

    Wire.write( reg );
    Wire.write( value );

    Wire.endTransmission();

    delayMicroseconds( 10 );
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Reset the milliseconds counter
 * 
 */
void DS3231::resetMillis() {
    _secStart = millis();
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Get the number of milliseconds since the beginning of the current second
 *
 * @return  Number of milliseconds elapsed
 * 
 */
uint16_t DS3231::getMillis() {
    uint16_t ret;
    ret = millis() - _secStart;

    return ( ret > 999 ) ? 999 : ret;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Register a delayed clock adjustment
 *
 * @param   ndt      DateTime structure containing the new date/time
 * @param   delay    Number of milliseconds to wait before writing the 
 *                   new date/time
 * 
 */
void DS3231::adjustClock( DateTime *ndt, int16_t delay ) {
    _delayStart = millis();
    _adjustDelay = delay;
    _adjust = ndt;
}


/*! ------------------------------------------------------------------------
 *
 * @brief   Interrupt service routine for the alarm event.
 * 
 */
void isr_ds3231() {
    rtc_event = true;

    g_rtc.resetMillis();
    g_rtc.disableInterrupt();
}