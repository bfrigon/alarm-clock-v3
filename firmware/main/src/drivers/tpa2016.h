#ifndef _TPA2016_H
#define _TPA2016_H

#include <Arduino.h>
#include <Wire.h>



#define TPA2016_I2C_ADDR        0x58

#define TPA2016_REG_CONTROL     0x01
#define TPA2016_REG_AGC_ATTACK  0x02
#define TPA2016_REG_AGC_RELEASE 0x03
#define TPA2016_REG_AGC_HOLD    0x04
#define TPA2016_REG_FIXED_GAIN  0x05
#define TPA2016_REG_AGC_CTRL1   0x06
#define TPA2016_REG_AGC_CTRL2   0x07




#define TPA2016_CTRL_RSPK       0x80
#define TPA2016_CTRL_LSPK       0x40
#define TPA2016_CTRL_SWS        0x20
#define TPA2016_CTRL_FAULT_R    0x10
#define TPA2016_CTRL_FAULT_L    0x08
#define TPA2016_CTRL_THERMAL    0x04
#define TPA2016_CTRL_NG         0x01




class TPA2016 {

  public:

    TPA2016();

    void begin();
    void end();

    void setPins( int8_t pin_shutdown );

    void enableOutputs();
    void disableOutputs();

    void setFixedGain( int8_t db );
    void setAttackTime( int8_t time );
    void setReleaseTime( int8_t time );
    void setHoldTime( int8_t time );
    void enableAGC( bool enabled );
    void setMaxGain( int8_t db );
    void setCompression( uint8_t compression );

    void dumpRegs();

  private:

    void write( uint8_t reg, uint8_t data );

    bool _init = false;
    int8_t _pin_shutdown = -1;
    uint8_t _control;

    uint8_t _compression;


};

#endif /* _TPA2016_H */