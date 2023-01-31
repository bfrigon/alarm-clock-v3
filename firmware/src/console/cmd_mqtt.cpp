//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/cmd_mqtt.cpp
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

#include "console_base.h"
#include "services/mqtt.h"



/*******************************************************************************
 * 
 * @brief   Starts the 'mqtt send' command task.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool ConsoleBase::beginTaskMqttSend() {

    /* Check if parameters are provided */
    char *param = this->getInputParameter();

    if( param == 0 ) {
        
        this->println_P( S_CONSOLE_MISSING_PARAMETER );
        this->print_P( S_CONSOLE_USAGE );
        this->println_P( S_USAGE_MQTT_SEND );
        this->println();

        return false;
    }

    /* Split parameters. first one is the message topic, second one is 
    the payload. */
    char *param_topic, *param_payload, *ptr;
    param_topic = strtok_rP( param, PSTR( "\x20" ), &ptr );
    param_payload = ptr;

    if( strlen( param_topic ) == 0 ) {
        
        this->println_P( S_CONSOLE_MISSING_PARAMETER );
        this->print_P( S_CONSOLE_USAGE );
        this->println_P( S_USAGE_MQTT_SEND );
        this->println();

        return false;
    }

    if( g_mqtt.publish( param_topic, param_payload, false ) == false ) {
        return false;
    }

    return this->startTask( TASK_CONSOLE_MQTT_SEND );
}


/*******************************************************************************
 * 
 * @brief   Monitor the 'mqtt send' task completion.
 * 
 */
void ConsoleBase::runTaskMqttSend() {

    /* Task still running */
    if( g_mqtt.isBusy() == true ) {
        return;
    }

    if( g_mqtt.getTaskError() == TASK_SUCCESS ) {
        this->println_P( S_CONSOLE_MQTT_PUB_ACK );
    }
   
    this->endTask( g_mqtt.getTaskError() );
}


/*******************************************************************************
 * 
 * @brief   Starts the 'mqtt enable' command task.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool ConsoleBase::beginTaskMqttEnable() {

    if( g_config.network.mqtt_enabled == true ) {
        this->println_P( S_CONSOLE_MQTT_IS_ENABLED );
        this->println();

        return false;
    }

    g_config.network.mqtt_enabled = true;
    g_config.save( EEPROM_SECTION_NETWORK );
    
    g_mqtt.enableClient( true );

    if( g_wifi.connected() == false ) {
        return false;
    }

    this->println_P( S_CONSOLE_MQTT_CONNECTING );

    this->startTask( TASK_CONSOLE_MQTT_ENABLE );
    return true;
}


/*******************************************************************************
 * 
 * @brief   Monitor the 'mqtt enable' task completion.
 * 
 */
void ConsoleBase::runTaskMqttEnable() {

    /* Task still running */
    if( g_mqtt.isBusy() == true ) {
        return;
    }

    if( g_mqtt.getTaskError() == TASK_SUCCESS ) {
        this->printfln_P( S_CONSOLE_MQTT_CONNECTED, g_config.network.mqtt_host, g_config.network.mqtt_port );
    }

    this->endTask( g_mqtt.getTaskError() ) ;
}


/*******************************************************************************
 * 
 * @brief   Starts the 'mqtt disable' command task.
 * 
 * @return  TRUE if successful, FALSE otherwise.
 * 
 */
bool ConsoleBase::beginTaskMqttDisable() {

    if( g_config.network.mqtt_enabled == false ) {
        this->println_P( S_CONSOLE_MQTT_IS_DISABLED );
        this->println();

        return false;
    }

    g_config.network.mqtt_enabled = false;
    g_config.save( EEPROM_SECTION_NETWORK );

    g_mqtt.enableClient( false );

    if( g_wifi.connected() == false || g_mqtt.connected() == false ) {
        return false;
    }

    this->startTask( TASK_CONSOLE_MQTT_DISABLE );
    return true;
}


/*******************************************************************************
 * 
 * @brief   Monitor the 'mqtt disable' task completion.
 * 
 */
void ConsoleBase::runTaskMqttDisable() {
    /* Task still running */
    if( g_mqtt.isBusy() == true ) {
        return;
    }

    if( g_mqtt.getTaskError() == TASK_SUCCESS && g_wifi.connected() == true ) {

        this->println_P( S_CONSOLE_MQTT_DISCONNECTED );
    }

    this->endTask( g_mqtt.getTaskError() );
}


/*******************************************************************************
 * 
 * @brief   Prints the status of the MQTT client to the console.
 * 
 */
void ConsoleBase::runCommandMqttStatus() {

    this->println();
    this->printfln_P( S_CONSOLE_MQTT_CLIENT_STATE, ( g_mqtt.enabled() ? S_ENABLED : S_DISABLED ));
    this->printfln_P( S_CONSOLE_MQTT_CONN_STATE, ( g_mqtt.connected() ? S_YES : S_NO ));
    this->printfln_P( S_CONSOLE_MQTT_BROKER_HOST, g_config.network.mqtt_host );
    this->printfln_P( S_CONSOLE_MQTT_BROKER_PORT, g_config.network.mqtt_port );
    this->println();
}