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
#include "../libs/time.h"


volatile bool rtc_event = false;



/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - pin_irq : DS3231 interrupt pin
 */
DS3231::DS3231( int8_t pin_irq ) {
    this->_pin_irq = pin_irq;
}


/*--------------------------------------------------------------------------
 *
 * Initialize the RTC IC.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
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

    this->getTime( &_now );
}


/*--------------------------------------------------------------------------
 *
 * Enable the alarm interrupt.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void DS3231::enableInterrupt() {
    uint8_t control = this->read( DS3231_REG_CONTROL );

    control |= DS3231_CTRL_A1IE; /* Enable alarm 1 interrupt */
    this->write( DS3231_REG_CONTROL, control );

    pinMode( this->_pin_irq, INPUT_PULLUP );
    attachInterrupt( digitalPinToInterrupt( this->_pin_irq ), isr_ds3231, LOW );
}


/*--------------------------------------------------------------------------
 *
 * Disable the alarm interrupt.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void DS3231::disableInterrupt() {
    detachInterrupt( digitalPinToInterrupt( this->_pin_irq ) );
}


/*--------------------------------------------------------------------------
 *
 * Clear the alarm flag.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void DS3231::clearAlarmFlag() {
    uint8_t status = this->read( DS3231_REG_STATUS );

    status &= ~DS3231_STATUS_ALARM1; /* Reset alarm 1 flag */
    this->write( DS3231_REG_STATUS, status );
}


/*--------------------------------------------------------------------------
 *
 * Set the frequency at which the alarm interrupt is raised.
 *
 * Arguments
 * ---------
 *  - freq : RTC_ALARM_EVERY_HOUR, RTC_ALARM_EVERY_MINUTE or RTC_ALARM_EVERY_SECOND
 *
 * Returns : Nothing
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


/*--------------------------------------------------------------------------
 *
 * Check if an alarm event occured. If so, it will reset the alarm flag and
 * rearm the interrupt.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if an alarm event occuredor False otherwise.
 */
bool DS3231::processEvents() {

    if( rtc_event == false ) {
        return false;
    }

    this->clearAlarmFlag();
    this->enableInterrupt();
    rtc_event = false;

    /* Read the current time */
    this->getTime( &_now );


    return true;
}


/*--------------------------------------------------------------------------
 *
 * For debugging purposes. Prints the contents of all registers
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
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


/*--------------------------------------------------------------------------
 *
 * Get the current date and time.
 *
 * Arguments
 * ---------
 *  - dt : Pointer to the DateTime structure where the date and time will
 *         be stored.
 *
 * Returns : Nothing
 */
void DS3231::getTime( DateTime *dt ) {

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

    dt->set( y, m, d, hh, mm, ss );
}


/*--------------------------------------------------------------------------
 *
 * Get the current unix time. This function interpolate the unix time based
 * on the date and time stored in the RTC. It accounts for leap year but Ignores
 * time zone. It always assume the time zone is UTC.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : The unix time.
 */
unsigned long DS3231::getEpoch() {
    return this->_now.getEpoch();
}


/*--------------------------------------------------------------------------
 *
 * Sets the date and time on the RTC IC.
 *
 * Arguments
 * ---------
 *  - ndt : Structure containing the date and time.
 *
 * Returns : Nothing
 */
void DS3231::setDateTime( DateTime *ndt ) {
    Wire.beginTransmission( I2C_ADDR_DS3231 );

    Wire.write( DS3231_REG_SEC );

    Wire.write( bin2bcd( ndt->second() ) );
    Wire.write( bin2bcd( ndt->minute() ) );
    Wire.write( bin2bcd( ndt->hour() & ~DS3231_HOUR_24H ) );
    Wire.write( bin2bcd( ndt->dow() + 1 ) );
    Wire.write( bin2bcd( ndt->day() ) );
    Wire.write( bin2bcd( ndt->month() ) );
    Wire.write( bin2bcd( ndt->year() % 100 ) );

    Wire.endTransmission();
}


/*--------------------------------------------------------------------------
 *
 * Read data from the RTC IC.
 *
 * Arguments
 * ---------
 *  - reg : Register address to read data from.
 *
 * Returns : Nothing
 */
uint8_t DS3231::read( uint8_t reg ) {

    Wire.beginTransmission( I2C_ADDR_DS3231 );

    Wire.write( reg );

    Wire.endTransmission();
    delayMicroseconds( 10 );

    Wire.requestFrom( I2C_ADDR_DS3231, 1 );
    return Wire.read();
}


/*--------------------------------------------------------------------------
 *
 * Write data to the RTC IC.
 *
 * Arguments
 * ---------
 *  - reg   : Register address to write data to.
 *  - value : Data to write
 *
 * Returns : Nothing
 */
void DS3231::write( uint8_t reg, uint8_t value ) {
    Wire.beginTransmission( I2C_ADDR_DS3231 );

    Wire.write( reg );
    Wire.write( value );

    Wire.endTransmission();

    delayMicroseconds( 10 );
}


/*--------------------------------------------------------------------------
 *
 * Interrupt service routine for the alarm event.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void isr_ds3231() {
    rtc_event = true;

    g_rtc.disableInterrupt();
}