//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/console/console.h
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
#ifndef CONSOLE_H
#define CONSOLE_H

#include <Arduino.h>
#include "../resources.h"
#include "../drivers/power.h"
#include "../libs/itask.h"
#include "../libs/iprint.h"

#define INPUT_BUFFER_LENGTH         128

#define CONSOLE_COMMANDS_COUNT      12

#define TASK_CONSOLE_PRINT_HELP     1
#define TASK_CONSOLE_NET_RESTART    2
#define TASK_CONSOLE_NET_STATUS     3
#define TASK_CONSOLE_NET_CONFIG     4
#define TASK_CONSOLE_NET_NSLOOKUP   5
#define TASK_CONSOLE_NET_PING       6
#define TASK_CONSOLE_NET_CONFIG     7
#define TASK_CONSOLE_NET_START      8
#define TASK_CONSOLE_NET_STOP       9
#define TASK_CONSOLE_SET_TZ         10




PROG_STR( S_CONSOLE_WELCOME_1,        "Alarm clock V3 (firmware " FW_VERSION ")" );
PROG_STR( S_CONSOLE_WELCOME_2,        "www.bfrigon.com");

PROG_STR( S_COMMAND_HELP,             "help" );
PROG_STR( S_COMMAND_REBOOT,           "reboot" );
PROG_STR( S_COMMAND_SET_TIMEZONE,     "set timezone" );
PROG_STR( S_COMMAND_DATE_SET,         "set date" );
PROG_STR( S_COMMAND_DATE,             "date" );
PROG_STR( S_COMMAND_TZ_INFO,          "tz info" );
PROG_STR( S_COMMAND_TZ_SET,           "tz set" );   /* alias of "set timezone" */
PROG_STR( S_COMMAND_NTPDATE,          "ntpdate" );
PROG_STR( S_COMMAND_PRINT_LOGS,       "print logs" );
PROG_STR( S_COMMAND_NET_STATUS,       "net status" );
PROG_STR( S_COMMAND_NET_CONFIG,       "net config" );
PROG_STR( S_COMMAND_NET_STOP,         "net stop" );
PROG_STR( S_COMMAND_NET_START,        "net start" );
PROG_STR( S_COMMAND_NET_RESTART,      "net restart" );
PROG_STR( S_COMMAND_NET_NSLOOKUP,     "nslookup" );
PROG_STR( S_COMMAND_NET_PING,         "ping" );
PROG_STR( S_COMMAND_SETTING_BACKUP,   "config backup" );
PROG_STR( S_COMMAND_SETTING_RESTORE,  "config restore" );
PROG_STR( S_COMMAND_FACTORY_RESET,    "config defaults" );

PROG_STR( S_HELP_HELP,                "Display this message." );
PROG_STR( S_HELP_REBOOT,              "Restart the firmware." );
PROG_STR( S_HELP_DATE_SET,            "Set the clock." );
PROG_STR( S_HELP_DATE,                "Display the current time and time zone" );
PROG_STR( S_HELP_SET_TIMEZONE,        "Set the time zone." );
PROG_STR( S_HELP_NTPDATE,             "Query the NTP time server" );
PROG_STR( S_HELP_PRINT_LOGS,          "Print the event log stored on SD card." );
PROG_STR( S_HELP_NET_STATUS,          "Show the status of the WiFi connection." );
PROG_STR( S_HELP_NET_CONFIG,          "Configure the network settings.");
PROG_STR( S_HELP_NET_RESTART,         "Restart the WiFi manager." );
PROG_STR( S_HELP_NET_STOP,            "Stop the WiFi manager." );
PROG_STR( S_HELP_NET_NSLOOKUP,        "Query the nameserver for the IP address of the given host." );
PROG_STR( S_HELP_NET_PING,            "Test the reachability of a given host." );

PROG_STR( S_USAGE_NSLOOKUP,           "nslookup [hostname]" );
PROG_STR( S_USAGE_PING,               "ping [host]" );

const char* const S_COMMANDS[] PROGMEM = {
    S_COMMAND_HELP,
    S_COMMAND_DATE,
    S_COMMAND_DATE_SET,
    S_COMMAND_SET_TIMEZONE,
    S_COMMAND_NTPDATE,
    S_COMMAND_PRINT_LOGS,
    S_COMMAND_NET_STATUS,
    S_COMMAND_NET_CONFIG,
    S_COMMAND_NET_RESTART,
    S_COMMAND_NET_STOP,
    S_COMMAND_NET_NSLOOKUP,
    S_COMMAND_NET_PING,
    S_COMMAND_REBOOT,
};
const char* const S_HELP_COMMANDS[] PROGMEM = {
    S_HELP_HELP,
    S_HELP_DATE,
    S_HELP_DATE_SET,
    S_HELP_SET_TIMEZONE,
    S_HELP_NTPDATE,
    S_HELP_PRINT_LOGS,
    S_HELP_NET_STATUS,
    S_HELP_NET_CONFIG,
    S_HELP_NET_RESTART,
    S_HELP_NET_STOP,
    S_HELP_NET_NSLOOKUP,
    S_HELP_NET_PING,
    S_HELP_REBOOT,
};



class Console : public IPrint, ITask {

  public:
    Console();
    void begin( unsigned long baud );

    void enableInput();
    void disableInput();
    void runTask();

  private:
    bool processInput();
    void trimInput();
    void parseCommand();
    void resetInput();
    bool matchCommandName( const char *command, bool hasParameter = false ); 
    char* getInputParameter();
    
    void displayPrompt();
    uint8_t _print( char c );

    bool startTaskPrintHelp();
    void runTaskPrintHelp();
    bool startTaskNetRestart();
    void runTaskNetRestart();
    bool startTaskNetStart();
    bool startTaskNetStop();
    void runTaskNetStop();
    void printNetStatus();
    bool startTaskNslookup();
    void runTaskNsLookup();
    bool startTaskPing();
    void runTaskPing();
    bool startTaskNetworkConfig();
    void runTaskNetworkConfig();
    bool startTaskDateSet();
    void runTaskDateSet();
    void printDateTime();
    bool startTaskSetTimeZone();
    void runTaskSetTimeZone();
    void showTimezoneInfo();



    char _inputbuffer[ INPUT_BUFFER_LENGTH + 1 ];
    char* _inputParameter= NULL;
    uint8_t _inputlength = 0;
    bool _inputenabled = false;
    bool _inputHidden = false;

    uint16_t _taskIndex = 0;
};

extern Console g_console;

#endif  /* CONSOLE_H */