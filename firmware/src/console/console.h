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
#define LOG_BUFFER_LENGTH           1024

#define CONSOLE_COMMANDS_COUNT      9

#define TASK_CONSOLE_PRINT_HELP     1
#define TASK_CONSOLE_NET_RESTART    2
#define TASK_CONSOLE_NET_STATUS     3
#define TASK_CONSOLE_NET_CONFIG     4
#define TASK_CONSOLE_NET_NSLOOKUP   5
#define TASK_CONSOLE_NET_PING       6




PROG_STR( S_CONSOLE_WELCOME_1,        "Alarm clock V3 (firmware " FW_VERSION ")" );
PROG_STR( S_CONSOLE_WELCOME_2,        "www.bfrigon.com");

PROG_STR( S_COMMAND_HELP,             "help" );
PROG_STR( S_COMMAND_REBOOT,           "reboot" );
PROG_STR( S_COMMAND_SET_DATE,         "set time" );
PROG_STR( S_COMMAND_PRINT_LOGS,       "print logs" );
PROG_STR( S_COMMAND_NET_STATUS,       "net status" );
PROG_STR( S_COMMAND_NET_CONFIG,       "net config" );
PROG_STR( S_COMMAND_NET_RESTART,      "net restart" );
PROG_STR( S_COMMAND_NET_NSLOOKUP,     "nslookup" );
PROG_STR( S_COMMAND_NET_PING,         "ping" );

PROG_STR( S_HELP_HELP,                "Display this message." );
PROG_STR( S_HELP_REBOOT,              "Restart the firmware." );
PROG_STR( S_HELP_SET_DATE,            "Set the clock" );
PROG_STR( S_HELP_PRINT_LOGS,          "Print event log" );
PROG_STR( S_HELP_NET_STATUS,          "Show the status of the WiFi connection" );
PROG_STR( S_HELP_NET_CONFIG,          "Setup WiFi parameters");
PROG_STR( S_HELP_NET_RESTART,         "Reconnect to the WiFi network" );
PROG_STR( S_HELP_NET_NSLOOKUP,        "Query the nameserver for the IP address of the given host" );
PROG_STR( S_HELP_NET_PING,            "Test the reachability of a given host " );

PROG_STR( S_USAGE_NSLOOKUP,           "nslookup [hostname]" );
PROG_STR( S_USAGE_PING,               "ping [host]" );

const char* const S_COMMANDS[] PROGMEM = {
    S_COMMAND_HELP,
    S_COMMAND_SET_DATE,
    S_COMMAND_PRINT_LOGS,
    S_COMMAND_NET_STATUS,
    S_COMMAND_NET_CONFIG,
    S_COMMAND_NET_RESTART,
    S_COMMAND_NET_NSLOOKUP,
    S_COMMAND_NET_PING,
    S_COMMAND_REBOOT,
};
const char* const S_HELP_COMMANDS[] PROGMEM = {
    S_HELP_HELP,
    S_HELP_SET_DATE,
    S_HELP_PRINT_LOGS,
    S_HELP_NET_STATUS,
    S_HELP_NET_CONFIG,
    S_HELP_NET_RESTART,
    S_HELP_NET_NSLOOKUP,
    S_HELP_NET_PING,
    S_HELP_REBOOT,
};



class Console : public IPrint, ITask {

  public:
    Console();
    void begin( unsigned long baud );

    void log( const char *message );

    void enableInput();
    void disableInput();

    void runTask();
    void endTask( int error );
    uint8_t startTask( uint8_t task );

    



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
    bool startTaskNetStatus();
    void runTaskNetStatus();
    bool startTaskNslookup();
    void runTaskNsLookup();
    bool startTaskPing();
    void runTaskPing();



    char _logbuffer[1024];
    char _inputbuffer[ INPUT_BUFFER_LENGTH ];
    char* _inputParameter= NULL;
    uint8_t _inputlength = 0;
    bool _inputenabled = false;
    unsigned long _timerCommandStart = 0;
    

    uint16_t _taskIndex = 0;
};

extern Console g_console;

#endif  /* CONSOLE_H */