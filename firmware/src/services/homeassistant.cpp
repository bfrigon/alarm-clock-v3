//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/homeassistant.cpp
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

#include "homeassistant.h"
#include "mqtt.h"
#include "logger.h"
#include "timezone.h"
#include "ui/ui.h"
#include "drivers/us2066.h"



/*******************************************************************************
 *
 * @brief   Class initializer
 *
 */
HomeAssistant::HomeAssistant() {
    _will_topic[0] = 0;
    _will_payload[0] = 0;
    _taskCurrentSensorID = SENSOR_ID_NONE;

    memset( this->lcd_message, 0, MAX_PAYLOAD_LCD_MESSAGE_LENGTH + 1 );
}


/*******************************************************************************
 *
 * @brief   Initialize the class and start connection to the MQTT broker
 * 
 */
void HomeAssistant::begin() {

    uint8_t mac[6];
    g_wifi.getMacAddress( mac );

    snprintf_P( _ha_device_id, MAX_HA_DEVICE_ID_LENGTH + 1, S_HA_ID_FORMAT, mac[ 0 ], mac[ 1 ], mac[ 2 ], mac[ 3 ], mac[ 4 ], mac[ 5 ] );
    snprintf_P( _will_topic, MAX_WILL_TOPIC_LENGTH + 1, S_TOPIC_AVAILABILITY, g_config.network.discovery_prefix, _ha_device_id );
    strncpy_P( _will_payload, S_PAYLOAD_AVAIL_OFFLINE , MAX_PAYLOAD_AVAILABILITY_LENGTH + 1 );
    memset( this->lcd_message, 0, MAX_PAYLOAD_LCD_MESSAGE_LENGTH + 1 );

    g_mqtt.setWillMessage( _will_topic, _will_payload, true, true );

    /* Start task waiting for the MQTT client to conenct to the broker */
    this->startTask( TASK_HOMEASSISTANT_WAIT_MQTT_CONNECT );
   
    
}


/*******************************************************************************
 *
 * @brief   Schedule all sensors to send update to home assistant.
 * 
 * @param   force   Schedule an update regardless of the last update time.
 * 
 * @return  TRUE if sensors updates were scheduled, FALSE otherwise.
 * 
 */
bool HomeAssistant::updateAllSensors( bool force ) {
    uint8_t i;
    bool scheduled = false;
    for( i = 1; i <= MAX_SENSORS_ID; i++ ) {

        if( this->updateSensor( i, force ) == true ) {
            scheduled = true;
        }
    }

    return scheduled;
}


/*******************************************************************************
 *
 * @brief   Schedule a specific sensor to send update to home assistant.
 * 
 * @param   sensorID    ID of the sensor to update
 * @param   force       Schedule an update regardless of the last update time.
 *
 * @return  TRUE if update scheduled, FALSE if ignored.
 */
bool HomeAssistant::updateSensor( uint8_t sensorID, bool force ) {
    if( sensorID > MAX_SENSORS_ID || sensorID == 0 ) {
        return false;
    }

    if( force == false ) {
        switch( sensorID ) {

            /* WiFi measured RSSI sensor */
            case SENSOR_ID_CONN_RSSI: {

                if( millis() - _prevTimestampConnRssi < MAX_UPDATE_RATE_CONN_RSSI ) {
                    return false;
                }
            }
            break;

            /* Battery charge sensor */
            case SENSOR_ID_BATTERY_CHARGE: {

                if( millis() - _prevTimestampBatteryCharge < MAX_UPDATE_RATE_BATTERY_CHARGE ) {
                    return false;
                }
            }
            break;

            /* Battery charge sensor */
            case SENSOR_ID_BATTERY_VOLT: {

                if( millis() - _prevTimestampBatteryVoltage < MAX_UPDATE_RATE_BATTERY_VOLTAGE ) {
                    return false;
                }
            }
            break;            

            /* Battery status sensor */
            case SENSOR_ID_BATTERY_STATUS: {
                
                if( millis() - _prevTimestampBatteryStatus < MAX_UPDATE_RATE_BATTERY_STATUS ) {
                    return false;
                }

                /* Skip update if battery status didn't change */
                uint8_t battState;
                battState = g_battery.getBatteryState();
                if( _prevBatteryStatus == battState ) {
                    return false;
                }

                /* Force update battery charge and voltage sensor when battery state changes */
                this->updateSensor( SENSOR_ID_BATTERY_CHARGE, true );
                this->updateSensor( SENSOR_ID_BATTERY_VOLT, true );

                _prevBatteryStatus = battState;
            }
            break;
        }
    }

    _sensorNeedUpdate[ sensorID - 1 ] = true;
    return true;
}


/*******************************************************************************
 *
 * @brief   Begin sending each sensors config payload.
 *
 */
void HomeAssistant::beginSendSensorConfig() {

    if( g_mqtt.connected() == false ) {
        return;
    }

    this->startTask( TASK_HOMEASSISTANT_SEND_SENSOR_CONFIG, true );

    _taskCurrentSensorID = SENSOR_ID_NONE;
}


/*******************************************************************************
 *
 * @brief   Begin sending sensors state which are flagged for update.
 *
 */
void HomeAssistant::beginSendSensorStates() {

    if( g_mqtt.connected() == false ) {
        return;
    }

    this->startTask( TASK_HOMEASSISTANT_SEND_SENSOR_STATES );

    _taskCurrentSensorID = SENSOR_ID_NONE;
}


/*******************************************************************************
 *
 * @brief   Send the next sensor's config in the list.
 *
 */
void HomeAssistant::sendNextSensorConfig() {

    if( this->getCurrentTask() != TASK_HOMEASSISTANT_SEND_SENSOR_CONFIG ) {
        return;
    }


    char *topic = nullptr;
    char *payload = nullptr;
    size_t topic_len, payload_len;
    bool isSubscribeTopic;    

    switch( _taskCurrentSensorID ) {

        case SENSOR_ID_ALARM_SWITCH:
            isSubscribeTopic = false;
            topic_len = strlen_P( S_TOPIC_CONFIG_ALARM_SWITCH ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_ALARM_SWITCH ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;

        case SENSOR_ID_NEXT_ALARM:
            isSubscribeTopic = false;
            topic_len = strlen_P( S_TOPIC_CONFIG_NEXT_ALARM ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_NEXT_ALARM ) + ( strlen( g_config.network.discovery_prefix ) * 3 ) + ( MAX_HA_DEVICE_ID_LENGTH * 5 ) + 1;
            break;

        case SENSOR_ID_CONN_RSSI:
            isSubscribeTopic = false;
            topic_len = strlen_P( S_TOPIC_CONFIG_CONN_RSSI ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_CONN_RSSI ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;

        case SENSOR_ID_BATTERY_CHARGE:
            isSubscribeTopic = false;
            topic_len = strlen_P( S_TOPIC_CONFIG_BATTERY_CHARGE ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_BATTERY_CHARGE ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;

        case SENSOR_ID_BATTERY_STATUS:
            isSubscribeTopic = false;
            topic_len = strlen_P( S_TOPIC_CONFIG_BATTERY_STATUS ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_BATTERY_STATUS ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;

        case SENSOR_ID_BATTERY_VOLT:
            isSubscribeTopic = false;
            topic_len = strlen_P( S_TOPIC_CONFIG_BATTERY_VOLT ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_BATTERY_VOLT ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;

        case SENSOR_ID_LCD_MESSAGE:
            isSubscribeTopic = false;
            topic_len = strlen_P( S_TOPIC_CONFIG_LCD_MSG ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_LCD_MSG ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;

        case SENSOR_ID_LCD_MESSAGE_SET:
            isSubscribeTopic = true;
            topic_len = strlen_P( S_TOPIC_CMD_LCD_MSG ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            break;

        /* Sendor does not need to send a configuration topic */
        default:
            return;
    }

    /* Allocate buffer for topic */
    topic = ( char* )malloc( topic_len );
    if( topic == nullptr ) {
        return;
    }

    /* Allocate buffer for the sensor state to publish if it is not a
       topic to subscribe to. */
    if( isSubscribeTopic == false ) {
    
        payload = ( char* )malloc( payload_len );

        if( payload == nullptr ) {
            return;
        }
    }

    switch( _taskCurrentSensorID ) {

        /* Alarm switch sensor config */
        case SENSOR_ID_ALARM_SWITCH: {

            snprintf_P( topic, topic_len, S_TOPIC_CONFIG_ALARM_SWITCH, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_ALARM_SWITCH, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* Next alarm timestamp sensor config */
        case SENSOR_ID_NEXT_ALARM: {

            snprintf_P( topic, topic_len, S_TOPIC_CONFIG_NEXT_ALARM, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_NEXT_ALARM, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* Signal strength sensor config */
        case SENSOR_ID_CONN_RSSI: {

            snprintf_P( topic, topic_len, S_TOPIC_CONFIG_CONN_RSSI, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_CONN_RSSI, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* Battery charge sensor config */
        case SENSOR_ID_BATTERY_CHARGE: {

            snprintf_P( topic, topic_len, S_TOPIC_CONFIG_BATTERY_CHARGE, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_BATTERY_CHARGE, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* Battery status sensor config */
        case SENSOR_ID_BATTERY_STATUS: {

            snprintf_P( topic, topic_len, S_TOPIC_CONFIG_BATTERY_STATUS, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_BATTERY_STATUS, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* Battery voltage sensor config */
        case SENSOR_ID_BATTERY_VOLT: {

            snprintf_P( topic, topic_len, S_TOPIC_CONFIG_BATTERY_VOLT, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_BATTERY_VOLT, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* LCD message sensor config */
        case SENSOR_ID_LCD_MESSAGE: {

            snprintf_P( topic, topic_len, S_TOPIC_CONFIG_LCD_MSG, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_LCD_MSG,
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* LCD message sensor set topic */
        case SENSOR_ID_LCD_MESSAGE_SET: {
            snprintf_P( topic, topic_len, S_TOPIC_CMD_LCD_MSG, g_config.network.discovery_prefix, _ha_device_id );
            
        }
        break;
    }
    
    if( isSubscribeTopic == true ) {

        /* Send topic subscription request */
        g_mqtt.subscribe( topic );
    } else {

        /* Publish configuration topic */
        g_mqtt.publish( topic, payload, true );
    }

    free( topic );

    if( payload != nullptr ) {
        free( payload );
    }
}


/*******************************************************************************
 *
 * @brief   Send the next sensor state in the list that needs updating
 *
 */
void HomeAssistant::sendNextSensorState() {

    if( this->getCurrentTask() != TASK_HOMEASSISTANT_SEND_SENSOR_STATES ) {
        return;
    }

    /* Current sensor in the list state has not changed */
    if( _sensorNeedUpdate[ _taskCurrentSensorID - 1 ] == false ) {
        return;
    }

    _sensorNeedUpdate[ _taskCurrentSensorID - 1 ] = false;

    char *topic, *payload;
    size_t topic_len, payload_len;

    switch( _taskCurrentSensorID ) {
        case SENSOR_ID_AVAILABILITY:
            topic_len = strlen_P( S_TOPIC_AVAILABILITY ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_AVAILABILITY_LENGTH + 1;
            break;

        case SENSOR_ID_NEXT_ALARM_AVAILABLE:
            topic_len = strlen_P( S_TOPIC_AVAIL_NEXT_ALARM ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_SWITCH_STATE_LENGTH + 1;
            break;

        case SENSOR_ID_ALARM_SWITCH:
            topic_len = strlen_P( S_TOPIC_STATE_ALARM_SWITCH ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_SWITCH_STATE_LENGTH + 1;
            break;

        case SENSOR_ID_NEXT_ALARM:
            topic_len = strlen_P( S_TOPIC_STATE_NEXT_ALARM ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_TIMESTAMP_STATE_LENGTH + 1;
            break;

        case SENSOR_ID_CONN_RSSI:
            topic_len = strlen_P( S_TOPIC_STATE_CONN_RSSI ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_RSSI_STATE_LENGTH + 1;
            break;

        case SENSOR_ID_BATTERY_CHARGE:
            topic_len = strlen_P( S_TOPIC_STATE_BATTERY_CHARGE ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_BATTERY_CHARGE_LENGTH + 1;
            break;

        case SENSOR_ID_BATTERY_STATUS:
            topic_len = strlen_P( S_TOPIC_STATE_BATTERY_STATUS ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_BATTERY_STATUS_LENGTH + 1;
            break;

       case SENSOR_ID_BATTERY_VOLT:
            topic_len = strlen_P( S_TOPIC_STATE_BATTERY_VOLT ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_BATTERY_VOLTAGE_LENGTH + 1;
            break;

        /* Sensor does not have a state to send */
        default:
            return;
    }

    topic = ( char* )malloc( topic_len );

    payload = ( char* )malloc( payload_len );
    memset( payload, 0, payload_len );

    if( topic == nullptr || payload == nullptr ) {
        return;
    }

    switch( _taskCurrentSensorID ) {

        /* Device online availability status */
        case SENSOR_ID_AVAILABILITY: {

            snprintf_P( topic, topic_len, S_TOPIC_AVAILABILITY, g_config.network.discovery_prefix, _ha_device_id );
            strncpy_P( payload, S_PAYLOAD_AVAIL_ONLINE, payload_len );
        }
        break;

        /* Alarm switch sensor state */
        case SENSOR_ID_ALARM_SWITCH: {

            snprintf_P( topic, topic_len, S_TOPIC_STATE_ALARM_SWITCH, g_config.network.discovery_prefix, _ha_device_id );
            strncpy_P( payload, ( g_alarm.isAlarmSwitchOn() ? S_PAYLOAD_SWITCH_ON : S_PAYLOAD_SWITCH_OFF ), payload_len );
        }
        break;

        /* Next alarm timestamp */
        case SENSOR_ID_NEXT_ALARM: {

            snprintf_P( topic, topic_len, S_TOPIC_STATE_NEXT_ALARM, g_config.network.discovery_prefix, _ha_device_id );

            DateTime local;
            local = g_rtc.now();
            g_timezone.toLocal( &local );
            
            int8_t next_alarm_id;
            next_alarm_id = g_alarm.getNextAlarmID( &local, false );

            if( next_alarm_id < 0 || next_alarm_id >= MAX_NUM_PROFILES ) {

                /* Skip state publish if alarm is off, availability topic in config 
                   will cause sensor to become unavailable instead. */
                free( topic );
                free( payload );
                return;

            } else {

                int16_t next_alarm_offset;
                next_alarm_offset = g_alarm.getNextAlarmOffset( next_alarm_id, &local, false );

                DateTime next_alarm = g_rtc.now();
                next_alarm += (( uint32_t )next_alarm_offset * 60 );

                snprintf_P( payload, payload_len, S_PAYLOAD_TIMESTAMP, 
                            next_alarm.year(),
                            next_alarm.month(),
                            next_alarm.day(),
                            next_alarm.hour(),
                            next_alarm.minute() );
            }
        }
        break;

        /* Next alarm available sensor */
        case SENSOR_ID_NEXT_ALARM_AVAILABLE: {
            snprintf_P( topic, topic_len, S_TOPIC_AVAIL_NEXT_ALARM, g_config.network.discovery_prefix, _ha_device_id );
            strncpy_P( payload, (( g_alarm.isAlarmEnabled() == true ) ? S_PAYLOAD_SWITCH_ON : S_PAYLOAD_SWITCH_OFF ), payload_len );
        }
        break;

        /* WiFi signal strength sensor */
        case SENSOR_ID_CONN_RSSI: {
            
            snprintf_P( topic, topic_len, S_TOPIC_STATE_CONN_RSSI, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_PAYLOAD_INTEGER, g_wifi.getRSSI() );

            _prevTimestampConnRssi = millis();
        }
        break;

        /* Battery charge sensor */
        case SENSOR_ID_BATTERY_CHARGE: {
            
            snprintf_P( topic, topic_len, S_TOPIC_STATE_BATTERY_CHARGE, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_PAYLOAD_INTEGER, g_battery.getStateOfCharge( false ));

            _prevTimestampBatteryCharge = millis();
        }
        break;

        /* Battery voltage sensor */
        case SENSOR_ID_BATTERY_VOLT: {
            
            snprintf_P( topic, topic_len, S_TOPIC_STATE_BATTERY_VOLT, g_config.network.discovery_prefix, _ha_device_id );

            uint16_t batt_v;
            batt_v = g_battery.getVoltage();

            snprintf_P( payload, payload_len, S_PAYLOAD_VOLTAGE, batt_v / 1000, batt_v % 1000 );

            _prevTimestampBatteryVoltage = millis();
        }
        break;

        /* Battery status sensor */
        case SENSOR_ID_BATTERY_STATUS: {
            
            snprintf_P( topic, topic_len, S_TOPIC_STATE_BATTERY_STATUS, g_config.network.discovery_prefix, _ha_device_id );

            switch( g_battery.getBatteryState() ) {

                case BATTERY_STATE_CHARGING:
                    strncpy_P( payload, S_PAYLOAD_BATT_CHARGING, payload_len );
                    break;

                case BATTERY_STATE_DISCHARGE_FULL:
                case BATTERY_STATE_DISCHARGE_HALF:
                case BATTERY_STATE_DISCHARGE_LOW:
                    strncpy_P( payload, S_PAYLOAD_BATT_DISCHARGING, payload_len );
                    break;

                case BATTERY_STATE_READY:
                    strncpy_P( payload, S_PAYLOAD_BATT_FULL, payload_len );
                    break;

                case BATTERY_STATE_NOT_PRESENT:
                    strncpy_P( payload, S_PAYLOAD_BATT_NOT_PRESENT, payload_len );
                    break;

                default:
                    strncpy_P( payload, S_PAYLOAD_BATT_UNKNOWN, payload_len );
                    break;
            }

            _prevTimestampBatteryStatus = millis();
        }
        break;
    }

    /* Publis state topic */
    g_mqtt.publish( topic, payload, true );

    free( topic );
    free( payload );
}


/*******************************************************************************
 *
 * @brief   Execute the current task
 * 
 */
void HomeAssistant::runTasks() {

    /* update sensors */
    if( g_mqtt.connected() == true ) {
        
        this->updateSensor( SENSOR_ID_CONN_RSSI );
        this->updateSensor( SENSOR_ID_BATTERY_CHARGE );
        this->updateSensor( SENSOR_ID_BATTERY_STATUS );
        this->updateSensor( SENSOR_ID_BATTERY_VOLT );
    }

    /* If connection to broker is lost, start wait connect task */
    if( g_mqtt.connected() == false && this->getCurrentTask() != TASK_HOMEASSISTANT_WAIT_MQTT_CONNECT ) {

        this->startTask( TASK_HOMEASSISTANT_WAIT_MQTT_CONNECT );
    }

    switch( this->getCurrentTask() ) {

        /* Waiting for MQTT connection to complete */
        case TASK_HOMEASSISTANT_WAIT_MQTT_CONNECT: {

            /* Still waiting for broker connection */
            if( g_mqtt.connected() == false ) {
                return;
            }

            /* Publish the configuration of all sensors */
            this->beginSendSensorConfig();

        }
        break;


        /* Publishing sensor configuration  */
        case TASK_HOMEASSISTANT_SEND_SENSOR_CONFIG: {

            /* Wait for current sensor config publish to complete */
            if (g_mqtt.isBusy() == true ) {
                return;
            }

            _taskCurrentSensorID++;

            /* End of sensor list */
            if( _taskCurrentSensorID > MAX_SENSORS_ID ) {
                this->endTask( TASK_SUCCESS );

                /* Force publish all sensors states */
                this->updateAllSensors( true );
                return;
            }

            /* Publish the configuration of the next sensor in the list */
            this->sendNextSensorConfig();
        }
        break;


        /* Publishing sensor states  */
        case TASK_HOMEASSISTANT_SEND_SENSOR_STATES: {

            /* Wait for current sensor state publish to complete */
            if (g_mqtt.isBusy() == true ) {
                return;
            }

            _taskCurrentSensorID++;

            /* End of sensor list */
            if( _taskCurrentSensorID > MAX_SENSORS_ID ) {
                this->endTask( TASK_SUCCESS );
                return;
            }

            /* Publish the state of the next sensor in the list */
            this->sendNextSensorState();

        }
        break;


        /* No current task running */
        case TASK_NONE: {
            
            if( g_mqtt.connected() == true ) {

                /* Publish states of sensors that changed in the sensor list */
                this->beginSendSensorStates();
            }
        }
        break;
    }
}


void handleHassTopicCallback( char* topic, size_t topicLength, char* payload, size_t payloadLength, bool retain ) {

    char* topic_cmp;

    /* Allocate memory for the topic compare. */
    topic_cmp = ( char* )malloc( topicLength + 1 );
    if( topic_cmp == nullptr ) {
        return;
    }

    /* Check if received topic is for sensor ID CMD_LCD_MSG */
    snprintf_P( topic_cmp, topicLength + 1, S_TOPIC_CMD_LCD_MSG, g_config.network.discovery_prefix, g_homeassistant.getDeviceID() );
    if( memcmp( topic, topic_cmp, topicLength ) == 0 ) {

        utf8ToLcdCharset( payload, payloadLength );

        /* Update LCD message */
        memset( g_homeassistant.lcd_message, 0, MAX_PAYLOAD_LCD_MESSAGE_LENGTH + 1 );
        strncpy( g_homeassistant.lcd_message, payload, payloadLength );

        if( g_screen.getId() == SCREEN_ID_ROOT ) {
            g_screen.requestScreenUpdate( false );
        }
    }

    free( topic_cmp );

}