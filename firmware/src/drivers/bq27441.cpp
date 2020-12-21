//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/drivers/bq27441.cpp
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
#include "bq27441.h"



BQ27441::BQ27441() {
}


/*--------------------------------------------------------------------------
 *
 * Initialize the IC.
 *
 * Arguments
 * ---------
 *  - capacity : Design capacity of the battery.
 *
 * Returns :
 */
bool BQ27441::begin( uint16_t capacity ) {

    if( this->readControlData( BQ27441_CONTROL_DEVICE_TYPE ) != 0x421 ) {
        return false;
    }

    /* If POR flag is set, initialize data memory parameters. Otherwise,
       device is ready. */
    if( ( this->readWord( BQ27441_COMMAND_FLAGS ) & BQ27441_FLAG_ITPOR ) == 0 ) {
        this->_init = true;
        return true;
    }


    this->_init = true;

    /* Unseal memory access */
    if( this->unseal() == false ) {
        this->_init = false;
        return false;
    }

    /* Enter config update mode */
    this->enterConfig();

    /* Set design battery capacity */
    uint8_t data[2] = { capacity >> 8, capacity & 0x00FF };
    this->writeDataBlock( BQ27441_ID_STATE, 10, data, sizeof( data ) );

    /* Exit config update mode */
    this->exitConfig();

    /* Seal memory access */
    if( this->seal() == false ) {
        this->_init = false;
        return false;
    }

    // Serial.println( "BQ27441: Initialized data memory parameters" );
    // Serial.print( "BQ27441: Capacity=" );
    // Serial.print( this->readWord( BQ27441_EXTENDED_CAPACITY ) );
    // Serial.println( "mAh" );

    // Serial.println( this->readControlData( BQ27441_CONTROL_STATUS ), HEX );

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Enable config update mode.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool BQ27441::enterConfig() {
    if( this->_init == false ) {
        return false;
    }

    this->executeControlCommand( BQ27441_CONTROL_SET_CFGUPDATE );

    while( ( this->readWord( BQ27441_COMMAND_FLAGS ) & BQ27441_FLAG_CFGUPMODE ) == 0 ) {
        delay( 10 );
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Exits config update mode.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool BQ27441::exitConfig() {
    if( this->_init == false ) {
        return false;
    }

    this->executeControlCommand( BQ27441_CONTROL_SOFT_RESET );

    while( ( this->readWord( BQ27441_COMMAND_FLAGS ) & BQ27441_FLAG_CFGUPMODE ) != 0 ) {
        delay( 10 );
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Un-seal memory access.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool BQ27441::unseal() {
    if( this->_init == false ) {
        return false;
    }

    this->readControlData( BQ27441_UNSEAL_KEY );
    this->readControlData( BQ27441_UNSEAL_KEY );

    return ( ( this->readControlData( BQ27441_CONTROL_STATUS ) & BQ27441_STATUS_SS ) == 0 );
}


/*--------------------------------------------------------------------------
 *
 * Re-seal memory access.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool BQ27441::seal() {
    if( this->_init == false ) {
        return false;
    }

    this->executeControlCommand( BQ27441_CONTROL_SEALED );

    return ( this->readControlData( BQ27441_CONTROL_STATUS ) & BQ27441_STATUS_SS );
}


/*--------------------------------------------------------------------------
 *
 * Checks if the battery monitor has finished initializing.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if ready or FALSE otherwise.
 */
bool BQ27441::isReady() {
    if( this->_init == false ) {
        return false;
    }

    if( this->readControlData( BQ27441_CONTROL_DEVICE_TYPE ) != 0x421 ) {
        return false;
    }

    return ( this->readControlData( BQ27441_CONTROL_STATUS ) & BQ27441_STATUS_INITCOMP );
}


/*--------------------------------------------------------------------------
 *
 * Checks if a battery has been detected.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if present or FALSE otherwise.
 */
bool BQ27441::isBatteryPresent() {
    if( this->_init == false ) {
        return false;
    }

    return( this->readWord( BQ27441_COMMAND_FLAGS ) & BQ27441_FLAG_BAT_DET );
}


/*--------------------------------------------------------------------------
 *
 * Checks if the battery is currently discharging.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if discharging or FALSE otherwise.
 */
bool BQ27441::isDischarging() {
    return ( this->getAvgPower() < 0 );
}


/*--------------------------------------------------------------------------
 *
 * Check if the battery is currently charging.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if charging or FALSE otherwise.
 */
bool BQ27441::isCharging() {
    return ( this->getAvgPower() > 0 );
}

/*--------------------------------------------------------------------------
 *
 * Gets the average power being consumed or stored in the battery.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : The average power in milliwatts (mW). power > 0 if battery 
 *           is being charged or power < 0 if discharged.
 */
int16_t BQ27441::getAvgPower() {
    if( this->_init == false ) {
        return 0;
    }

    return this->readWord( BQ27441_COMMAND_AVG_POWER );
}


/*--------------------------------------------------------------------------
 *
 * Gets the average current being consumed or stored in the battery.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : The average current in milliamps (mA). current > 0 if battery 
 *           is being charged or current < 0 if discharged.
 */
int16_t BQ27441::getAvgCurrent() {
    if( this->_init == false ) {
        return 0;
    }

    return this->readWord( BQ27441_COMMAND_AVG_CURRENT );
}


/*--------------------------------------------------------------------------
 *
 * Gets the state of the battery.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : - BATTERY_STATE_READY          : Battery is detected, fully charged
 *           - BATTERY_STATE_NOT_PRESENT    : No battery detected
 *           - BATTERY_STATE_CHARGING       : Currently charging
 *           - BATTERY_STATE_DISCHARGE_FULL : Discahrging, above full threshold (FC)
 *           - BATTERY_STATE_DISCHARGE_HALF : Discharging, bellow full threshold (FC)
 *           - BATTERY_STATE_DISCHARGE_LOW  : Discharging, bellow low threshold (SOCL)
 */
uint8_t BQ27441::getBatteryState() {
    if( this->_init == false ) {
        return BATTERY_STATE_NOT_PRESENT;
    }

    uint16_t flags = this->readWord( BQ27441_COMMAND_FLAGS );
    int16_t power = this->getAvgPower();

    /* Check if battery is plugged */
    if( ( flags & BQ27441_FLAG_BAT_DET ) == 0 ) {
        return BATTERY_STATE_NOT_PRESENT;
    }

    if( power > 0 ) {
        return BATTERY_STATE_CHARGING;

    } else if( power == 0 ) {
        return BATTERY_STATE_READY;
    }

    if( flags & BQ27441_FLAG_FC ) {
        return BATTERY_STATE_DISCHARGE_FULL;
    }

    if( flags & BQ27441_FLAG_SOC1 ) {
        return BATTERY_STATE_DISCHARGE_LOW;
    }

    return BATTERY_STATE_DISCHARGE_HALF;
}


/*--------------------------------------------------------------------------
 *
 * Gets the estimated remaining battery capacity.
 *
 * Arguments
 * ---------
 *  - Filtered : 
 *
 * Returns : Remaining capacity in milliamps-hour (mAh)
 */
uint16_t BQ27441::getRemainingCapacity( bool filtered ) {
    if( this->_init == false ) {
        return 0;
    }

    return this->readWord( filtered == true ? BQ27441_COMMAND_REM_CAP_FIL : BQ27441_COMMAND_REM_CAP_UNFL );
}


/*--------------------------------------------------------------------------
 *
 * Gets the fully charged battery capacity.
 *
 * Arguments
 * ---------
 *  - Filtered : 
 *
 * Returns : Fully charged capacity in milliamps-hour (mAh)
 */
uint16_t BQ27441::getFullCapacity( bool filtered ) {
    if( this->_init == false ) {
        return 0;
    }

    return this->readWord( filtered == true ? BQ27441_COMMAND_FULL_CAP_FIL : BQ27441_COMMAND_FULL_CAP_UNFL );
}


/*--------------------------------------------------------------------------
 *
 * Gets the battery voltage.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Battery voltage in millivolts (mV).
 */
uint16_t BQ27441::getVoltage() {
    if( this->_init == false ) {
        return 0;
    }

    return this->readWord( BQ27441_COMMAND_VOLTAGE );
}


/*--------------------------------------------------------------------------
 *
 * Gets the battery health percentage.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : Calculated battery health in percent or -1 if monitor is not 
 *           ready or if battery was not detected.
 */
int8_t BQ27441::getStateOfHealth() {
    if( this->_init == false ) {
        return -1;
    }

    uint8_t result[2];

    if( this->read( BQ27441_COMMAND_SOH, &result, sizeof( result ) ) != sizeof( result ) ) {
        return -1;
    }

    if( result[0] > 100 ) {
        /* Invalid percent value */
        return -1;
    }

    if( result[1] == 0x00 ) {
        /* SOH status 0x00 = not valid */
        return -1;
    }

    return result[0];
}


/*--------------------------------------------------------------------------
 *
 * Gets the calculated state of charge
 *
 * Arguments
 * ---------
 *  - Filtered : 
 *
 * Returns : State of charge in percent.
 */
uint8_t BQ27441::getStateOfCharge( bool filtered ) {
    if( this->_init == false ) {
        return 0;
    }

    return this->readWord( filtered == true ? BQ27441_COMMAND_SOC : BQ27441_COMMAND_SOC_UNFL );
}


/*--------------------------------------------------------------------------
 *
 * Transmit data to the monitor IC.
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
uint8_t BQ27441::write( uint8_t reg, void* data, uint8_t size ) {
    uint8_t res;

    Wire.beginTransmission( I2C_ADDR_BQ27441 );
    Wire.write( reg );
    Wire.write( ( char* )data, size );
    res = Wire.endTransmission( true );

    delayMicroseconds( 100 );

    return res;
}


/*--------------------------------------------------------------------------
 *
 * Transmit a single byte to the monitor IC.
 *
 * Arguments
 * ---------
 *  - reg  : Register address to write to.
 *  - data : byte to write.
 *
 * Returns : Status of the transmission
 *   0: Success
 *   1: Data too long to fit in transmit buffer
 *   2: Received NACK on transmit of address
 *   3: Received NACK on transmit of data
 *   4: Other error
 */
uint8_t BQ27441::write( uint8_t reg, uint8_t data ) {
    return this->write( reg, &data, sizeof( data ) );
}


/*--------------------------------------------------------------------------
 *
 * Read data from the monitor IC.
 *
 * Arguments
 * ---------
 *  - reg  : Register address to start reading from.
 *  - data : Pointer to the buffer.
 *  - size : Size of the data.
 *
 * Returns : The number of bytes read.
 */
uint8_t BQ27441::read( uint8_t reg, void* data, uint8_t size ) {
    uint8_t length = 0;

    Wire.beginTransmission( I2C_ADDR_BQ27441 );
    Wire.write( reg );
    Wire.endTransmission( true );


    Wire.requestFrom( I2C_ADDR_BQ27441, ( int )size, true );

    while( Wire.available() ) {
        *( ( char* )data + length ) = Wire.read();
        length++;
    }

    delayMicroseconds( 100 );

    return length;
}


/*--------------------------------------------------------------------------
 *
 * Read a 2-bytes (word) value from the monitor IC
 *
 * Arguments
 * ---------
 *  - reg  : Register address where to read the word from.
 *
 * Returns : Word value (2 bytes)
 */
uint16_t BQ27441::readWord( uint8_t reg ) {

    uint8_t data[2];
    this->read( reg, data, 2 );

    return ( ( uint16_t ) data[1] << 8 ) | data[0];
}


/*--------------------------------------------------------------------------
 *
 * Execute a control function and read the return value.
 *
 * Arguments
 * ---------
 *  - function  : Function to execute.
 *
 * Returns : Function result (2 bytes)
 */
uint16_t BQ27441::readControlData( uint8_t function ) {
    if( this->executeControlCommand( function ) == false ) {
        return 0;
    }

    return this->readWord( BQ27441_COMMAND_CONTROL );
}


/*--------------------------------------------------------------------------
 *
 * Execute a control function..
 *
 * Arguments
 * ---------
 *  - function  : Function to execute.
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool BQ27441::executeControlCommand( uint8_t function ) {
    uint8_t data[2] = { function & 0x00FF, function >> 8};

    return ( this->write( BQ27441_COMMAND_CONTROL, data, 2 ) == 0 );
}


/*--------------------------------------------------------------------------
 *
 * Read a data block when in update config mode.
 *
 * Arguments
 * ---------
 *  - classID  : Data class ID to access.
 *  - offset   : Offset in the datab class to start reading from.
 *  - data     : Pointer to a memory location where to store the read data.
 *  - size     : Size of the data to read.
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool BQ27441::readDataBlock( uint8_t classID, uint8_t offset, void* data, uint8_t length ) {
    if( this->_init == false ) {
        return false;
    }

    if( ( this->readWord( BQ27441_COMMAND_FLAGS ) & BQ27441_FLAG_CFGUPMODE ) == 0 ) {
        return false;
    }

    /* Enable data block access */
    if( this->write( BQ27441_EXTENDED_CONTROL, 0x00 ) != 0 ) {
        return false;
    }

    /* Set data class ID */
    if( this->write( BQ27441_EXTENDED_DATACLASS, classID ) != 0 ) {
        return false;
    }

    /* Set data class offset */
    if( this->write( BQ27441_EXTENDED_DATABLOCK, offset / 32 ) != 0 ) {
        return false;
    }

    /* Read data from the block */
    return ( this->read( BQ27441_EXTENDED_BLOCKDATA + ( offset % 32 ), data, length ) == length );
}


/*--------------------------------------------------------------------------
 *
 * Write to a data block when in config update mode.
 *
 * Arguments
 * ---------
 *  - classID  : Data class ID to access.
 *  - offset   : Offset in the datab class to start reading from.
 *  - data     : Pointer to the data to write.
 *  - size     : Size of the data to write.
 *
 * Returns : TRUE if successful or FALSE otherwise.
 */
bool BQ27441::writeDataBlock( uint8_t classID, uint8_t offset, void* data, uint8_t length ) {
    if( this->_init == false ) {
        return false;
    }

    if( ( this->readWord( BQ27441_COMMAND_FLAGS ) & BQ27441_FLAG_CFGUPMODE ) == 0 ) {
        return false;
    }

    /* Enable data block access */
    if( this->write( BQ27441_EXTENDED_CONTROL, 0x00 ) != 0 ) {
        return false;
    }

    /* Set data class ID */
    if( this->write( BQ27441_EXTENDED_DATACLASS, classID ) != 0 ) {
        return false;
    }

    /* Set data class offset */
    if( this->write( BQ27441_EXTENDED_DATABLOCK, offset / 32 ) != 0 ) {
        return false;
    }

    /* Write data to the block */
    if( this->write( BQ27441_EXTENDED_BLOCKDATA + ( offset % 32 ), data, length ) != 0 ) {
        return false;
    }

    uint8_t crc = this->calcBlockChecksum();

    /* Save the block to RAM by updating the checksum */
    if( this->write( BQ27441_EXTENDED_CHECKSUM, crc ) != 0 ) {
        return false;
    }

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Calculate the checksum of the 32 bytes block currently accessed.
 *
 * Arguments
 * ---------
 *
 * Returns : CRC value.
 */
uint8_t BQ27441::calcBlockChecksum() {

    uint8_t block[32];

    if( this->read( BQ27441_EXTENDED_BLOCKDATA, block, sizeof( block ) ) != sizeof( block ) ) {
        return 0;
    }

    uint8_t csum = 0;

    for( uint8_t i = 0; i < sizeof( block ); i++ ) {
        csum += block[i];
    }

    return 255 - csum;
}