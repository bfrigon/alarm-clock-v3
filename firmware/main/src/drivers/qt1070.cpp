//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/qt1070.cpp
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
#include "qt1070.h"
#include "power.h"


volatile bool qt1070_event = false;


/*--------------------------------------------------------------------------
 *
 * Class constructor
 *
 * Arguments
 * ---------
 *  - pin_irq : QT1070 interrupt pin
 *
 * Returns : Nothing
 */
QT1070::QT1070( uint8_t pin_irq ) {


    this->_pin_irq = pin_irq;


    for( uint8_t i = 0; i < 7; i++ ) {
        this->config.nthr[i] = 0x14;
        this->config.di[i] = 0x04;
        this->config.aks[i] = {
            0x00,       /* Adjacent key supression group 1 */
            0x00        /* Averaging factor */
        };
    }

    this->config.guardChannel = 0;
    this->config.maxCal = false;
    this->config.fastOut = false;
    this->config.maxOn = 250;
    this->config.lowPower = 2;



    /* Initialize Keypad touch IC */
    this->config.nthr[3] = 30;   /* Menu */
    this->config.aks[3] = { 0x00, 0x08 };
    this->config.di[3] = 0x02;

    this->config.nthr[1] = 30;   /* Alarm */
    this->config.aks[1] = { 0x00, 0x08 };
    this->config.di[1] = 0x02;

    this->config.nthr[2] = 45;   /* Set */
    this->config.aks[2] = { 0x00, 0x08 };
    this->config.di[2] = 0x02;
}


/*--------------------------------------------------------------------------
 *
 * Initialize the touch IC.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void QT1070::begin() {

    this->_init = true;


    /* Write default config */
    this->writeConfig();

    /* Clear pending interrupt */
    this->readStatus();


    this->enableInterrupt();
}


/*--------------------------------------------------------------------------
 *
 * Enable key change interrupt.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void QT1070::enableInterrupt() {
pinMode( this->_pin_irq, INPUT_PULLUP );
    attachInterrupt( digitalPinToInterrupt( this->_pin_irq ), isr_qt1070, LOW );
}


/*--------------------------------------------------------------------------
 *
 * Disable key change interrupt.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Nothing
 */
void QT1070::disableInterrupt() {

    detachInterrupt( digitalPinToInterrupt( this->_pin_irq ) );
}


/*--------------------------------------------------------------------------
 * Reads the two status bytes from the touch IC.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : True if the read was successful.
 */
bool QT1070::readStatus() {

    if( this->_init == false ) {
        this->begin();
    }

    return ( this->read( QT1070_REG_STATUSBLOCK, ( void * )&this->status, sizeof( statusBlock ) ) != 0 );
}


/*--------------------------------------------------------------------------
 * Write configuration block to the touch IC.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : True if the write was successful.
 */
bool QT1070::writeConfig() {

    if( this->_init == false ) {
        this->begin();
    }

    return ( this->write( QT1070_REG_CONFIGBLOCK, ( void * )&this->config, sizeof( configBlock ) ) == 0 );
}


/*--------------------------------------------------------------------------
 *
 * Process the touche events to detect which key was pressed or held or if
 * a swipe left or swipe right occured
 *
 * Arguments
 * ---------
 * - None
 *
 * Returns : The pressed key ID, false otherwize.
 */
uint8_t QT1070::processEvents() {

    uint8_t key = 0;
    int16_t strongestSignal;
    unsigned long lastEventDelay;

    /* Calculate the elapsed time since the last event */
    lastEventDelay = this->lastEventStart > 0 ? millis() - this->lastEventStart : 0;

    if( qt1070_event == true ) {

        qt1070_event = false;

        /* Read the status bytes (2-3) to reset the interrupt */
        if( this->readStatus() == false ) {
            return KEY_NONE;
        }

        /* Ignore event if the touch IC is calibrating */
        if( this->status.calibrating == true ) {
            return KEY_NONE;
        }

        this->enableInterrupt();

        if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {

            g_power.setPowerMode( POWER_MODE_LOW_POWER );

            this->firstKeyState = 0;
            this->lastKeyState = 0;
            this->repeatCount = 0;

            return KEY_NONE;

        } else if( g_power.getPowerMode() == POWER_MODE_LOW_POWER ) {
            g_power.resetSuspendDelay();
        }


        uint8_t i;

        for( i = 1; i < 7; i++ ) {

            if( this->status.keys & ( 1 << i ) ) {

                /* Read the signal level and reference level of the current detected key */
                int16_t keySignal;
                int16_t keyRef;
                this->read( QT1070_REG_KEYSIGNAL + ( i * 2 ), &keySignal, sizeof( keySignal ) );
                this->read( QT1070_REG_KEYREF + ( i * 2 ), &keyRef, sizeof( keyRef ) );

                if( ( keySignal - keyRef ) > strongestSignal ) {
                    key = ( 1 << ( i - 1 ) );
                    strongestSignal = ( keySignal - keyRef );
                }
            }
        }

        this->lastEventStart = millis();

    } else {

        key = this->lastKeyState;
    }

    if( ( this->firstKeyState != 0 ? this->firstKeyState : key ) & this->repeatMask ) {
        return this->processKeyRepeatMode( key, lastEventDelay );
    }

    return this->processKeyStandardMode( key, lastEventDelay );
}


/*--------------------------------------------------------------------------
 *
 * Process key event using the standard mode meaning that holding the key down
 * will be considered a SHIFT+KEY press.
 *
 * Arguments
 * ---------
 * - key            : Detected key
 * - lastEventDelay : Delay in ms since the last event.
 *
 * Returns : The pressed key ID or KEY_NONE otherwise.
 */
uint8_t QT1070::processKeyStandardMode( uint8_t key, uint16_t lastEventDelay ) {

    if( key != 0 && key == this->firstKeyState && lastEventDelay > this->longKeyDelay ) {

        key |= KEY_SHIFT;

        /* Reset the states */
        this->firstKeyState = 0;
        this->lastKeyState = 0;
        this->repeatCount = 0;

        return key;
    }


    /* Key up */
    if( key == 0 && this->lastKeyState != 0 ) {

        if( this->firstKeyState == KEY_LEFT && this->lastKeyState == KEY_RIGHT ) {
            key = KEY_RIGHT | KEY_SWIPE;

        } else if( this->firstKeyState == KEY_RIGHT && this->lastKeyState == KEY_LEFT ) {
            key = KEY_LEFT | KEY_SWIPE;

        } else {
            key = this->firstKeyState;
        }

        /* Reset the states */
        this->firstKeyState = 0;
        this->lastKeyState = 0;
        this->repeatCount = 0;

        return key;
    }

    /* Key down */
    if( key != 0 ) {

        this->lastKeyState = key;

        if( this->firstKeyState == 0 ) {
            this->firstKeyState = key;
        }

        return KEY_NONE;
    }




    return KEY_NONE;
}


/*--------------------------------------------------------------------------
 *
 * Process key event using the repeat mode.
 *
 * Arguments
 * ---------
 * - key            : Detected key
 * - lastEventDelay : Delay in ms since the last event.
 *
 * Returns : The pressed key ID or KEY_NONE otherwise.
 */
uint8_t QT1070::processKeyRepeatMode( uint8_t key, uint16_t lastEventDelay ) {

    /* Key down */
    if( key != 0 ) {

        this->lastKeyState = key;

        if( this->firstKeyState == 0 ) {
            this->repeatCount = 1;
            this->firstKeyState = key;

            return key;
        }
    }

    /* Key down, moved to a different key */
    if( key != 0 && key != this->firstKeyState ) {

        /* Ignore the key until the user move back to the current key or release the key */
        return KEY_NONE;
    }

    /* Key up */
    if( key == 0 ) {

        /* Reset the states, but return nothing */
        this->firstKeyState = 0;
        this->lastKeyState = 0;
        this->repeatCount = 0;

        return KEY_NONE;
    }



    /* Key down, first repeat : Check if the minimum repeat delay elapsed */
    if( this->repeatCount == 1 && lastEventDelay < this->repeatDelay ) {
        return KEY_NONE;
    }

    /* Key down, subsequent repeats : Check if the rate delay elapsed */
    if( this->repeatCount > 1 && lastEventDelay < this->repeatRate ) {
        return KEY_NONE;
    }

    this->lastEventStart = millis();
    this->repeatCount++;

    return key;
}


/*--------------------------------------------------------------------------
 *
 * Transmit data to the touch IC.
 *
 * Arguments
 * ---------
 *  - reg  : Register address to start writing to.
 *  - data : Pointer to the data to be written.
 *  - size : Size of the data.
 *
 * Returns : Status of the transmission
 *   0: Success
 *   1: Data too long to fit in transmit buffer
 *   2: Received NACK on transmit of address
 *   3: Received NACK on transmit of data
 *   4: Other error
 */
uint8_t QT1070::write( uint8_t reg, void *data, uint8_t size ) {
    uint8_t res;

    Wire.beginTransmission( I2C_ADDR_AT42QT1070 );
    Wire.write( reg );
    Wire.write( ( char * )data, size );
    res = Wire.endTransmission( true );

    delay( 1 );

    return res;
}


/*--------------------------------------------------------------------------
 *
 * Read data from the touch IC.
 *
 * Arguments
 * ---------
 *  - reg  : Register address to start reading from.
 *  - data : Pointer to the buffer.
 *  - size : Size of the data.
 *
 * Returns : The number of bytes read.
 */
uint8_t QT1070::read( uint8_t reg, void *data, uint8_t size ) {
    uint8_t length = 0;

    Wire.beginTransmission( I2C_ADDR_AT42QT1070 );
    Wire.write( reg );
    Wire.endTransmission( true );


    Wire.requestFrom( I2C_ADDR_AT42QT1070, ( int )size, true );

    while( Wire.available() ) {
        *( ( char * )data + length ) = Wire.read();
        length++;
    }

    return length;
}


/*--------------------------------------------------------------------------
 *
 * Interrupt service routine for the key change event.
 *
 * None
 *
 * Returns : Nothing
 */
void isr_qt1070() {
    qt1070_event = true;

    g_keypad.disableInterrupt();
}