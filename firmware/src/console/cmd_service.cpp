//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/cmd_service.cpp
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

#include <resources.h>
#include <services/ntpclient.h>
#include <services/telnet_console.h>
#include <services/mqtt.h>
#include <services/ftpserver.h>
#include "console_base.h"



/* Commands syntax */
PROG_STR( S_ACTION_ENABLE,  "enable" );
PROG_STR( S_ACTION_DISABLE, "disable" );
PROG_STR( S_ACTION_STATUS,  "status" );
PROG_STR( S_SERVICE_TELNET, "telnet" );
PROG_STR( S_SERVICE_NTP,    "ntp" );
PROG_STR( S_SERVICE_MQTT,   "mqtt" );
PROG_STR( S_SERVICE_FTP,    "ftp" );

/* Service IDs */
enum {
    SERVICE_TELNET = 1,
    SERVICE_NTP,
    SERVICE_MQTT,
    SERVICE_FTP
};

/* Actions */
enum {
    SERVICE_ACTION_DISABLE,
    SERVICE_ACTION_ENABLE,
    SERVICE_ACTION_STATUS
};



/*******************************************************************************
 *
 * @brief   Run the 'service' command.
 *           
 */
void ConsoleBase::runCommandService() {

    /* Check if parameters are provided */
    char *param = this->getInputParameter();

    if( param == 0 ) {
        
        this->println_P( S_CONSOLE_MISSING_PARAMETER );
        this->print_P( S_CONSOLE_USAGE );
        this->println_P( S_USAGE_SERVICE );
        this->println();

        return;
    }

    /* Split parameters. first one is the service name, second one is 
       the action to take. */
    char *param_name, *param_action, *ptr;
    param_name = strtok_rP( param, PSTR( "\x20" ), &ptr );
    param_action = strtok_rP( NULL, PSTR( "\x20" ), &ptr );

    if( strlen( param_name ) == 0 || strlen( param_action ) == 0 ) {
        
        this->println_P( S_CONSOLE_MISSING_PARAMETER );
        this->print_P( S_CONSOLE_USAGE );
        this->println_P( S_USAGE_SERVICE );
        this->println();

        return;
    }

    uint8_t action;
    if( strcasecmp_P( param_action, S_ACTION_DISABLE ) == 0 ) {
        action = SERVICE_ACTION_DISABLE;

    } else if( strcasecmp_P( param_action, S_ACTION_ENABLE ) == 0 ) {
        action = SERVICE_ACTION_ENABLE;
    } else if( strcasecmp_P( param_action, S_ACTION_STATUS ) == 0 ) {
        action = SERVICE_ACTION_STATUS;
    } else {
        this->println_P( S_CONSOLE_SERV_INVALID_OPT );
        this->print_P( S_CONSOLE_USAGE );
        this->println_P( S_USAGE_SERVICE );
        this->println();

        return;
    }

    
    /* Telnet console */
    if( strcasecmp_P( param_name, S_SERVICE_TELNET ) == 0 ) {

        switch( action ) {

            /* Action : Enable */
            case SERVICE_ACTION_ENABLE:
                if( g_config.network.telnetEnabled == false ) {    

                    g_config.network.telnetEnabled = true;
                    g_config.save( EEPROM_SECTION_NETWORK );

                    g_telnetConsole.enableServer( true );
                } 

                this->println_P( S_CONSOLE_TELNET_ENABLED );
                break;

            /* Action : Disable */
            case SERVICE_ACTION_DISABLE:
                if( g_config.network.telnetEnabled == true ) {    

                    g_config.network.telnetEnabled = false;
                    g_config.save( EEPROM_SECTION_NETWORK );

                    g_telnetConsole.enableServer( false );
                } 

                this->println_P( S_CONSOLE_TELNET_DISABLED );
                break;

            /* Action : Status */
            case SERVICE_ACTION_STATUS:
                g_telnetConsole.printConsoleStatus( this );
                break;
        }

        this->println();


    /* NTP auto synchronization */
    } else if( strcasecmp_P( param_name, S_SERVICE_NTP ) == 0 ) {

        switch( action ) {

            /* Action : Enable */
            case SERVICE_ACTION_ENABLE:
               if( g_config.clock.use_ntp == false ) {    

                    g_config.clock.use_ntp = true;
                    g_config.save( EEPROM_SECTION_CLOCK );

                    g_ntp.setAutoSync( true );
                }

                this->print_P( S_CONSOLE_NTP_AUTOSYNC );
                this->println_P( S_ENABLED );

                break;


            /* Action : Disable */
            case SERVICE_ACTION_DISABLE:
               if( g_config.clock.use_ntp == true ) {    

                    g_config.clock.use_ntp = false;
                    g_config.save( EEPROM_SECTION_CLOCK );

                    g_ntp.setAutoSync( false );
                }

                this->print_P( S_CONSOLE_NTP_AUTOSYNC );
                this->println_P( S_DISABLED );

                break;

            /* Action : Status */
            case SERVICE_ACTION_STATUS:
                g_ntp.printNTPStatus( this );
                break;

        }

        this->println();
        
    /* MQTT client */
    } else if( strcasecmp_P( param_name, S_SERVICE_MQTT ) == 0 ) {

        switch( action ) {

            /* Action : Enable */
            case SERVICE_ACTION_ENABLE:
                if( g_config.network.mqtt_enabled == false ) {    

                    g_config.network.mqtt_enabled = true;
                    g_config.save( EEPROM_SECTION_NETWORK );

                    g_mqtt.enableClient( true );
                } 

                this->println_P( S_CONSOLE_MQTT_ENABLED );
                break;

            /* Action : Disable */
            case SERVICE_ACTION_DISABLE:
                if( g_config.network.mqtt_enabled == true ) {    

                    g_config.network.mqtt_enabled = false;
                    g_config.save( EEPROM_SECTION_NETWORK );

                    g_mqtt.enableClient( false );
                } 

                this->println_P( S_CONSOLE_MQTT_DISABLED );
                break;

            /* Action : Status */
            case SERVICE_ACTION_STATUS:
                this->runCommandMqttStatus();
                break;

        }

        this->println();

    /* FTP server */
    } else if( strcasecmp_P( param_name, S_SERVICE_FTP ) == 0 ) {

        switch( action ) {

            /* Action : Enable */
            case SERVICE_ACTION_ENABLE:
                if( g_config.network.ftp_enabled == false ) {    

                    g_config.network.ftp_enabled = true;
                    g_config.save( EEPROM_SECTION_NETWORK );

                    g_ftpServer.enableServer( true );
                } 

                this->println_P( S_CONSOLE_FTP_ENABLED );
                break;

            /* Action : Disable */
            case SERVICE_ACTION_DISABLE:
                if( g_config.network.ftp_enabled == true ) {    

                    g_config.network.ftp_enabled = false;
                    g_config.save( EEPROM_SECTION_NETWORK );

                    g_ftpServer.enableServer( false );
                } 

                this->println_P( S_CONSOLE_FTP_DISABLED );
                break;

            /* Action : Status */
            case SERVICE_ACTION_STATUS:
                g_ftpServer.printServerStatus( this );
                break;

        }

        this->println();


    /* Unknown service */
    } else {
        this->println_P( S_CONSOLE_SERV_UNKNOWN );
        this->println();
    }
}