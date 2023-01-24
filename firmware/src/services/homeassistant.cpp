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



/*******************************************************************************
 *
 * @brief   Class initializer
 *
 */
HomeAssistant::HomeAssistant() {
    _will_topic[0] = 0;
    _will_payload[0] = 0;
    _taskCurrentSensorID = SENSOR_ID_NONE;
    _prevRssi = -120;

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

    g_mqtt.setWillMessage( _will_topic, _will_payload, true, true );

    /* Start task waiting for the MQTT client to conenct to the broker */
    this->startTask( TASK_HOMEASSISTANT_WAIT_MQTT_CONNECT );
   
}


/*******************************************************************************
 *
 * @brief   Schedule all sensors to send update to home assistant
 *
 */
void HomeAssistant::updateAllSensors() {
    uint8_t i;
    for( i = 0; i < MAX_SENSORS_ID; i++ ) {

        _sensorNeedUpdate[ i ] = true;
    }
}


/*******************************************************************************
 *
 * @brief   Schedule a specific sensor to send update to home assistant
 *
 */
void HomeAssistant::updateSensor( uint8_t sensorID ) {
    if( sensorID > MAX_SENSORS_ID || sensorID == 0 ) {
        return;
    }

    _sensorNeedUpdate[ sensorID - 1 ] = true;
}


/*******************************************************************************
 *
 * @brief   Begin sending each sensors config payload
 *
 */
void HomeAssistant::beginSendSensorConfig() {

    if( g_mqtt.connected() == false ) {
        return;
    }

    this->startTask( TASK_HOMEASSISTANT_SEND_SENSOR_CONFIG );

    _taskCurrentSensorID = SENSOR_ID_NONE;
}


/*******************************************************************************
 *
 * @brief   Begin sending sensors state which are flagged for update
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


    char *topic, *payload;
    size_t topic_len, payload_len;

    switch( _taskCurrentSensorID ) {

        /* These sensors don't need config topic, skip */
        case SENSOR_ID_AVAILABILITY:
        case SENSOR_ID_NEXT_ALARM_AVAILABLE:
            return;

        case SENSOR_ID_ALARM_SWITCH:
            topic_len = strlen_P( S_TOPIC_ALARM_SWITCH_CONFIG ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_ALARM_SWITCH ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;

        case SENSOR_ID_NEXT_ALARM:
            topic_len = strlen_P( S_TOPIC_NEXT_ALARM_CONFIG ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_NEXT_ALARM ) + ( strlen( g_config.network.discovery_prefix ) * 3 ) + ( MAX_HA_DEVICE_ID_LENGTH * 5 ) + 1;
            break;

        case SENSOR_ID_CONN_RSSI:
            topic_len = strlen_P( S_TOPIC_CONN_RSSI_CONFIG ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = strlen_P( S_JSON_CONFIG_CONN_RSSI ) + ( strlen( g_config.network.discovery_prefix ) * 2 ) + ( MAX_HA_DEVICE_ID_LENGTH * 4 ) + 1;
            break;
    }


    topic = ( char* )malloc( topic_len );
    payload = ( char* )malloc( payload_len );

    if( topic == nullptr || payload == nullptr ) {
        return;
    }

    switch( _taskCurrentSensorID ) {

        /* Alarm switch sensor config */
        case SENSOR_ID_ALARM_SWITCH: {

            snprintf_P( topic, topic_len, S_TOPIC_ALARM_SWITCH_CONFIG, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_ALARM_SWITCH, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;

        /* Next alarm timestamp sensor config */
        case SENSOR_ID_NEXT_ALARM: {

            snprintf_P( topic, topic_len, S_TOPIC_NEXT_ALARM_CONFIG, g_config.network.discovery_prefix, _ha_device_id );
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

            snprintf_P( topic, topic_len, S_TOPIC_CONN_RSSI_CONFIG, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_JSON_CONFIG_CONN_RSSI, 
                _ha_device_id,
                g_config.network.discovery_prefix, _ha_device_id, 
                g_config.network.discovery_prefix, _ha_device_id, 
                _ha_device_id );
        }
        break;
    }

    /* Publish configuration topic */
    g_mqtt.publish( topic, payload, true );

    free( topic );
    free( payload );
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
            topic_len = strlen_P( S_TOPIC_NEXT_ALARM_AVAIL ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_SWITCH_STATE_LENGTH + 1;
            break;

        case SENSOR_ID_ALARM_SWITCH:
            topic_len = strlen_P( S_TOPIC_ALARM_SWITCH_STATE ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_SWITCH_STATE_LENGTH + 1;
            break;

        case SENSOR_ID_NEXT_ALARM:
            topic_len = strlen_P( S_TOPIC_NEXT_ALARM_STATE ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_TIMESTAMP_STATE_LENGTH + 1;
            break;

        case SENSOR_ID_CONN_RSSI:
            topic_len = strlen_P( S_TOPIC_CONN_RSSI_STATE ) + strlen( g_config.network.discovery_prefix ) + MAX_HA_DEVICE_ID_LENGTH + 1;
            payload_len = MAX_PAYLOAD_RSSI_STATE_LENGTH + 1;
            break;
    }

    topic = ( char* )malloc( topic_len );
    payload = ( char* )malloc( payload_len );

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

            snprintf_P( topic, topic_len, S_TOPIC_ALARM_SWITCH_STATE, g_config.network.discovery_prefix, _ha_device_id );
            strncpy_P( payload, ( g_alarm.isAlarmSwitchOn() ? S_PAYLOAD_SWITCH_ON : S_PAYLOAD_SWITCH_OFF ), payload_len );
        }
        break;

        /* Next alarm timestamp */
        case SENSOR_ID_NEXT_ALARM: {

            snprintf_P( topic, topic_len, S_TOPIC_NEXT_ALARM_STATE, g_config.network.discovery_prefix, _ha_device_id );

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
            snprintf_P( topic, topic_len, S_TOPIC_NEXT_ALARM_AVAIL, g_config.network.discovery_prefix, _ha_device_id );
            strncpy_P( payload, (( g_alarm.isAlarmEnabled() == true ) ? S_PAYLOAD_SWITCH_ON : S_PAYLOAD_SWITCH_OFF ), payload_len );
        }
        break;

        /* WiFi signal strength sensor */
        case SENSOR_ID_CONN_RSSI: {
            
            snprintf_P( topic, topic_len, S_TOPIC_CONN_RSSI_STATE, g_config.network.discovery_prefix, _ha_device_id );
            snprintf_P( payload, payload_len, S_PAYLOAD_INTEGER, g_wifi.getRSSI() );
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

    if( g_wifi.getRSSI() != _prevRssi ) {
        _prevRssi = g_wifi.getRSSI();

        this->updateSensor( SENSOR_ID_CONN_RSSI );
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
                this->updateAllSensors();
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