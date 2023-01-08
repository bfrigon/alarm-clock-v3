//******************************************************************************
//
// Project : Alarm Clock V3
// File    : include/resources.h
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
#ifndef RESOURCES_H
#define RESOURCES_H

#include <avr/pgmspace.h>


//--------------------------------------------------------------------------
//
// Macro
//
//--------------------------------------------------------------------------

/* Declare a string in program memory */
#define PROG_STR(name, value) \
    const char name[] PROGMEM = value


//--------------------------------------------------------------------------
//
// Custom character sets
//
//--------------------------------------------------------------------------

const unsigned char CUSTOM_CHARACTERS_DEFAULT[] PROGMEM = {

    0x00, 0x00, 0x04, 0x0A, 0x04, 0x00, 0x00, 0x00,     /* unchecked */
    0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00, 0x00,     /* checked */
    0xC0, 0xC8, 0xCC, 0xCE, 0xCC, 0xC8, 0xC0, 0xC0,     /* selection (flashing) */
    0xC0, 0xC2, 0xC6, 0xCE, 0xC6, 0xC2, 0xC0, 0xC0,     /* selection, reverse (flashing) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* --Unused-- */
    0x0A, 0x00, 0x08, 0x00, 0x08, 0x00, 0x0A, 0x00,     /* Field begin marker */
    0x0A, 0x00, 0x02, 0x00, 0x02, 0x00, 0x0A, 0x00,     /* Field end marker */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* --Unused-- */
};

const unsigned char CUSTOM_CHARACTERS_ROOT[] PROGMEM = {
    0x00, 0x01, 0x01, 0x05, 0x05, 0x15, 0x00, 0x00,     /* Wifi connected */
    0x06, 0x0E, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00,     /* NO SD Card */
    0x00, 0x0E, 0x0A, 0x0E, 0x00, 0x00, 0x00, 0x00,     /* Degree */
    0x06, 0x09, 0x09, 0x09, 0x09, 0x0F, 0x00, 0x00,     /* Battery : Empty */
    0x06, 0x09, 0x09, 0x0F, 0x0F, 0x0F, 0x00, 0x00,     /* Battery : Half */
    0x06, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00,     /* Battery : Full */
    0x07, 0x0E, 0x1C, 0x06, 0x0C, 0x18, 0x00, 0x00,     /* Charging */
    0x04, 0x0E, 0x0E, 0x04, 0x00, 0x04, 0x00, 0x00      /* No battery */
};




//--------------------------------------------------------------------------
//
// Progmem strings
//
//--------------------------------------------------------------------------
PROG_STR( S_PROFILE_DEF_FILENAME,       "*Default*" );
PROG_STR( S_DEFAULT_HOSTNAME,           "clock-v3" );
PROG_STR( S_DEFAULT_NTPSERVER,          "pool.ntp.org" );

/* Dialog strings */
PROG_STR( S_QUESTION_SAVE,              "Apply ?" );
PROG_STR( S_YES,                        "Yes" );
PROG_STR( S_NO,                         "No" );
PROG_STR( S_SEPARATOR,                  ": " );

PROG_STR( S_ON,                         "On" );
PROG_STR( S_OFF,                        "Off" );
PROG_STR( S_ENABLED,                    "Enabled" );
PROG_STR( S_DISABLED,                   "Disabled" );
PROG_STR( S_AM,                         "am" );
PROG_STR( S_PM,                         "pm" );
PROG_STR( S_DOW,                        "SMTWTFS" );
PROG_STR( S_ALARMS_OFF,                 "Alarms off" );
PROG_STR( S_ALARM_IN,                   "Alarm #%d in" );
PROG_STR( S_ALARM_NUM1,                 "1." );
PROG_STR( S_ALARM_NUM2,                 "2." );

/* Main menu strings */
PROG_STR( S_MAIN_MENU_SET_ALARMS,       "Set alarms" );
PROG_STR( S_MAIN_MENU_SET_TIME,         "Set time" );
PROG_STR( S_MAIN_MENU_PROFILES,         "Profiles" );
PROG_STR( S_MAIN_MENU_DISPLAY,          "Display" );
PROG_STR( S_MAIN_MENU_LAMP,             "Night lamp" );
PROG_STR( S_MAIN_MENU_TIMEZONE,         "Set timezone" );
PROG_STR( S_MAIN_MENU_NETWORK,          "Network" );
PROG_STR( S_MAIN_MENU_SERVICES,         "Services" );
PROG_STR( S_MAIN_MENU_SETTINGS,         "Settings" );

/* Service menu */
PROG_STR( S_MENU_SERVICE_TIME_AUTOSYNC, "NTP Sync" );
PROG_STR( S_MENU_SERVICE_TELNET,        "Telnet  " );

/* Network menu srings */
PROG_STR( S_MENU_NETWORK_STATUS,        "Network status" );
PROG_STR( S_MENU_NETWORK_DHCP,          "DHCP" );
PROG_STR( S_MENU_NETWORK_IP,            "IP Address" );
PROG_STR( S_MENU_NETWORK_MASK,          "Mask" );
PROG_STR( S_MENU_NETWORK_GATEWAY,       "Gateway" );
PROG_STR( S_MENU_NETWORK_DNS,           "DNS" );
PROG_STR( S_MENU_NETWORK_HOSTNAME,      "Hostname" );

/* Settings menu strings */
PROG_STR( S_MENU_SETTINGS_BACKUP,       "Backup" );
PROG_STR( S_MENU_SETTINGS_RESTORE,      "Restore" );
PROG_STR( S_MENU_SETTINGS_RESET,        "Factory reset" );
PROG_STR( S_MENU_SETTINGS_BATT_INFO,    "Battery status" );

/* Display settings menu strings */
PROG_STR( S_MENU_SETTINGS_24H,          "24H format" );
PROG_STR( S_MENU_SETTINGS_COLOR,        "Color" );
PROG_STR( S_MENU_SETTINGS_BRIGHT,       "Brightness" );
PROG_STR( S_MENU_SETTINGS_LCD_CTR,      "LCD contrast" );
PROG_STR( S_MENU_SETTINGS_DATE_FMT,     "Date format" );
PROG_STR( S_MENU_SETTINGS_ALS_PRESET,   "ALS preset");

/* List profile screen */
PROG_STR( S_EDIT_ALARM_1,               "Alarm #1..." );
PROG_STR( S_EDIT_ALARM_2,               "Alarm #2..." );

/* Edit profile menu */
PROG_STR( S_EDIT_PROFILE_FILENAME,      "Sound file" );
PROG_STR( S_EDIT_PROFILE_SNOOZE,        "Snooze delay" );
PROG_STR( S_EDIT_PROFILE_VOLUME,        "Volume" );
PROG_STR( S_EDIT_PROFILE_GRADUAL,       "Gradual" );
PROG_STR( S_EDIT_PROFILE_VISUAL,        "Visual effect" );
PROG_STR( S_EDIT_PROFILE_LAMP,          "Alarm lamp" );
PROG_STR( S_EDIT_PROFILE_MESSAGE,       "Message" );
PROG_STR( S_EDIT_PROFILE_TEST,          "Test..." );
PROG_STR( S_EDIT_PROFILE_VISUAL_MODE,   "Effect type" );
PROG_STR( S_EDIT_PROFILE_VISUAL_SPEED,  "Speed" );

/* Night lamp config menu */
PROG_STR( S_NIGHT_LAMP_COLOR,           "Color" );
PROG_STR( S_NIGHT_LAMP_BRIGHTNESS,      "Brightness" );
PROG_STR( S_NIGHT_LAMP_EFFECT,          "Mode" );
PROG_STR( S_NIGHT_LAMP_SPEED,           "Effect speed" );
PROG_STR( S_NIGHT_LAMP_DELAY,           "Delay" );


PROG_STR( S_TEST_ALARM,                 "Alarm test..." );
PROG_STR( S_ALARM_MSG_MORNING,          "Good morning" );
PROG_STR( S_ALARM_MSG_AFTERNOON,        "Good afternoon" );
PROG_STR( S_ALARM_MSG_EVENING,          "Good evening" );

PROG_STR( S_SNOOZE,                     "Snooze" );


PROG_STR( S_INSTR_CANCEL_ALARM_1,       "Hold <Alarm> to" );
PROG_STR( S_INSTR_CANCEL_ALARM_2,       "cancel..." );


PROG_STR( S_CONFIRM_SETTINGS_RESTORE,   "Restore?" );
PROG_STR( S_CONFIRM_SETTINGS_RESET,     "Factory reset?" );
PROG_STR( S_CONFIRM_OVERWRITE,          "Overwrite file?" );

PROG_STR( S_STATUS_SAVING,              "Saving..." );
PROG_STR( S_STATUS_RESTORING,           "Restoring..." );
PROG_STR( S_STATUS_DONE,                "Done" );
PROG_STR( S_STATUS_ERROR_NO_SDCARD,     "No SD card!" );
PROG_STR( S_STATUS_ERROR_WRITE,         "Write error!" );
PROG_STR( S_STATUS_ERROR_NOTFOUND,      "File not found!" );
PROG_STR( S_STATUS_ERROR_READ,          "Read error!" );
PROG_STR( S_STATUS_ERROR_UNKNOWN,       "Unknown error!" );


/* Battery status screen */
PROG_STR( S_BATT_VOLTAGE,               "%d.%03d V" );
PROG_STR( S_BATT_CAPACITY,              "%d mAh (%d%%)" );
PROG_STR( S_BATT_SOH,                   "SOH : %d%%" );
PROG_STR( S_BATT_FULL_CAP,              "FCap: %d mAh" );
PROG_STR( S_BATT_CURRENT,               "Cur: %d mA" );
PROG_STR( S_BATT_POWER,                 "Pwr: %d mW" );
PROG_STR( S_BATT_NO_VOLTAGE,            "*.*** V" );
PROG_STR( S_BATT_NO_CAPACITY,           "*** mAh (n/a)" );
PROG_STR( S_BATT_NO_FULL_CAP,           "FCap: n/a" );
PROG_STR( S_BATT_NO_SOH,                "SOH : n/a" );
PROG_STR( S_BATT_NO_CURRENT,            "Cur: n/a" );
PROG_STR( S_BATT_NO_POWER,              "Pwr: n/a" );

/* Network status screen */
PROG_STR( S_NETINFO_IP,                 "%d.%d.%d.%d" );
PROG_STR( S_SSID,                       "SSID" );

PROG_STR( S_CONSOLE_CONNECT_REFUSE,     "Another telnet session is active.\r\n" );

PROG_STR( S_CONSOLE_WELCOME,            "Alarm clock V3 console (fw date: " __DATE__ ")\r\n" 
                                        "https://github.com/bfrigon/alarm-clock-v3\r\n"
                                        "\r\n"
                                        "Type 'help' for a list of commands" );

PROG_STR( S_CONSOLE_GOODBYE,            "Bye!" );
PROG_STR( S_CONSOLE_TIMEOUT,            "Session timeout" );
PROG_STR( S_CONSOLE_INIT,               "Initializing..." );

PROG_STR( S_CONSOLE_BUSY,               "Console is busy running another task!" );
PROG_STR( S_CONSOLE_WIFI_BUSY,          "WiFi manager is busy running another request." );
PROG_STR( S_CONSOLE_PROMPT,             "(%s) > " );
PROG_STR( S_CONSOLE_USAGE,              "Usage : " );
PROG_STR( S_CONSOLE_APPLY,              "Apply settings? " );
PROG_STR( S_CONSOLE_DONE,               "Done!" );
PROG_STR( S_CONSOLE_CONTINUE,           "Do you want to continue? " );
PROG_STR( S_CONSOLE_UNKNOWN_ERROR,      "Unknown error! (%d)" );
PROG_STR( S_CONSOLE_SUCCESS,            "Success" );
PROG_STR( S_CONSOLE_NONE,               "None" );
PROG_STR( S_CONSOLE_FREEMEM,            "Free SRAM  : %hd bytes" );
PROG_STR( S_CONSOLE_TOTALMEM,           "Total SRAM : %hd bytes" );
PROG_STR( S_CONSOLE_INVALID_COMMAND,    "Invalid command!" );
PROG_STR( S_CONSOLE_INVALID_INPUT_BOOL, "Invalid input! Enter 'Y' or 'N'" );
PROG_STR( S_CONSOLE_INVALID_INPUT_IP,   "Invalid IP address" );
PROG_STR( S_CONSOLE_INVALID_INPUT_SUB,  "Invalid subnet mask" );
PROG_STR( S_CONSOLE_MISSING_PARAMETER,  "Missing parameter!" );
PROG_STR( S_CONSOLE_NET_CONNECTED,      "Connected" );
PROG_STR( S_CONSOLE_NET_DISCONNECTED,   "Disconnected" );
PROG_STR( S_CONSOLE_NET_NOT_CONNECTED,  "Not connected to the WiFi network" );
PROG_STR( S_CONSOLE_NET_ALREADY_CONN,   "Already connected to the WiFi network" );
PROG_STR( S_CONSOLE_NET_RECONNECTING,   "Re-connecting to %s..." );
PROG_STR( S_CONSOLE_NET_CONNECTING,     "Connecting to %s..." );
PROG_STR( S_CONSOLE_NET_CONN_FAIL,      "Unable to connect" );
PROG_STR( S_CONSOLE_NET_INVALID_HOST,   "Invalid hostname" );
PROG_STR( S_CONSOLE_NET_DNS_QUERY,      "Resolving %s..." );
PROG_STR( S_CONSOLE_NET_PING_HOSTNAME,  "Pinging %s..." );
PROG_STR( S_CONSOLE_NET_PING_IP,        "Pinging %d.%d.%d.%d" );
PROG_STR( S_CONSOLE_NET_PING_RESULT,    "Reply from %d.%d.%d.%d time=%dms" );
PROG_STR( S_CONSOLE_NET_PING_TIMEOUT,   "Ping timeout" );
PROG_STR( S_CONSOLE_NET_PING_UNREACH,   "Destination network unreachable" );
PROG_STR( S_CONSOLE_NET_PING_ERROR,     "Ping error" );
PROG_STR( S_CONSOLE_NET_PING_UNKNOWN,   "Unknown host" );

PROG_STR( S_CONSOLE_NET_STATUS,         "Status           : " );
PROG_STR( S_CONSOLE_NET_DHCP,           "DHCP             : " );
PROG_STR( S_CONSOLE_NET_SSID,           "SSID             : %s" );
PROG_STR( S_CONSOLE_NET_IP,             "Local IP address : %d.%d.%d.%d" );
PROG_STR( S_CONSOLE_NET_MASK,           "Subnet mask      : %d.%d.%d.%d" );
PROG_STR( S_CONSOLE_NET_GATEWAY,        "Gateway          : %d.%d.%d.%d" );
PROG_STR( S_CONSOLE_NET_DNS,            "DNS              : %d.%d.%d.%d" );

PROG_STR( S_CONSOLE_NSLOOKUP_RESULT,    "Address: %d.%d.%d.%d" );
PROG_STR( S_CONSOLE_NET_CFG_INSTR,      "Network settings\r\nLeave the field empty to keep existing settings." );
PROG_STR( S_CONSOLE_NET_CFG_SSID,       "SSID (%s): " );
PROG_STR( S_CONSOLE_NET_CFG_KEY,        "Password: " );
PROG_STR( S_CONSOLE_NET_CFG_DHCP,       "Use DHCP (%s): " );
PROG_STR( S_CONSOLE_NET_CFG_IP,         "Local IP address (%d.%d.%d.%d): " );
PROG_STR( S_CONSOLE_NET_CFG_SUBNET,     "Subnet mask (%d.%d.%d.%d): " );
PROG_STR( S_CONSOLE_NET_CFG_GATEWAY,    "Gateway address (%d.%d.%d.%d): " );
PROG_STR( S_CONSOLE_NET_CFG_DNS,        "DNS (%d.%d.%d.%d): " );
PROG_STR( S_CONSOLE_NET_CFG_HOSTNAME,   "Hostname (%s): " );
PROG_STR( S_CONSOLE_NET_CFG_NTPSERVER,  "NTP server address (%s): " );
PROG_STR( S_CONSOLE_NET_CFG_APPLY,      "Apply network settings? " );

PROG_STR( S_CONSOLE_TIME_SET_INSTR,     "Set date/time\r\nLeave the field empty to keep existing settings.");
PROG_STR( S_CONSOLE_TIME_CFG_TIME,      "Time (%02d:%02d): " );
PROG_STR( S_CONSOLE_TIME_CFG_DATE,      "Date (%d-%02d-%02d): " );
PROG_STR( S_CONSOLE_TIME_CFG_NTP,       "Syncrhonise clock automatically using NTP (%s): " );
PROG_STR( S_CONSOLE_TIME_CFG_APPLY,     "Apply clock settings? " );
PROG_STR( S_CONSOLE_TIME_CURRENT_TZ,    "Current time zone : " );
PROG_STR( S_CONSOLE_TIME_NEW_TZ,        "New time zone : " );
PROG_STR( S_CONSOLE_TIME_EXIT_MENU,     "Leave empty to keep current timezone" );
PROG_STR( S_CONSOLE_TIME_ENTER_TZ_NAME, "Enter new time zone name or select region : " );
PROG_STR( S_CONSOLE_TIME_SELECT_TZ,     "Select time zone : " );
PROG_STR( S_CONSOLE_TIME_INVALID_TZ,    "Invalid time zone" );
PROG_STR( S_CONSOLE_TIME_INVALID_SEL,   "Invalid selection" );
PROG_STR( S_CONSOLE_TIME_INSTR_RETURN,  " 0) Return to the previous menu" );
PROG_STR( S_CONSOLE_INVALID_DATE_FMT,   "Invalid date! (expects YYYY-MM-DD)" );
PROG_STR( S_CONSOLE_INVALID_TIME_FMT,   "Invalid time! (expects HH:MM)" );
PROG_STR( S_CONSOLE_TZ_IS_DST,          "Currently on daylight saving time: %S");
PROG_STR( S_CONSOLE_TZ_NO_DST,          "Daylight saving time not observed");
PROG_STR( S_CONSOLE_TZ_EQUAL_UTC,       "Local time is the same as UTC/GMT");
PROG_STR( S_CONSOLE_TZ_BEHIND_UTC_PRE,  "Local time is ");
PROG_STR( S_CONSOLE_TZ_BEHIND_UTC,      " behind UTC/GMT");
PROG_STR( S_CONSOLE_TZ_AHEAD_UTC_PRE,   "Local time is ");
PROG_STR( S_CONSOLE_TZ_AHEAD_UTC,       " ahead of UTC/GMT");

PROG_STR( S_CONSOLE_TZ_DST_TRANS,       "Daylight saving time starts on %S, %S %d %d\r\nAt %02d:%02d local time, turn clock forward ");
PROG_STR( S_CONSOLE_TZ_STD_TRANS,       "Daylight saving time ends on %S, %S %d %d\r\nAt %02d:%02d local time, turn clock back ");

PROG_STR( S_CONSOLE_CFG_SAVING,         "Saving settings..." );
PROG_STR( S_CONSOLE_CFG_RESTORING,      "Restoring settings..." );
PROG_STR( S_CONSOLE_CFG_RESETTING,      "Restoring default settings..." );
PROG_STR( S_CONSOLE_CFG_SAVE_FILENAME,  "File name to save the settings to (default: 'config.txt') : " );
PROG_STR( S_CONSOLE_CFG_LOAD_FILENAME,  "File name to restore the settings from (default: 'config.txt') : " );
PROG_STR( S_CONSOLE_CFG_FILE_EXISTS,    "File '%s' already exists. Do you want to overwrite it? " );
PROG_STR( S_CONSOLE_CFG_RESTORE_MSG,    "WARNING! This will overwrite all current settings.");
PROG_STR( S_CONSOLE_CFG_RESET_MSG,      "WARNING! This will revert all settings to their default values. ");

PROG_STR( S_CONSOLE_NTP_SEND_FAIL,      "NTP packet send fail!");
PROG_STR( S_CONSOLE_NTP_INVALID_RESP,   "Invalid response from NTP server!");
PROG_STR( S_CONSOLE_NTP_NO_RESP,        "No response from NTP server!");
PROG_STR( S_CONSOLE_NTP_BIND_FAIL,      "Socket bind fail!");
PROG_STR( S_CONSOLE_NTP_UNKNOWN_HOST,   "The NTP server address could not be resolved!" );
PROG_STR( S_CONSOLE_NTP_LAST_SYNC,      "Last synchronized on : " );
PROG_STR( S_CONSOLE_NTP_NEXT_SYNC,      "Next synchronization in : " );
PROG_STR( S_CONSOLE_NTP_SYNC_WITH,      "Synchronize with %s" );
PROG_STR( S_CONSOLE_NTP_SENDING,        "Sending request (%d.%d.%d.%d)..." );
PROG_STR( S_CONSOLE_NTP_ADJUST,         "Adjusted clock %c%ld.%03ld seconds" );
PROG_STR( S_CONSOLE_NTP_SERVER,         "NTP server : %s" );
PROG_STR( S_CONSOLE_NTP_AUTOSYNC,       "Automatic clock synchronization : " );
PROG_STR( S_CONSOLE_NTP_LAST_ERROR,     "Previous sync error : " );

PROG_STR( S_CONSOLE_SERV_UNKNOWN,       "Unknown service name" );
PROG_STR( S_CONSOLE_SERV_INVALID_OPT,   "Invalid option" );

PROG_STR( S_CONSOLE_TELNET_ENABLED,     "Telnet console is enabled" );
PROG_STR( S_CONSOLE_TELNET_DISABLED,    "Telnet console is disabled" );
PROG_STR( S_CONSOLE_TELNET_SESS_ACTIVE, "Session active from %d.%d.%d.%d on port %hu" );
PROG_STR( S_CONSOLE_TELNET_NO_SESS,     "No active session" );


PROG_STR( S_CONSOLE_BATT_STATE,         "State    : " );
PROG_STR( S_CONSOLE_BATT_VOLTAGE,       "Voltage  : %d.%03d V" );
PROG_STR( S_CONSOLE_BATT_CHARGE,        "Charge   : %d%%" );
PROG_STR( S_CONSOLE_BATT_CAPACITY,      "Capacity : %d mAh / %d mAh" );
PROG_STR( S_CONSOLE_BATT_CURRENT,       "Current  : %d mA" );
PROG_STR( S_CONSOLE_BATT_SOH,           "SOH      : %d%%" );
PROG_STR( S_CONSOLE_BATT_STATE_CHG,     "Charging" );
PROG_STR( S_CONSOLE_BATT_STATE_DISCHG,  "Discharging" );
PROG_STR( S_CONSOLE_BATT_STATE_IDLE,    "Idle" );
PROG_STR( S_CONSOLE_BATT_STATE_UNKNOWN, "Unknown" );
PROG_STR( S_CONSOLE_BATT_STATE_MISSING, "No battery detected!" );

PROG_STR( S_LOG_REPEAT,                 " <- Occured %d times" );
PROG_STR( S_LOG_REPEAT_LIMIT,           " <- Occured more than 250 times!" );


PROG_STR( S_LOGMSG_UNKNOWN,                     "**Unknown log entry type!** (type: %d, flags: %ld)" );
PROG_STR( S_LOGMSG_RESET,                       "System reset (MCUSR: 0x%02X)" );
PROG_STR( S_LOGMSG_TELNET_SERVICE_ENABLED,      "Telnet server enabled" );
PROG_STR( S_LOGMSG_TELNET_SERVICE_DISABLED,     "Telnet server disabled" );
PROG_STR( S_LOGMSG_TELNET_SESSION_END,          "Telnet session ended" );
PROG_STR( S_LOGMSG_TELNET_SESSION_TIMEOUT,      "Telnet session timed out" );
PROG_STR( S_LOGMSG_TELNET_SESSION_START,        "Client started a telnet session from %d.%d.%d.%d" );
PROG_STR( S_LOGMSG_WIFI_CONNECTED,              "WiFi connected" );
PROG_STR( S_LOGMSG_WIFI_DISCONNECTED,           "WiFi disconnected" );
PROG_STR( S_LOGMSG_WIFI_FAIL_CONN_LOST,         "WiFi connection lost" );
PROG_STR( S_LOGMSG_WIFI_FAIL_CONN_TIMEOUT,      "Unable to connect to WiFi (Timeout)" );
PROG_STR( S_LOGMSG_WIFI_FAIL_CONN,              "Unable to connect to WiFi" );
PROG_STR( S_LOGMSG_CON_ADJ_TIME,                "Date/Time was adjusted from the console" );
PROG_STR( S_LOGMSG_CON_ADJ_TZ,                  "Timezone was changed from the console to \"%S\"" );
PROG_STR( S_LOGMSG_NTP_FAIL_NO_WIFI,            "NTP sync failed (not connected to WiFi)" );
PROG_STR( S_LOGMSG_NTP_FAIL_SOCKET_ERR,         "NTP sync failed (socket error)" );
PROG_STR( S_LOGMSG_NTP_FAIL_CANT_RESOLVE_HOST,  "NTP sync failed (cannot resolve host)" );
PROG_STR( S_LOGMSG_NTP_FAIL_NO_RESPONSE,        "NTP sync failed (no response from server)" );
PROG_STR( S_LOGMSG_NTP_FAIL_INVALID_RESPONSE,   "NTP sync failed (invalid response from server)" );
PROG_STR( S_LOGMSG_NTP_FAIL_SEND_PACKET,        "NTP sync failed (packet send failed!)" );
PROG_STR( S_LOGMSG_FACTORY_RESET,               "Settings was reset to factory defaults" );
PROG_STR( S_LOGMSG_CONFIG_RESTORED,             "Restored settings from SD card" );
PROG_STR( S_LOGMSG_POWER_ON_BATTERY,            "Main power lost, running on battery" );
PROG_STR( S_LOGMSG_POWER_RESTORED,              "Main power restored" );


//--------------------------------------------------------------------------
//
// Strings arrays
//
//--------------------------------------------------------------------------



#define MAX_ALS_PRESETS_NAMES   4
#define ALS_PRESET_NAME_LENGTH  8
const char _ALS_PRESET_NAMES[ MAX_ALS_PRESETS_NAMES ][ ALS_PRESET_NAME_LENGTH + 1 ] PROGMEM = {
    "Disabled", "Low", "Medium", "High"
};


#define MAX_DATE_FORMATS        8
#define DATE_FORMAT_LENGTH      14
const char _DATE_FORMATS[ MAX_DATE_FORMATS ][ DATE_FORMAT_LENGTH + 1 ] PROGMEM = {
    "dd/mm/yyyy",
    "mm/dd/yyyy",
    "yyyy/mm/dd",
    "dd-mmm-yyyy",
    "mmm-dd-yyyy",
    "yyyy-mmm-dd",
    "wd, mmm d",
    "wd, mmm d yyyy"
};

#define MAX_ALARM_VISUALS           6
#define ALARM_VISUAL_NAME_LENGTH    11
const char _ALARM_VISUAL[ MAX_ALARM_VISUALS ][ ALARM_VISUAL_NAME_LENGTH + 1 ] PROGMEM = {
    "None",
    "Flashing",
    "Fading",
    "Rainbow",
    "White flash",
    "Red flash"
};


#define MAX_ALARM_LAMP_MODES            5
#define ALARM_LAMP_MODES_NAME_LENGTH    8
const char _ALARM_LAMP_MODES[ MAX_ALARM_LAMP_MODES ][ ALARM_LAMP_MODES_NAME_LENGTH + 1 ] PROGMEM = {
    "OFF",
    "ON",
    "Flashing",
    "Fading",
    "Rainbow"
};


//--------------------------------------------------------------------------
//
// Display colors
//
//--------------------------------------------------------------------------

#define COLOR_TABLE_MAX_COLORS  13
#define COLOR_NAME_MAX_LENGTH   9

#define COLOR_RED       0
#define COLOR_ORANGE    1
#define COLOR_YELLOW    2
#define COLOR_LIME      3
#define COLOR_GREEN     4
#define COLOR_TURQUOISE 5
#define COLOR_CYAN      6
#define COLOR_COBALT    7
#define COLOR_BLUE      8
#define COLOR_VIOLET    9
#define COLOR_MAGENTA   10
#define COLOR_PINK      11
#define COLOR_WHITE     12

/* Clock display color names */
const char _COLOR_NAMES[ COLOR_TABLE_MAX_COLORS ][ COLOR_NAME_MAX_LENGTH + 1 ] PROGMEM = {
    "Red",
    "Orange",
    "Yellow",
    "Lime",
    "Green",
    "Turquoise",
    "Cyan",
    "Cobalt",
    "Blue",
    "Violet",
    "Magenta",
    "Pink",
    "White"
};

/* Clock display RGB color table */
const uint8_t _COLOR_TABLE[ COLOR_TABLE_MAX_COLORS ][ 3 ] PROGMEM = {
    0xFF, 0x00, 0x00,   /* Red */
    0xFF, 0x70, 0x00,   /* Orange */
    0xFF, 0xFF, 0x08,   /* Yellow */
    0xA0, 0xDF, 0x00,   /* Lime */
    0x00, 0xFF, 0x00,   /* Green */
    0x00, 0xFF, 0x60,   /* Turqoise */
    0x00, 0xFF, 0xFF,   /* Cyan */
    0x44, 0x88, 0xFF,   /* Cobalt */
    0x00, 0x00, 0xFF,   /* Blue */
    0xB7, 0x00, 0xFF,   /* Violet */
    0xFF, 0x00, 0xF0,   /* Magenta */
    0xDF, 0x00, 0x7F,   /* Pink */
    0xFF, 0xFF, 0xFF,   /* White */
};


//--------------------------------------------------------------------------
//
// Audio files
//
//--------------------------------------------------------------------------

#define DEFAULT_ALARMSOUND_DATA_LENGTH  1080

/* Default alarm clock sound, mp3 8kpbs mono 8kHz */
const uint8_t PROGMEM _DEFAULT_ALARMSOUND_DATA[] = {
    0xFF, 0xE3, 0x18, 0xC4, 0x00, 0x09, 0x18, 0x26, 0x19, 0x90, 0x08, 0xC4, 0x00, 0x00, 0x20, 0x6A,
    0x61, 0x43, 0xC0, 0x71, 0x47, 0x02, 0x34, 0x59, 0x20, 0x06, 0xD3, 0xA9, 0xDF, 0x35, 0xFF, 0x4F,
    0x43, 0x7F, 0xD3, 0xF4, 0x90, 0x7A, 0xFF, 0xFE, 0xD7, 0x7F, 0x63, 0xE5, 0xE7, 0x3D, 0xFA, 0xFF,
    0xBD, 0x6A, 0x38, 0x37, 0x11, 0xC9, 0xE4, 0xB0, 0x07, 0x00, 0xEA, 0x8F, 0x0E, 0xC7, 0x44, 0x31,
    0x1D, 0x5D, 0x31, 0x65, 0x3A, 0x94, 0x58, 0xE2, 0xFF, 0xE3, 0x18, 0xC4, 0x16, 0x0C, 0x18, 0x56,
    0x70, 0x09, 0x4C, 0x18, 0x00, 0xE7, 0xF8, 0x20, 0xE2, 0x81, 0x89, 0x77, 0x67, 0xFD, 0x79, 0x73,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x63, 0x4E, 0xCF, 0xCA, 0x72, 0x87, 0x3F, 0x78, 0x7D, 0x15,
    0x92, 0x5B, 0x6D, 0xB6, 0x8A, 0x28, 0xB4, 0x08, 0x02, 0x02, 0xC2, 0x3A, 0xAF, 0x34, 0x44, 0xCB,
    0xC8, 0xEA, 0xA2, 0x4E, 0xA6, 0x82, 0xC7, 0x26, 0xF3, 0x32, 0xD6, 0x34, 0xAD, 0x7F, 0xE4, 0xC6,
    0xFF, 0xE3, 0x18, 0xC4, 0x20, 0x15, 0x23, 0x8E, 0xDE, 0x59, 0x8F, 0x68, 0x03, 0x3D, 0x75, 0xAC,
    0xDD, 0xE7, 0xAE, 0x52, 0xDB, 0x5D, 0xD7, 0xFE, 0xB5, 0xDF, 0xFF, 0xED, 0xD4, 0xCB, 0xFF, 0xFB,
    0x6A, 0x7F, 0xDB, 0xFF, 0xFF, 0xFF, 0xFF, 0x9C, 0x1E, 0x85, 0x01, 0xAC, 0x4C, 0x82, 0x71, 0xFF,
    0xFF, 0xFF, 0xE3, 0x20, 0x7B, 0xED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4, 0x07, 0x99, 0x71, 0x63,
    0xC2, 0x75, 0xDB, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE3, 0x18, 0xC4, 0x06, 0x0E, 0xBA, 0xBB,
    0x26, 0x59, 0x85, 0x50, 0x02, 0x1D, 0xFE, 0x83, 0xB8, 0x09, 0x99, 0xD4, 0x78, 0x36, 0xA2, 0x9F,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xED, 0x49, 0xFA, 0xBB, 0xFF, 0xCD,
    0x42, 0x3F, 0xEA, 0xC3, 0x21, 0xF0, 0x81, 0xFC, 0xA8, 0xFC, 0x9C, 0x71, 0xD8, 0xF1, 0xE7, 0xFF,
    0xCB, 0x7F, 0xF8, 0x8C, 0x05, 0xAF, 0xF8, 0x1B, 0xFF, 0xD4, 0x02, 0x14, 0x55, 0xA3, 0x4C, 0xF1,
    0xFF, 0xE3, 0x18, 0xC4, 0x06, 0x0E, 0x9A, 0xC6, 0xC8, 0x21, 0x85, 0x90, 0x01, 0x30, 0x7F, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x1D, 0x7F, 0xFF, 0xFF, 0xF6, 0x45, 0x12, 0xE9,
    0x91, 0x16, 0x0F, 0x4B, 0xFB, 0x3A, 0xD4, 0x8F, 0xEA, 0x3A, 0x91, 0x06, 0x1E, 0x43, 0x2D, 0xA0,
    0x39, 0x43, 0xB9, 0x7D, 0x7F, 0xA0, 0xAB, 0xFF, 0xE0, 0x3E, 0xC0, 0x2E, 0xC8, 0xC2, 0x91, 0x8A,
    0xE0, 0x75, 0x00, 0x00, 0xD8, 0x10, 0x01, 0x92, 0xFF, 0xE3, 0x18, 0xC4, 0x06, 0x0E, 0xC2, 0xBE,
    0xD4, 0x51, 0x81, 0x98, 0x01, 0xCF, 0x45, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xD6, 0xA4, 0x96, 0x92, 0x49, 0x7F, 0xA2, 0xCE, 0x4C, 0xFE, 0x9A, 0x69, 0xAC, 0x8B,
    0x92, 0x7F, 0xAE, 0x70, 0xC4, 0xD4, 0x5A, 0x82, 0x02, 0x89, 0x49, 0x3F, 0xFE, 0x10, 0xAC, 0x06,
    0xC8, 0x48, 0x54, 0xDF, 0xE1, 0xB2, 0x86, 0xDA, 0x48, 0x8D, 0x40, 0x43, 0x20, 0xF7, 0x23, 0xCC,
    0xFF, 0xE3, 0x18, 0xC4, 0x06, 0x0E, 0xB2, 0xC2, 0xCC, 0x11, 0x82, 0xA0, 0x00, 0x5F, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5A, 0xBF, 0x41, 0x65, 0xF2,
    0xD0, 0xE5, 0x89, 0xDF, 0xF5, 0x3C, 0xC8, 0xCC, 0x59, 0xAB, 0x5F, 0xFF, 0x91, 0x52, 0x7C, 0x9F,
    0x22, 0x06, 0x66, 0xE7, 0x97, 0xF6, 0x10, 0x88, 0x11, 0x43, 0x27, 0x94, 0xF4, 0xFF, 0xFA, 0xCF,
    0xAA, 0x4D, 0x00, 0x00, 0x04, 0x14, 0x30, 0x1B, 0xFF, 0xE3, 0x18, 0xC4, 0x06, 0x0E, 0xAA, 0x86,
    0xDC, 0x79, 0x81, 0x88, 0x00, 0x2D, 0x8B, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xEA, 0xD7, 0x49, 0x56, 0xFF, 0xDB, 0xD4, 0x8A, 0xD6, 0x68, 0x46, 0x91, 0x3F, 0xD7,
    0x4D, 0x22, 0x88, 0xE8, 0xD7, 0xB7, 0xB9, 0x90, 0x9E, 0xC1, 0x7C, 0x6F, 0xFF, 0xC6, 0x54, 0xC8,
    0xD8, 0x8F, 0x96, 0x0B, 0x62, 0x2D, 0x3F, 0xE9, 0xFF, 0x5A, 0x0C, 0x00, 0x10, 0x6C, 0x32, 0xE6,
    0xFF, 0xE3, 0x18, 0xC4, 0x06, 0x0E, 0x7A, 0xBE, 0xD4, 0x11, 0x82, 0x88, 0x00, 0x1D, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD0, 0x40, 0xDC, 0xCC, 0xBF,
    0xFD, 0x22, 0xF1, 0x89, 0x15, 0x15, 0xAF, 0x99, 0xA4, 0xA4, 0x2A, 0x4C, 0xD8, 0x41, 0x41, 0x3A,
    0x15, 0x13, 0x4F, 0xFE, 0x10, 0xC0, 0x24, 0x48, 0x8A, 0x49, 0x24, 0xBF, 0xE0, 0xD5, 0x05, 0xD5,
    0xA5, 0x74, 0x6C, 0x00, 0xDA, 0xD4, 0x00, 0x2A, 0xFF, 0xE3, 0x18, 0xC4, 0x07, 0x0D, 0x1A, 0x5A,
    0xF2, 0x59, 0xC2, 0x28, 0x02, 0x59, 0x8D, 0x5B, 0xB1, 0x7F, 0x7F, 0x75, 0x48, 0x32, 0x3F, 0xFF,
    0xFF, 0xFF, 0xE5, 0x2F, 0xA1, 0x8C, 0x3C, 0x45, 0x94, 0x55, 0x93, 0xEB, 0xFD, 0x19, 0x0D, 0x52,
    0xB2, 0x1A, 0x63, 0x65, 0x98, 0xDF, 0xFF, 0xFF, 0xF2, 0xE5, 0x99, 0xEB, 0x33, 0x09, 0x00, 0x40,
    0x10, 0x18, 0xCA, 0x23, 0x8E, 0x1E, 0x8A, 0x82, 0xD0, 0xFC, 0x04, 0x50, 0xB9, 0x09, 0x08, 0xB2,
    0xFF, 0xE3, 0x18, 0xC4, 0x0D, 0x09, 0xBA, 0x56, 0x80, 0x10, 0x50, 0x0A, 0xB4, 0x97, 0x54, 0x3A,
    0xFF, 0x7F, 0xE6, 0xFA, 0x7F, 0xF3, 0x1A, 0x54, 0xDF, 0x37, 0x2A, 0x8A, 0x89, 0x00, 0xC2, 0xDF,
    0xF2, 0xDF, 0x73, 0xBB, 0xBD, 0x0C, 0x19, 0xD5, 0xB8, 0xAB, 0x2E, 0x29, 0xA8, 0x1E, 0x80, 0x90,
    0x51, 0xCB, 0x34, 0xDC, 0x94, 0x04, 0x04, 0x8E, 0xA3, 0x8A, 0x07, 0x7E, 0x2A, 0xD4, 0x27, 0xFD,
    0xA8, 0xFA, 0xFF, 0xC3, 0x5D, 0xF7, 0xE7, 0x59, 0xFF, 0xE3, 0x18, 0xC4, 0x21, 0x0A, 0x28, 0x46,
    0x58, 0x50, 0x61, 0x86, 0x00, 0xFF, 0xF2, 0xAE, 0xFF, 0xFD, 0xBF, 0xFC, 0x92, 0x02, 0xE2, 0xAE,
    0x50, 0xF0, 0x44, 0x0D, 0x2B, 0x44, 0x92, 0x3C, 0x68, 0x67, 0x32, 0x22, 0x2A, 0x75, 0x47, 0x88,
    0x08, 0xAC, 0xEA, 0x7F, 0x6F, 0x2D, 0xCB, 0x7F, 0x3D, 0x8D, 0x7F, 0xE4, 0x7D, 0xDF, 0xD1, 0xFD,
    0x5F, 0xFF, 0xFF, 0xE5, 0xBF, 0x55, 0x86, 0x63, 0x21, 0x60, 0x86, 0x14, 0x42, 0x02, 0xE9, 0x01,
    0xFF, 0xE3, 0x18, 0xC4, 0x33, 0x0A, 0x28, 0x3A, 0x20, 0x50, 0x31, 0x86, 0x00, 0xA0, 0x34, 0x75,
    0x70, 0xCA, 0x81, 0x4C, 0x0C, 0xB9, 0x69, 0xEE, 0x69, 0x1C, 0x88, 0x5C, 0x85, 0x4A, 0xD7, 0x86,
    0xB7, 0x11, 0x0D, 0x6A, 0x7F, 0x5C, 0xB7, 0xFF, 0x53, 0xFF, 0xFE, 0xDF, 0xFF, 0xD2, 0x4C, 0x41,
    0x4D, 0x45, 0x33, 0x2E, 0x39, 0x39, 0x2E, 0x35, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xE3, 0x18, 0xC4, 0x45, 0x0A, 0xB8, 0x61,
    0xD0, 0x00, 0xC2, 0x4C, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xFF, 0xE3, 0x18, 0xC4, 0x55, 0x00, 0x00, 0x03, 0x48, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
};


#endif /* RESOURCES_H */
