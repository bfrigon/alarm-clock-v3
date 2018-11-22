//******************************************************************************
//
// Project : Alarm Clock V3
// File    : main.ino
// Author  : Benoit Frigon <www.bfrigon.com>
//
// -----------------------------------------------------------------------------
//
// This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
// International License. To view a copy of this license, visit
//
// http://creativcommons.org/licenses/by-sa/4.0/
//
// or send a letter to Creative Commons,
// PO Box 1866, Mountain View, CA 94042, USA.
//
//******************************************************************************
#include "src/hardware.h"
#include "src/ui/ui.h"
#include "src/config.h"


Alarm g_alarm( PIN_VS1053_RESET, PIN_VS1053_CS, PIN_VS1053_XDCS, PIN_VS1053_DREQ,
               PIN_VS1053_SDCS, PIN_SD_DETECT, PIN_ALARM_SW, PIN_AMP_SHDN );
NeoClock g_clock( PIN_NEOCLOCK, PIN_PIX_SHDN );
Lamp g_lamp( PIN_PIX_LAMP );
QT1070 g_keypad( PIN_INT_KEYPAD );
US2066 g_lcd( I2C_ADDR_OLED, PIN_OLED_RESET );
Power g_power( PIN_ON_BATTERY, PIN_SYSOFF, PIN_FACTORY_RESET );
DS3231 g_rtc( PIN_INT_RTC );
ConfigManager g_config;



/*--------------------------------------------------------------------------
 *
 * Checks if the check button is held after reset or if EEPROM contains an
 * invalid magic code. In that case, all settings are restored to default.
 *
 * Arguments
 * ---------
 *  None
 *
 * Returns : TRUE if settings were reset to default or False otherwise.
 */
bool checkFactoryReset() {

    /* No need to reset settings */
    if( g_config.isEepromValid() == true && g_power.detectConfigResetButton() == false ) {
        return false;
    }

    if( g_power.detectConfigResetButton() == true ) {

        unsigned long start = millis();

        /* Wait 1 second to test if the config reset continues to be pressed */
        while( g_power.detectConfigResetButton() == true ) {

            if( ( millis() - start ) >= 1000 ) {
                break;
            }
        }

        /* Config reset button was released before the timeout.
        No need to reset settings */
        if( g_power.detectConfigResetButton() == false ) {
            return false;
        }
    }


    /* Set the clock display to "88:88" */
    g_clock.begin();
    g_clock.setColorFromTable( COLOR_RED );
    g_clock.setBrightness( 50 );
    g_clock.setTestMode( true );
    g_clock.update();

    g_config.reset();
    delay( 1000 );

    g_clock.restoreClockDisplay();

    return true;
}


/*--------------------------------------------------------------------------
 *
 * Initialization
 *
 */
void setup() {

    pinMode( PIN_WIFI_RESET, OUTPUT );
    digitalWrite( PIN_WIFI_RESET, LOW );

    pinMode( PIN_WIFI_ENABLE, OUTPUT );
    digitalWrite( PIN_WIFI_ENABLE, LOW );


    /* Setup serial */
    Serial.begin( 115200 );

    /* Setup I2C */
    Wire.begin();
    Wire.setClock( 100000 );


    /* Initialize UI */
    initScreens();

    /* Initialize power management driver */
    g_power.begin();

    /* Check for factory reset sequence */
    if( checkFactoryReset() == false ) {

        /* Load configuration from EEPROM */
        g_config.load();
    }

    /* Initialize clock display */
    g_clock.begin();
    g_lamp.begin();
    

    /* Set clock display color and brightness */
    g_clock.setColorFromTable( g_config.settings.clock_color );
    g_clock.setBrightness( g_config.settings.clock_brightness );
    g_clock.update();

    /* Initialize DS3231 RTC */
    g_rtc.begin();
    g_rtc.setAlarmFrequency( RTC_ALARM_EVERY_SECOND );
    g_rtc.enableInterrupt();
    g_clock.restoreClockDisplay();

    /* Initialize OLED display */
    g_lcd.begin();
    g_lcd.setContrast( g_config.settings.lcd_contrast );

    /* Initialize touch keypad */
    g_keypad.begin();

    /* Initialize audio system */
    g_alarm.begin();



    //enableWifi();


    /* Enable watchdog timer */
    g_power.enableWatchdog();

}


/*--------------------------------------------------------------------------
 *
 * Main loop
 *
 */
void loop() {

    /* Reset watchdog timer */
    g_power.resetWatchdog();

    /* Run power management tasks */
    g_power.runTask();

    /* If an RTC interrupt occured, read the current time */
    g_rtc.processEvents();
    
    /* Update the Clock display if needed */
    g_clock.runTask();
   
    /* Check for alarms, feed alarm audio buffer */
    g_alarm.runTask();

    /* Process keypad events and check if screen has timed out */
    g_currentScreen->runTask();

    /* Update the current screen if requested */
    if( g_screenUpdate == true ) {

        if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
            screen_root.update( true );

        } else {
            g_currentScreen->update();
        }

        g_screenUpdate = false;
    }

    /* Process lamp effect if lamp is active */
    g_lamp.processEvents();

    /* Run config manager tasks */
    g_config.runTask();
}


