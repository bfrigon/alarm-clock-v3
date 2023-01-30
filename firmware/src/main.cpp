//******************************************************************************
//
// Project : Alarm Clock V3
// File    : main.cpp
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

#include <hardware.h>
#include <config.h>
#include <freemem.h>
#include "services/console.h"
#include "services/telnet_console.h"
#include "services/ntpclient.h"
#include "services/mqtt.h"
#include "services/homeassistant.h"
#include "services/logger.h"
#include "ui/ui.h"



Alarm           g_alarm( PIN_VS1053_RESET, PIN_VS1053_CS, PIN_VS1053_XDCS, PIN_VS1053_DREQ,
                         PIN_VS1053_SDCS, PIN_SD_DETECT, PIN_ALARM_SW, PIN_AMP_SHDN );
WiFi            g_wifi( PIN_WIFI_CS, PIN_WIFI_IRQ, PIN_WIFI_RESET, PIN_WIFI_ENABLE );
NeoClock        g_clock( PIN_NEOCLOCK, PIN_PIX_SHDN );
Lamp            g_lamp( PIN_PIX_LAMP );
QT1070          g_keypad( PIN_INT_KEYPAD );
US2066          g_lcd( I2C_ADDR_OLED, PIN_OLED_RESET );
Power           g_power( PIN_ON_BATTERY, PIN_SYSOFF, PIN_FACTORY_RESET );
DS3231          g_rtc( PIN_INT_RTC );
ALS             g_als;
BQ27441         g_battery;
ConfigManager   g_config;
Console         g_console;
TelnetConsole   g_telnetConsole;
TimeZone        g_timezone;
NtpClient       g_ntp;
Screen          g_screen;
Logger          g_log;
MqttClient      g_mqtt;
HomeAssistant   g_homeassistant;

bool g_prev_state_wifi = false;
bool g_prev_state_telnetConsole = false;



/*******************************************************************************
 *
 * @brief   Checks if the check button is held after reset or if EEPROM 
 *          contains an invalid magic code. In that case, all settings are 
 *          restored to default.
 *
 * @return  TRUE if settings were reset to default, FALSE otherwise.
 * 
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

    /* Reset EEPROM to default values */
    g_config.reset();
    delay( 1000 );

    g_clock.restoreClockDisplay();

    return true;
}


/*******************************************************************************
 *
 * @brief   Initialize drivers
 *
 */
void setup() {
    
    
    g_log.add( EVENT_RESET, MCUSR );
    MCUSR = 0;

    /* Setup console */
    g_console.begin( 115200 );
    g_console.clearScreen();
    g_console.println_P( S_CONSOLE_INIT );

    /* Setup I2C */
    Wire.begin();
    Wire.setClock( 100000 );

    /* Initialize power management driver */
    g_power.begin();
    g_battery.begin( BATTERY_DESIGN_CAPACITY );

    /* Initialize ambiant light detector */
    g_als.begin();

    /* Check for factory reset sequence */
    if( checkFactoryReset() == false ) {

        /* Load configuration from EEPROM */
        g_config.load();
    }

    /* Initialize time zone */
    g_timezone.setTimezoneByName( g_config.clock.timezone );

    /* Initialize clock display */
    g_clock.begin();
    g_lamp.begin();

    /* Set clock display color and brightness */
    g_clock.setColorFromTable( g_config.clock.clock_color );
    g_clock.setBrightness( g_config.clock.clock_brightness );
    g_clock.update();

    /* Initialize DS3231 RTC */
    g_rtc.begin();
    g_rtc.setAlarmFrequency( RTC_ALARM_EVERY_SECOND );
    g_rtc.enableInterrupt();
    g_clock.restoreClockDisplay();

    /* Initialize OLED display */
    g_lcd.begin();
    g_lcd.setContrast( g_config.clock.lcd_contrast );

    /* Initialize touch keypad */
    g_keypad.begin();

    /* Initialize audio system */
    g_alarm.begin();

    /* Initialize WIFI driver */
    g_wifi.begin();
    g_wifi.setSystemTime( g_rtc.now() );

    /* Initialize UI */
    g_screen.activate( &screen_root );

    /* Display console prompt */
    g_console.resetConsole();

    /* Start telnet server if enabled */
    g_telnetConsole.enableServer( g_config.network.telnetEnabled );

    /* Connect to the mqtt broker if enabled */
    g_mqtt.begin();

    /* Enable home assistant client via MQTT */
    g_homeassistant.begin();

    /* Enable automatic ntp sync at random interval */
    g_ntp.setAutoSync( g_config.clock.use_ntp );

    /* Enable watchdog timer */
    g_power.enableWatchdog();
}


/*******************************************************************************
 *
 * @brief   Main loop
 *
 */
void loop() {
    g_freeMemory = freeMemory();

    /* Reset watchdog timer */
    g_power.resetWatchdog();

    /* Run power management tasks */
    g_power.detectPowerState();

    /* If an RTC interrupt occured, read the current time */
    g_rtc.processEvents();
    
    /* Update the Clock display if needed */
    g_clock.processEvents();
   
    /* Check for alarms, feed alarm audio buffer */
    g_alarm.processEvents();

    /* Process keypad events and check if screen has timed out */
    g_screen.processEvents();

    /* Update the current screen if requested */
    g_screen.update();

    /* Process lamp effect if lamp is active */
    g_lamp.processEvents();

    /* Run config manager tasks */
    g_config.runTasks();

    /* Run ambiand light sensor tasks */
    g_als.processEvents();

    /* Process WIFI driver events */
    g_wifi.runTasks();

    /* Process serial console inputs */
    g_console.runTasks();

    /* Run NTP client tasks */
    g_ntp.runTasks();

    /* Process telnet server events */
    g_telnetConsole.runTasks();

    /* Process MQTT client events */
    g_mqtt.runTasks();

    /* Push events to Home Assistant via MQTT */
    g_homeassistant.runTasks();

    /* Update status icons on main display */
    if( g_telnetConsole.clientConnected() != g_prev_state_telnetConsole ) {
        g_prev_state_telnetConsole = g_telnetConsole.clientConnected();

        if( g_screen.getId() == SCREEN_ID_ROOT ) {
            g_screen.requestScreenUpdate( false );
        }
    }

    if( g_wifi.connected() != g_prev_state_wifi ) {
        g_prev_state_wifi = g_wifi.connected();

        if( g_screen.getId() == SCREEN_ID_ROOT ) {
            g_screen.requestScreenUpdate( false );
        }

        if( g_screen.getId() == SCREEN_ID_NET_STATUS ) {
            g_screen.requestScreenUpdate( true );
        }
    }
}