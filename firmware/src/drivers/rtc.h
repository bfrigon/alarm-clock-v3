//******************************************************************************
//
// Project : Alarm Clock V3
// File    : rtc.h
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
#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <Wire.h>
#include <time.h>


/* --- I2C address --- */
#define I2C_ADDR_DS3231         0x68

/* --- DS3231 registers --- */
#define DS3231_REG_SEC          0x00
#define DS3231_REG_MIN          0x01
#define DS3231_REG_HOUR         0x02
#define DS3231_REG_WDAY         0x03
#define DS3231_REG_MDAY         0x04
#define DS3231_REG_MONTH        0x05
#define DS3231_REG_YEAR         0x06
#define DS3231_REG_AL1SEC       0x07
#define DS3231_REG_AL1MIN       0x08
#define DS3231_REG_AL1HOUR      0x09
#define DS3231_REG_AL1WDAY      0x0A
#define DS3231_REG_AL2MIN       0x0B
#define DS3231_REG_AL2HOUR      0x0C
#define DS3231_REG_AL2WDAY      0x0D
#define DS3231_REG_CONTROL      0x0E
#define DS3231_REG_STATUS       0x0F
#define DS3231_REG_AGING_OFFSET 0x10
#define DS3231_REG_TMP_UP       0x11
#define DS3231_REG_TMP_LOW      0x12


#define DS3231_CTRL_A1IE        ( 1 << 0 )
#define DS3231_CTRL_A2IE        ( 1 << 1 )
#define DS3231_CTRL_INTCN       ( 1 << 2 )
#define DS3231_CTRL_CONV        ( 1 << 5 )
#define DS3231_CTRL_BBSQW       ( 1 << 6 )
#define DS3231_CTRL_EOSC        ( 1 << 7 )


#define DS3231_STATUS_ALARM1    ( 1 << 0 );
#define DS3231_STATUS_ALARM2    ( 1 << 1 );
#define DS3231_STATUS_BUSY      ( 1 << 2 );
#define DS3231_STATUS_32KHZ     ( 1 << 3 );
#define DS3231_STATUS_OSF       ( 1 << 7 );

#define DS3231_HOUR_24H         ( 1 << 6 )

#define DS3231_SECOND_IGNORE    ( 1 << 7 )
#define DS3231_SECOND_MATCH     0
#define DS3231_MINUTE_IGNORE    ( 1 << 7 )
#define DS3231_MINUTE_MATCH     0
#define DS3231_HOUR_IGNORE      ( 1 << 7 )
#define DS3231_HOUR_MATCH       0
#define DS3231_WDAY_IGNORE      ( 1 << 7 )
#define DS3231_WDAY_MATCH       0




#define RTC_ALARM_EVERY_MINUTE  0
#define RTC_ALARM_EVERY_SECOND  1
#define RTC_ALARM_EVERY_HOUR    2


class DS3231 {

  public:

    DS3231( int8_t pin_irq );
    void begin();
    void setAlarmFrequency( uint8_t freq );
    void enableInterrupt();
    void disableInterrupt();
    void clearAlarmFlag();
    bool processEvents();
    void readTime( DateTime *dt = NULL );
    unsigned long getEpoch();
    void writeTime( DateTime *ndt );
    void dumpRegs();
    uint16_t getMillis();
    void resetMillis();

    void adjustClock( DateTime *ndt, int16_t delay );

    DateTime* now()     { return &_now; }

  private:
    uint8_t read( uint8_t reg );
    void write( uint8_t reg, uint8_t value );

    bool _init = false;
    int8_t _pin_irq;

    unsigned long _secStart;
    unsigned long _delayStart;
    DateTime _now;
    DateTime _adjust;
    int16_t _adjustDelay;
};

void isr_ds3231();

extern DS3231 g_rtc;

#endif /* RTC_H */