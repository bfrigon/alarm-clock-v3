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

    if( g_power.getPowerMode() != g_power.detectPowerState() ) {
        g_screenUpdate = true;
    }

    /* Process Keypad event if available */
    uint8_t key = g_keypad.processEvents();

    if( key != KEY_NONE ) {
        g_currentScreen->processKeypadEvent( key );
    }

    /* Process RTC event if available */
    if( g_rtc.processEvents() == true ) {

        DateTime now = g_rtc.now();
        
        if( now.hour() != g_clock.hour || now.minute() != g_clock.minute ) {
            g_alarm.checkForAlarms( &now );

            if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
                g_screenUpdate = true;
            }

            switch( g_currentScreen->getId() ) {
                case SCREEN_ID_SET_TIME:
                case SCREEN_ID_SHOW_ALARMS:
                    /* Don't update clock display on these screens */
                    break;

                case SCREEN_ID_ROOT:
                    g_screenUpdate = true;

                /* Fall-through */

                default:
                    g_clock.hour = now.hour();
                    g_clock.minute = now.minute();

                    g_clockUpdate = true;
            }
        }
    }

    /* Detect if the SD card is present, if so, initialize it */
    if( g_alarm.isSDCardPresent() != g_alarm.detectSDCard() ) {
        g_screenUpdate = true;
    }

    /* Detect alarm switch state */
    if( g_alarm.isAlarmSwitchOn() != g_alarm.detectAlarmSwitchState() ) {
        g_power.resetSuspendDelay();
        g_clockUpdate = true;
        g_screenUpdate = true;
    }

    /* Feed alarm audio buffer and process visual effect */
    g_alarm.processAlarmEvents();

    /* Exit the current screen if it has reached its timeout value */
    g_currentScreen->checkScreenTimeout();

    /* Update the current screen if requested */
    if( g_screenUpdate == true ) {

        if( g_power.getPowerMode() == POWER_MODE_SUSPEND ) {
            screen_root.update( true );

        } else {
            g_currentScreen->update();
        }

        g_screenUpdate = false;
    }

    /* Update the Clock display if requested */
    g_clock.processUpdateEvents();

    /* Process lamp effect if lamp is active */
    g_lamp.processEvents();





    g_config.runTask();




    /* Handle pending WiFi module events */
    // g_wifi.handleEvents();

    // if( g_wifi.statusChanged() == true ) {

    //     if( g_currentScreen->getId()() == SCREEN_ID_ROOT )  {
    //         g_screenUpdate = true;
    //     }
    // }



}


