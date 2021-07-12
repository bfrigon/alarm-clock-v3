//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/cmd_batt.cpp
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

#include <task_errors.h>
#include <drivers/bq27441.h>

#include "console_base.h"


/*! ------------------------------------------------------------------------
 * 
 * @brief   Prints the battery health info
 * 
 */
void ConsoleBase::printBattStatus() {


    this->print_P( S_CONSOLE_BATT_STATE );

    switch( g_battery.getBatteryState() ) {
        case BATTERY_STATE_NOT_PRESENT:
            this->println_P( S_CONSOLE_BATT_STATE_MISSING );
            return;

        case BATTERY_STATE_READY:
            this->println_P( S_CONSOLE_BATT_STATE_IDLE );
            break;

        case BATTERY_STATE_CHARGING:
            this->println_P( S_CONSOLE_BATT_STATE_CHG );
            break;

        case BATTERY_STATE_DISCHARGE_FULL:
        case BATTERY_STATE_DISCHARGE_HALF:
        case BATTERY_STATE_DISCHARGE_LOW:
            this->println_P( S_CONSOLE_BATT_STATE_DISCHG );
            break;

        default:
            this->println_P( S_CONSOLE_BATT_STATE_UNKNOWN );
            break;
    }


    uint16_t voltage = g_battery.getVoltage();
    this->printfln_P( S_CONSOLE_BATT_VOLTAGE, voltage / 1000, voltage % 1000 );

    uint8_t soc = g_battery.getStateOfCharge( false );
    this->printfln_P( S_CONSOLE_BATT_CHARGE, soc );

    uint16_t remain_cap = g_battery.getRemainingCapacity( false );
    uint16_t fcap = g_battery.getFullCapacity( false );
    this->printfln_P( S_CONSOLE_BATT_CAPACITY, remain_cap, fcap );

    int16_t current = g_battery.getAvgCurrent();
    this->printfln_P( S_CONSOLE_BATT_CURRENT, current );

    int8_t soh = g_battery.getStateOfHealth();
    this->printfln_P( S_CONSOLE_BATT_SOH, soh );

}