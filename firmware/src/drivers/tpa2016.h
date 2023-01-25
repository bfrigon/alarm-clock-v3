//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/tpa2016.h
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
#ifndef _TPA2016_H
#define _TPA2016_H

#include <Arduino.h>
#include <Wire.h>



#define TPA2016_I2C_ADDR              0x58

#define TPA2016_REG_CONTROL           0x01
#define TPA2016_REG_AGC_ATTACK        0x02
#define TPA2016_REG_AGC_RELEASE       0x03
#define TPA2016_REG_AGC_HOLD          0x04
#define TPA2016_REG_FIXED_GAIN        0x05
#define TPA2016_REG_AGC_CTRL1         0x06
#define TPA2016_REG_AGC_CTRL2         0x07


#define TPA2016_CTRL_RSPK_ON          0x80
#define TPA2016_CTRL_RSPK_OFF         0x00

#define TPA2016_CTRL_LSPK_ON          0x40
#define TPA2016_CTRL_LSPK_OFF         0x00

#define TPA2016_CTRL_SWS              0x20
#define TPA2016_CTRL_FAULT_R          0x10
#define TPA2016_CTRL_FAULT_L          0x08
#define TPA2016_CTRL_THERMAL          0x04

#define TPA2016_AGC_LIMITER_DISABLED  0x80
#define TPA2016_AGC_LIMITER_ENABLED   0x00

#define TPA2016_CTRL_NG_ON            0x01
#define TPA2016_CTRL_NG_OFF           0x00

#define TPA2016_AGC_NG_THRESHOLD_1    0x00
#define TPA2016_AGC_NG_THRESHOLD_4    0x01
#define TPA2016_AGC_NG_THRESHOLD_10   0x02
#define TPA2016_AGC_NG_THRESHOLD_20   0x03

#define TPA2016_COMPRESSION_OFF       0x00
#define TPA2016_COMPRESSION_2_1       0x01
#define TPA2016_COMPRESSION_4_1       0x02
#define TPA2016_COMPRESSION_8_1       0x03



/*******************************************************************************
 *
 * @brief   Audio amplifier (TPA2016) driver class
 * 
 *******************************************************************************/
class TPA2016 {

  public:
    TPA2016();
    void begin();
    void end();
    void setPins( int8_t pin_shutdown );
    bool enableOutputs();
    bool disableOutputs();
    bool setFixedGain( int8_t db );
    bool setAttackTime( int8_t time );
    bool setReleaseTime( int8_t time );
    bool setHoldTime( int8_t time );
    bool setMaxGain( int8_t db );
    bool setLimiter( bool disabled, int8_t level );
    bool setCompression( uint8_t compression );
    void dumpRegs();


  private:
    bool write( uint8_t reg, uint8_t data );

    bool _init = false;
    int8_t _pin_shutdown = -1;
    uint8_t _control;
    int8_t _attackTime;
    int8_t _releaseTime;
    int8_t _holdTime;
    int8_t _fixedGain;
    bool _outputLimiterDisabled;
    int8_t _noiseGateThreshold;
    int8_t _outputLimiterLevel;
    int8_t _maxGain;
    uint8_t _compression;
};

#endif /* _TPA2016_H */