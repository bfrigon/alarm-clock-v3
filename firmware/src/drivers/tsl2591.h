//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/tsl2591.h
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
#ifndef _TSL2591_H
#define _TSL2591_H

#include <Arduino.h>
#include <Wire.h>
#include "../libs/task.h"

#define TSL2591_LUX_DF          428.0F

#define TSL2591_I2C_ADDR        0x29

#define TSL2591_REG_ENABLE      0x00
#define TSL2591_REG_CONFIG      0x01
#define TSL2591_REG_AILTL       0x04
#define TSL2591_REG_AILTH       0x05
#define TSL2591_REG_AIHTL       0x06
#define TSL2591_REG_AIHTH       0x07
#define TSL2591_REG_NPAILTL     0x08
#define TSL2591_REG_NPAILTH     0x09
#define TSL2591_REG_NPAIHTL     0x0a
#define TSL2591_REG_NPAIHTH     0x0b
#define TSL2591_REG_PERSIST     0x0c
#define TSL2591_REG_PID         0x11
#define TSL2591_REG_ID          0x12
#define TSL2591_REG_STATUS      0x13
#define TSL2591_REG_C0DATAL     0x14
#define TSL2591_REG_C0DATAH     0x15
#define TSL2591_REG_C1DATAL     0x16
#define TSL2591_REG_C1DATAH     0x17


#define TSL2591_COMMAND_SELECT      0x80
#define TSL2591_TRANSACTION_NORMAL  0x20
#define TSL2591_TRANSACTION_SPECIAL 0x60

#define TSL2591_GAIN_LOW            0x00
#define TSL2591_GAIN_MEDIUM         0x01
#define TSL2591_GAIN_HIGH           0x02
#define TSL2591_GAIN_MAXIMUM        0x03

#define TSL2591_INTEGRATION_100MS   0x00
#define TSL2591_INTEGRATION_200MS   0x01
#define TSL2591_INTEGRATION_300MS   0x02
#define TSL2591_INTEGRATION_400MS   0x03
#define TSL2591_INTEGRATION_500MS   0x04
#define TSL2591_INTEGRATION_600MS   0x05
#define TSL2591_INTEGRATION_MAX     0x05


#define TSL2591_ENABLE_PON          0x01
#define TSL2591_ENABLE_AEN          0x02
#define TSL2591_ENABLE_AIEN         0x10
#define TSL2591_ENABLE_SAI          0x40
#define TSL2591_ENABLE_NPIEN        0x80
#define TSL2591_ENABLE_OFF          0x00


class TSL2591 : public Task {

  public:
    TSL2591();

    void configure( uint8_t gain, uint8_t integration );

    void onPowerStateChange( uint8_t state );
    void suspend();
    void resume();
    void runTask();

    void begin();


  private:


    void sendCommand( uint8_t command, uint8_t value );
    uint8_t readByte( uint8_t address );
    uint16_t readWord( uint8_t address );

    bool _init = false;
    uint32_t _lastIntegrationStart;
    uint8_t _integration;
    uint8_t _gain;
    uint8_t _integrationDelay;
    float _lux;

};

extern TSL2591 g_als;


#endif  /* _TSL2591_H */