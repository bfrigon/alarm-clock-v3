//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/bq27441.h
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
#ifndef BQ27441_H
#define BQ27441_H

#include <Arduino.h>
#include <Wire.h>


#define BQ27441_UNSEAL_KEY	            0x8000
#define BQ27441_DEVICE_ID	              0x0421

/* --- I2C address --- */
#define I2C_ADDR_BQ27441                0x55

/* -- commands -- */
#define BQ27441_COMMAND_CONTROL			    0x00
#define BQ27441_COMMAND_TEMP			      0x02
#define BQ27441_COMMAND_VOLTAGE			    0x04
#define BQ27441_COMMAND_FLAGS			      0x06
#define BQ27441_COMMAND_NOM_CAPACITY	  0x08
#define BQ27441_COMMAND_AVAIL_CAPACITY	0x0A
#define BQ27441_COMMAND_REM_CAPACITY	  0x0C
#define BQ27441_COMMAND_FULL_CAPACITY	  0x0E
#define BQ27441_COMMAND_AVG_CURRENT		  0x10
#define BQ27441_COMMAND_STDBY_CURRENT	  0x12
#define BQ27441_COMMAND_MAX_CURRENT		  0x14
#define BQ27441_COMMAND_AVG_POWER		    0x18
#define BQ27441_COMMAND_SOC				      0x1C
#define BQ27441_COMMAND_INT_TEMP		    0x1E
#define BQ27441_COMMAND_SOH				      0x20
#define BQ27441_COMMAND_REM_CAP_UNFL	  0x28
#define BQ27441_COMMAND_REM_CAP_FIL		  0x2A
#define BQ27441_COMMAND_FULL_CAP_UNFL	  0x2C
#define BQ27441_COMMAND_FULL_CAP_FIL	  0x2E
#define BQ27441_COMMAND_SOC_UNFL		    0x30

/* -- control subcommands -- */
#define BQ27441_CONTROL_STATUS			    0x00
#define BQ27441_CONTROL_DEVICE_TYPE		  0x01
#define BQ27441_CONTROL_FW_VERSION		  0x02
#define BQ27441_CONTROL_DM_CODE			    0x04
#define BQ27441_CONTROL_PREV_MACWRITE	  0x07
#define BQ27441_CONTROL_CHEM_ID			    0x08
#define BQ27441_CONTROL_BAT_INSERT	  	0x0C
#define BQ27441_CONTROL_BAT_REMOVE		  0x0D
#define BQ27441_CONTROL_SET_HIBERNATE	  0x11
#define BQ27441_CONTROL_CLEAR_HIBERNATE	0x12
#define BQ27441_CONTROL_SET_CFGUPDATE 	0x13
#define BQ27441_CONTROL_SHUTDOWN_ENABLE	0x1B
#define BQ27441_CONTROL_SHUTDOWN		    0x1C
#define BQ27441_CONTROL_SEALED			    0x20
#define BQ27441_CONTROL_PULSE_SOC_INT	  0x23
#define BQ27441_CONTROL_RESET			      0x41
#define BQ27441_CONTROL_SOFT_RESET		  0x42
#define BQ27441_CONTROL_EXIT_CFGUPDATE	0x43
#define BQ27441_CONTROL_EXIT_RESIM		  0x44

/* -- Status bits -- */
#define BQ27441_STATUS_SHUTDOWNEN	      (1<<15)
#define BQ27441_STATUS_WDRESET		      (1<<14)
#define BQ27441_STATUS_SS			          (1<<13)
#define BQ27441_STATUS_CALMODE		      (1<<12)
#define BQ27441_STATUS_CCA			        (1<<11)
#define BQ27441_STATUS_BCA			        (1<<10)
#define BQ27441_STATUS_QMAX_UP		      (1<<9)
#define BQ27441_STATUS_RES_UP		        (1<<8)
#define BQ27441_STATUS_INITCOMP		      (1<<7)
#define BQ27441_STATUS_HIBERNATE	      (1<<6)
#define BQ27441_STATUS_SLEEP		        (1<<4)
#define BQ27441_STATUS_LDMD			        (1<<3)
#define BQ27441_STATUS_RUP_DIS		      (1<<2)
#define BQ27441_STATUS_VOK			        (1<<1)

/* -- Flags -- */
#define BQ27441_FLAG_OT			            (1<<15)
#define BQ27441_FLAG_UT			            (1<<14)
#define BQ27441_FLAG_FC			            (1<<9)
#define BQ27441_FLAG_CHG		            (1<<8)
#define BQ27441_FLAG_OCVTAKEN	          (1<<7)
#define BQ27441_FLAG_ITPOR		          (1<<5)
#define BQ27441_FLAG_CFGUPMODE	        (1<<4)
#define BQ27441_FLAG_BAT_DET	          (1<<3)
#define BQ27441_FLAG_SOC1		            (1<<2)
#define BQ27441_FLAG_SOCF		            (1<<1)
#define BQ27441_FLAG_DSG		            (1<<0)

/* -- Extended commands -- */
#define BQ27441_EXTENDED_OPCONFIG	      0x3A
#define BQ27441_EXTENDED_CAPACITY	      0x3C
#define BQ27441_EXTENDED_DATACLASS	    0x3E
#define BQ27441_EXTENDED_DATABLOCK	    0x3F
#define BQ27441_EXTENDED_BLOCKDATA	    0x40
#define BQ27441_EXTENDED_CHECKSUM	      0x60
#define BQ27441_EXTENDED_CONTROL	      0x61

/* -- Data class ID's -- */
#define BQ27441_ID_SAFETY			          2
#define BQ27441_ID_CHG_TERMINATION	    36
#define BQ27441_ID_CONFIG_DATA		      48
#define BQ27441_ID_DISCHARGE		        49
#define BQ27441_ID_REGISTERS		        64
#define BQ27441_ID_POWER			          68
#define BQ27441_ID_IT_CFG			          80
#define BQ27441_ID_CURRENT_THRESH	      81
#define BQ27441_ID_STATE			          82
#define BQ27441_ID_R_A_RAM			        89
#define BQ27441_ID_CALIB_DATA		        104
#define BQ27441_ID_CC_CAL			          105
#define BQ27441_ID_CURRENT			        107
#define BQ27441_ID_CODES			          112

/* -- Battery states -- */
#define BATTERY_STATE_READY             0
#define BATTERY_STATE_NOT_PRESENT       1
#define BATTERY_STATE_CHARGING          2
#define BATTERY_STATE_DISCHARGE_FULL    3
#define BATTERY_STATE_DISCHARGE_HALF    4
#define BATTERY_STATE_DISCHARGE_LOW     5



/*******************************************************************************
 *
 * @brief   Battery monitor (BQ27441) driver class
 * 
 *******************************************************************************/
class BQ27441 {

  public:
    BQ27441();
    bool begin( uint16_t capacity );
    bool isReady();
    bool isBatteryPresent();
    bool isCharging();
    bool isDischarging();
    uint8_t getBatteryState();
    int16_t getAvgPower();
    int16_t getAvgCurrent();
    int8_t getStateOfHealth();
    uint8_t getStateOfCharge( bool filtered = true );
    uint16_t getRemainingCapacity( bool filtered = true );
    uint16_t getFullCapacity( bool filtered = true );
    uint16_t getVoltage();


  private:
    uint8_t read( uint8_t reg, void* data, uint8_t size );
    uint8_t write( uint8_t reg, void* data, uint8_t size );
    uint8_t write( uint8_t reg, uint8_t data );
    bool writeDataBlock( uint8_t classID, uint8_t blockID, void* data, uint8_t length );
    bool readDataBlock( uint8_t classID, uint8_t blockID, void* data, uint8_t length );
    uint8_t calcBlockChecksum();
    uint16_t readWord( uint8_t reg );
    uint16_t readControlData( uint16_t function );
    bool executeControlCommand( uint16_t function );
    bool enterConfig();
    bool exitConfig();
    bool seal();
    bool unseal();

    bool _init = false;     /* Class initialized */
};


/* Battery monitor */
extern BQ27441 g_battery ;

#endif /* BQ27441_H */