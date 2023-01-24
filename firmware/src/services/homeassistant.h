//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/homeassistant.h
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
#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H


#include <Arduino.h>
#include <version.h>
#include <itask.h>
#include <time.h>
#include "resources.h"
#include "services/mqtt.h"
#include "config.h"
#include "alarm.h"



/* Sensors state types maximum length */
#define MAX_PAYLOAD_AVAILABILITY_LENGTH     7   /* Availability (online/offline) */
#define MAX_PAYLOAD_SWITCH_STATE_LENGTH     3   /* Switch state (on/off) */
#define MAX_PAYLOAD_TIMESTAMP_STATE_LENGTH  25  /* Timestamp (YYYY-MM-DDThh:mm:00+00:00) */
#define MAX_PAYLOAD_RSSI_STATE_LENGTH       4   /* RSSI value (max 3 digits inc. sign) */

/* Topic name maxmimum length */
#define MAX_WILL_TOPIC_LENGTH               15 + MAX_HA_DEVICE_ID_LENGTH + MAX_DISCOVERY_PREFIX_LENGTH

/* Maximum number of sensors */
#define MAX_SENSORS_ID      SENSOR_ID_AVAILABILITY

/* Sensors ID's */
enum HASensorsIDs {
    SENSOR_ID_NONE = 0,
    SENSOR_ID_NEXT_ALARM = 1,
    SENSOR_ID_NEXT_ALARM_AVAILABLE,
    SENSOR_ID_ALARM_SWITCH,
    SENSOR_ID_CONN_RSSI,
    SENSOR_ID_AVAILABILITY
};

/* Class tasks ID's */
enum {
    TASK_HOMEASSISTANT_WAIT_MQTT_CONNECT = 1,
    TASK_HOMEASSISTANT_SEND_SENSOR_CONFIG,
    TASK_HOMEASSISTANT_SEND_SENSOR_STATES,
};

/* Sendor states format */
PROG_STR( S_PAYLOAD_AVAIL_ONLINE,       "online" );
PROG_STR( S_PAYLOAD_AVAIL_OFFLINE,      "offline" );
PROG_STR( S_PAYLOAD_SWITCH_ON,          "ON" );
PROG_STR( S_PAYLOAD_SWITCH_OFF,         "OFF" );
PROG_STR( S_PAYLOAD_INTEGER,            "%d" );
PROG_STR( S_PAYLOAD_TIMESTAMP,          "%4d-%02d-%02dT%02d:%02d:00+00:00" );

/* node ID format */
#define MAX_HA_DEVICE_ID_LENGTH         12
PROG_STR( S_HA_ID_FORMAT,               "%02x%02x%02x%02x%02x%02x" );

/* Sendor topic names */
PROG_STR( S_TOPIC_ALARM_SWITCH_CONFIG,  "%s/binary_sensor/%s/clock_alarm_switch/config" );
PROG_STR( S_TOPIC_ALARM_SWITCH_STATE,   "%s/binary_sensor/%s/clock_alarm_switch/state" );
PROG_STR( S_TOPIC_NEXT_ALARM_CONFIG,    "%s/sensor/%s/clock_next_alarm/config" );
PROG_STR( S_TOPIC_NEXT_ALARM_STATE,     "%s/sensor/%s/clock_next_alarm/state" );
PROG_STR( S_TOPIC_NEXT_ALARM_AVAIL,     "%s/sensor/%s/clock_next_alarm/available" );
PROG_STR( S_TOPIC_CONN_RSSI_CONFIG,     "%s/sensor/%s/clock_rssi/config" );
PROG_STR( S_TOPIC_CONN_RSSI_STATE,      "%s/sensor/%s/clock_rssi/state" );
PROG_STR( S_TOPIC_AVAILABILITY,         "%s/sensor/%s/status" );

/* Sendor configuration topics payload */
PROG_STR( S_JSON_CONFIG_ALARM_SWITCH,   "{\"name\":\"Alarm clock Alarm switch\"," \
                                        "\"uniq_id\":\"clock_%s_alarm_switch\"," \
                                        "\"stat_t\":\"%s/binary_sensor/%s/clock_alarm_switch/state\"," \
                                        "\"avty_t\": \"%s/sensor/%s/status\"," \
                                        "\"ic\":\"mdi:bell-ring-outline\"," \
                                        "\"dev\":{" \
                                        "\"ids\":[\"%s\"]," \
                                        "\"name\":\"Alarm clock\"," \
                                        "\"mf\":\"bfrigon.com\"," \
                                        "\"mdl\":\"Clock V3\"," \
                                        "\"sw\":\"" FIRMWARE_VERSION "\"" \
                                        "}}" );

PROG_STR( S_JSON_CONFIG_NEXT_ALARM,     "{\"name\":\"Alarm clock Next alarm\"," \
                                        "\"uniq_id\":\"clock_%s_next_alarm\"," \
                                        "\"dev_cla\":\"timestamp\"," \
                                        "\"stat_t\":\"%s/sensor/%s/clock_next_alarm/state\"," \
                                        "\"availability\":[" \
                                        "{\"topic\":\"%s/sensor/%s/status\"}," \
                                        "{\"topic\":\"%s/sensor/%s/clock_next_alarm/available\",\"pl_avail\":\"ON\",\"pl_not_avail\":\"OFF\"}]," \
                                        "\"avty_mode\":\"all\", " \
                                        "\"ic\":\"mdi:alarm\"," \
                                        "\"dev\":{" \
                                        "\"ids\":[\"%s\"]" \
                                        "}}" );

PROG_STR( S_JSON_CONFIG_CONN_RSSI,      "{\"name\":\"Alarm clock Signal strength\"," \
                                        "\"uniq_id\":\"clock_%s_rssi\"," \
                                        "\"dev_cla\":\"signal_strength\"," \
                                        "\"unit_of_meas\":\"dBm\"," \
                                        "\"ent_cat\":\"diagnostic\", " \
                                        "\"stat_t\":\"%s/sensor/%s/clock_rssi/state\"," \
                                        "\"avty_t\": \"%s/sensor/%s/status\"," \
                                        "\"ic\":\"mdi:signal\"," \
                                        "\"dev\":{" \
                                        "\"ids\":[\"%s\"]" \
                                        "}}" );



/*******************************************************************************
 *
 * @brief   Home assistant client
 * 
 * @details Publish sensors states (alarm state, next alarm time ) to 
 *          home assistant via a MQTT broker
 * 
 *******************************************************************************/
class HomeAssistant : public ITask {
  public:

    HomeAssistant();

    void begin();    
    void runTasks();
    void updateSensor( uint8_t sensorID );
    void updateAllSensors();

  private:

    void beginSendSensorConfig();
    void sendNextSensorConfig();
    void beginSendSensorStates();
    void sendNextSensorState();
    
    uint8_t _taskCurrentSensorID;                               /* Current sensor ID for the publish config or state task */
    char _will_topic[ MAX_WILL_TOPIC_LENGTH + 1 ];              /* WILL message topic buffer */
    char _will_payload[ MAX_PAYLOAD_AVAILABILITY_LENGTH + 1 ];  /* WILL message payload buffer */
    char _ha_device_id[ MAX_HA_DEVICE_ID_LENGTH + 1 ];          /* Home assistant device ID */
    bool _sensorNeedUpdate[ MAX_SENSORS_ID ];                   /* Sensors need update flag */
    int8_t _prevRssi;                                           /* Previously sent WiFi RSSI value */
};

extern HomeAssistant g_homeassistant;

#endif /* HOMEASSISTANT_H */