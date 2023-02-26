//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/console_base.h
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
#ifndef CONSOLE_BASE_H
#define CONSOLE_BASE_H


#include <Arduino.h>
#include <resources.h>
#include <itask.h>
#include <iprint.h>



/* Limits */
#define INPUT_BUFFER_LENGTH         80
#define CMD_HISTORY_BUFFER_LENGTH   256

/* Console tasks ID's */ 
enum consoleTaskIds {
    TASK_CONSOLE_PRINT_HELP = 1,
    TASK_CONSOLE_NET_RESTART,
    TASK_CONSOLE_NET_STATUS,
    TASK_CONSOLE_NET_CONFIG,
    TASK_CONSOLE_NET_NSLOOKUP,
    TASK_CONSOLE_NET_PING,
    TASK_CONSOLE_NET_START,
    TASK_CONSOLE_NET_STOP,
    TASK_CONSOLE_SET_TZ,
    TASK_CONSOLE_CONFIG_BACKUP,
    TASK_CONSOLE_CONFIG_RESTORE,
    TASK_CONSOLE_FACTORY_RESET,
    TASK_CONSOLE_SET_DATE,
    TASK_CONSOLE_NTP_SYNC,
    TASK_CONSOLE_PRINT_LOGS,
    TASK_CONSOLE_MQTT_SEND,
    TASK_CONSOLE_MQTT_ENABLE,
    TASK_CONSOLE_MQTT_DISABLE,
    TASK_CONSOLE_PRINT_JULIETTE_ANSI,
};

/* Accepted commands */ 
PROG_STR( S_COMMAND_HELP,             "help" );
PROG_STR( S_COMMAND_EXIT,             "exit" );
PROG_STR( S_COMMAND_CLEAR,            "clear" );
PROG_STR( S_COMMAND_REBOOT,           "reboot" );
PROG_STR( S_COMMAND_SET_TIMEZONE,     "set timezone" );
PROG_STR( S_COMMAND_SET_DATE,         "set date" );
PROG_STR( S_COMMAND_SET_TIME,         "set time" );   /* alias of "set date" */
PROG_STR( S_COMMAND_DATE,             "date" );
PROG_STR( S_COMMAND_TZ_INFO,          "tz info" );
PROG_STR( S_COMMAND_TZ_SET,           "tz set" );     /* alias of "set timezone" */
PROG_STR( S_COMMAND_NTP_SYNC,         "ntp sync" );
PROG_STR( S_COMMAND_NTP_STATUS,       "ntp status" );
PROG_STR( S_COMMAND_LOGS,             "logs" );
PROG_STR( S_COMMAND_SERVICE,          "service" );
PROG_STR( S_COMMAND_NET_STATUS,       "net status" );
PROG_STR( S_COMMAND_NET_CONFIG,       "net config" );
PROG_STR( S_COMMAND_NET_STOP,         "net stop" );
PROG_STR( S_COMMAND_NET_START,        "net start" );
PROG_STR( S_COMMAND_NET_RESTART,      "net restart" );
PROG_STR( S_COMMAND_NET_PING,         "net ping" );   /* alias of "ping" */
PROG_STR( S_COMMAND_NSLOOKUP,         "nslookup" );
PROG_STR( S_COMMAND_PING,             "ping" );
PROG_STR( S_COMMAND_FREE,             "free" );
PROG_STR( S_COMMAND_SETTING_BACKUP,   "config backup" );
PROG_STR( S_COMMAND_SETTING_RESTORE,  "config restore" );
PROG_STR( S_COMMAND_FACTORY_RESET,    "factory reset" );
PROG_STR( S_COMMAND_BATT_STATUS,      "batt status");
PROG_STR( S_COMMAND_MQTT_ENABLE,      "mqtt enable");
PROG_STR( S_COMMAND_MQTT_DISABLE,     "mqtt disable");
PROG_STR( S_COMMAND_MQTT_STATUS,      "mqtt status");
PROG_STR( S_COMMAND_MQTT_SEND,        "mqtt send");
PROG_STR( S_COMMAND_JULIETTE,         "juliette");

/* Command descriptions */ 
PROG_STR( S_HELP_HELP,                "Display this message." );
PROG_STR( S_HELP_REBOOT,              "Restart the firmware." );
PROG_STR( S_HELP_SET_TIMEZONE,        "Set the time zone." );
PROG_STR( S_HELP_SET_DATE,            "Set the clock." );
PROG_STR( S_HELP_DATE,                "Display the current time and time zone" );
PROG_STR( S_HELP_NTPSYNC,             "Synchronize the clock using the configured NTP server" );
PROG_STR( S_HELP_LOGS,                "Print the events log" );
PROG_STR( S_HELP_NET_STATUS,          "Show the status of the WiFi connection." );
PROG_STR( S_HELP_NET_CONFIG,          "Configure the network settings.");
PROG_STR( S_HELP_NET_RESTART,         "Restart the WiFi manager." );
PROG_STR( S_HELP_NET_STOP,            "Stop the WiFi manager." );
PROG_STR( S_HELP_NSLOOKUP,            "Query the nameserver for the IP address of the given host." );
PROG_STR( S_HELP_PING,                "Test the reachability of a given host." );
PROG_STR( S_HELP_SERVICE,             "Disable/enable service." );
PROG_STR( S_HELP_SETTING_BACKUP,      "Save settings to a file on the SD card." );
PROG_STR( S_HELP_SETTING_RESTORE,     "Restore settings from a file on the SD card." );
PROG_STR( S_HELP_FACTORY_RESET,       "Restore settings to their default values." );
PROG_STR( S_HELP_BATT_STATUS,         "Get the battery health status" );
PROG_STR( S_HELP_MQTT_ENABLE,         "Enable the MQTT client" );
PROG_STR( S_HELP_MQTT_DISABLE,        "Disable the MQTT client" );
PROG_STR( S_HELP_MQTT_STATUS,         "Display the client connection status" );
PROG_STR( S_HELP_MQTT_SEND_TOPIC,     "Send a message" );

/* Commands usage */ 
PROG_STR( S_USAGE_NSLOOKUP,           "nslookup [hostname]" );
PROG_STR( S_USAGE_PING,               "ping [host]" );
PROG_STR( S_USAGE_SERVICE,            "service [name] (enable|disable|status)" );
PROG_STR( S_USAGE_MQTT_SEND,          "mqtt send [topic] [payload]" );

/* Commands listed on the help menu */
#define CONSOLE_HELP_MENU_ITEMS       22
const char* const S_COMMANDS[] PROGMEM = {
    S_COMMAND_HELP,
    S_COMMAND_DATE,
    S_COMMAND_SET_DATE,
    S_COMMAND_SET_TIMEZONE,
    S_COMMAND_NTP_SYNC,
    S_COMMAND_LOGS,
    S_COMMAND_NET_STATUS,
    S_COMMAND_NET_CONFIG,
    S_COMMAND_NET_RESTART,
    S_COMMAND_NET_STOP,
    S_COMMAND_NSLOOKUP,
    S_COMMAND_PING,
    S_COMMAND_SERVICE,
    S_COMMAND_SETTING_BACKUP,
    S_COMMAND_SETTING_RESTORE,
    S_COMMAND_FACTORY_RESET,
    S_COMMAND_BATT_STATUS,
    S_COMMAND_REBOOT,
    S_COMMAND_MQTT_ENABLE,
    S_COMMAND_MQTT_DISABLE,
    S_COMMAND_MQTT_STATUS,
    S_COMMAND_MQTT_SEND,
};
const char* const S_HELP_COMMANDS[] PROGMEM = {
    S_HELP_HELP,
    S_HELP_DATE,
    S_HELP_SET_DATE,
    S_HELP_SET_TIMEZONE,
    S_HELP_NTPSYNC,
    S_HELP_LOGS,
    S_HELP_NET_STATUS,
    S_HELP_NET_CONFIG,
    S_HELP_NET_RESTART,
    S_HELP_NET_STOP,
    S_HELP_NSLOOKUP,
    S_HELP_PING,
    S_HELP_SERVICE,
    S_HELP_SETTING_BACKUP,
    S_HELP_SETTING_RESTORE,
    S_HELP_FACTORY_RESET,
    S_HELP_BATT_STATUS,
    S_HELP_REBOOT,
    S_HELP_MQTT_ENABLE,
    S_HELP_MQTT_DISABLE,
    S_HELP_MQTT_STATUS,
    S_HELP_MQTT_SEND_TOPIC,
};

enum ctrlSequences { 
    CTRL_SEQ_CLEAR_SCREEN,
    CTRL_SEQ_CLEAR_SCROLLBACK,
    CTRL_SEQ_CURSOR_POSITION,
    CTRL_SEQ_ERASE_LINE,
    CTRL_SEQ_CURSOR_COLUMN,
    CTRL_SEQ_CURSOR_LEFT,
};



/*******************************************************************************
 *
 * @brief   Console base class
 * 
 *******************************************************************************/
class ConsoleBase : public IPrint, protected ITask {

  public:
    ConsoleBase();
    virtual void runTasks() = 0;
    void printDateTime( DateTime *dt, const char *timezone, int16_t ms = -1 );
    void printErrorMessage( int8_t error );
    void clearScreen();


  protected:
    void resetInput();
    void displayPrompt();
    virtual int _read() = 0;
    virtual int _peek() = 0;
    virtual int _available() = 0;
    virtual void exitConsole( bool timeout = false ) = 0;
    virtual void resetConsole() = 0;


  private:
    char _inputBuffer[ INPUT_BUFFER_LENGTH + 1 ];
    char _historyBuffer[ CMD_HISTORY_BUFFER_LENGTH + 1];
    char* _inputParameter;
    char* _cmdHistoryPtr;
    uint8_t _inputBufferLimit;
    bool _inputHidden;
    uint8_t _escapeSequence;
    int16_t _taskIndex;
    bool _cmdHistoryEnabled;
    
    bool processInput();
    void trimInput();
    void parseCommand();
    bool matchCommandName( const char *command, bool hasParameter = false ); 
    char* getInputParameter();
    void readHistoryBuffer( bool forward );
    void writeHistoryBuffer();
    void sendControlSequence( uint8_t sequence, uint8_t row = 1, uint8_t col = 1 );
    void processControlSequence( char ch );

    // ----------------------------------------
    // Commands
    // ----------------------------------------

    /* 'help' command */
    void beginTaskPrintHelp();
    void runTaskPrintHelp();
    
    /* 'net restart' command */
    bool beginTaskNetRestart();
    void runTaskNetRestart();

    /* 'net start' command */
    bool beginTaskNetStart();

    /* 'net stop' command */
    bool beginTaskNetStop();
    void runTaskNetStop();

    /* 'net status' command */
    void printNetStatus();

    /* 'nslookup' command */
    bool beginTaskNslookup();
    void runTaskNsLookup();

    /* 'ping' command */
    bool beginTaskPing();
    void runTaskPing();

    /* 'net config' command */
    bool beginTaskNetworkConfig();
    void runTaskNetworkConfig();
    
    /* 'set date' command */
    bool beginTaskSetDate();
    void runTaskSetDate();

    /* 'date' command */
    void runCommandPrintCurrentTime();

    /* 'set timezone' and 'tz set' command */
    bool beginTaskSetTimeZone();
    void runTaskSetTimeZone();

    /* 'tz info' command */
    void showTimezoneInfo();
    
    /* 'config backup' command */
    bool beginTaskConfigBackup();
    void runTaskConfigBackup();

    /* 'config restore' command */
    bool beginTaskConfigRestore();
    void runTaskConfigRestore();

    /* 'factory reset' command */
    bool beginTaskFactoryReset();
    void runTaskFactoryReset();

    /* 'ntp sync' command */
    bool beginTaskNtpSync();
    void runTaskNtpSync();

    /* 'service' command */
    void runCommandService();

    /* 'batt status' command */
    void printBattStatus();

    /* 'logs' command */
    void beginTaskPrintLogs(); 
    void runTaskPrintLogs();

    /* mqtt enable */
    bool beginTaskMqttEnable();
    void runTaskMqttEnable();

    /* mqtt disable */
    bool beginTaskMqttDisable();
    void runTaskMqttDisable();

    /* mqtt send */
    bool beginTaskMqttSend();
    void runTaskMqttSend();

    /* mqtt status */
    void runCommandMqttStatus();

    /* Juliette */
    bool beginPrintJulietteANSI();
    void runTaskPrintJulietteANSI();
};

#endif  /* CONSOLE_H */