//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/qt1070.h
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

#ifndef QT1070_H
#define QT1070_H

#include <Arduino.h>
#include <Wire.h>



/* --- I2C address --- */
#define I2C_ADDR_AT42QT1070     0x1B

/* --- AT42QT1070 registers --- */
#define QT1070_REG_CHIPID       0x00
#define QT1070_REG_FIRMWARE     0x01
#define QT1070_REG_DETECTION    0x02
#define QT1070_REG_KEYSTATUS    0x03
#define QT1070_REG_KEYSIGNAL    0x04
#define QT1070_REG_KEYREF       0x12

#define QT1070_REG_CALIBRATE    0x38
#define QT1070_REG_RESET        0x39

#define QT1070_REG_CONFIGBLOCK  0x20
#define QT1070_REG_STATUSBLOCK  0x02


#define KEY_NONE                0x00
#define KEY_LEFT                0x02
#define KEY_RIGHT               0x04
#define KEY_CENTER              0x08

#define KEY_SHIFT               0x80
#define KEY_MENU                KEY_LEFT
#define KEY_ALARM               KEY_RIGHT
#define KEY_SET                 KEY_CENTER
#define KEY_EXIT                KEY_LEFT
#define KEY_NEXT                KEY_RIGHT


/* Status block */
typedef struct {
    /* Address 2 */
    bool touch: 1;          /* Key detected flag */
    uint8_t unused: 5;      /* (Bit 1-5 unused) */
    bool overflow: 1;       /* Time to acquire all key signals exceeded 8ms */
    bool calibrating: 1;    /* Calibration sequence */

    /* Address 3 */
    uint8_t keys;           /* Key status (bit 0-6, bit 7 is not used) */
} statusBlock;

struct configBlock {

    /* Address 32-38 */
    uint8_t nthr[7];

    /* Address 39-45 */
    struct {
        uint8_t group: 2;
        uint8_t factor: 6;
    } aks[7];

    /* Address 46-52 */
    uint8_t di[7];

    /* Address 53 */
    uint8_t guardChannel: 4;
    bool maxCal: 1;
    bool fastOut: 1;
    uint8_t unused1: 2;

    /* Address 54 */
    uint8_t lowPower;

    /* Address 55 */
    uint8_t maxOn;
};



/*******************************************************************************
 *
 * @brief   Capacitive touch controller class
 * 
 *******************************************************************************/
class QT1070 {

  public:
    QT1070( uint8_t pin_irq );
    void begin();
    void enableInterrupt();
    void disableInterrupt();
    bool readStatus();
    bool writeConfig();
    uint8_t processEvents();

    statusBlock status;
    configBlock config;
    uint16_t longKeyDelay = 1200;
    uint16_t repeatDelay = 750;
    uint16_t repeatRate = 125;
    uint16_t repeatCount = 0;
    uint8_t repeatMask = 0;


  private:
    uint8_t write( uint8_t reg, void *data, uint8_t size );
    uint8_t read( uint8_t reg, void *data, uint8_t size );
    uint8_t processKeyStandardMode( uint8_t key, uint16_t lastEventDelay );
    uint8_t processKeyRepeatMode( uint8_t key, uint16_t lastEventDelay );

    bool _init = false;
    uint8_t _pin_irq;
    uint8_t firstKeyState = 0;
    uint8_t lastKeyState = 0;
    unsigned long lastEventStart = 0;
};

void isr_qt1070();


/* Keypad driver */
extern QT1070 g_keypad;

#endif /* QT1070_H */