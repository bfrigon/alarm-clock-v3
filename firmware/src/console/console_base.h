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
#include "../resources.h"
#include "../libs/itask.h"
#include "../libs/iprint.h"


#define INPUT_BUFFER_LENGTH         80
#define CMD_HISTORY_BUFFER_LENGTH   256



// ----------------------------------------
// Console tasks ID's
// ----------------------------------------
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
#define TASK_CONSOLE_CONFIG_BACKUP  11
#define TASK_CONSOLE_CONFIG_RESTORE 12
#define TASK_CONSOLE_FACTORY_RESET  13
#define TASK_CONSOLE_SET_DATE       14
#define TASK_CONSOLE_NTP_SYNC       15


// ----------------------------------------
// Command names
// ----------------------------------------
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
PROG_STR( S_COMMAND_NET_STATUS,       "net status" );
PROG_STR( S_COMMAND_NET_CONFIG,       "net config" );
PROG_STR( S_COMMAND_NET_STOP,         "net stop" );
PROG_STR( S_COMMAND_NET_START,        "net start" );
PROG_STR( S_COMMAND_NET_RESTART,      "net restart" );
PROG_STR( S_COMMAND_NET_PING,         "net ping" );   /* alias of "ping" */
PROG_STR( S_COMMAND_NSLOOKUP,         "nslookup" );
PROG_STR( S_COMMAND_PING,             "ping" );
PROG_STR( S_COMMAND_SETTING_BACKUP,   "config backup" );
PROG_STR( S_COMMAND_SETTING_RESTORE,  "config restore" );
PROG_STR( S_COMMAND_FACTORY_RESET,    "factory reset" );


// ----------------------------------------
// Command help strings
// ----------------------------------------
PROG_STR( S_HELP_HELP,                "Display this message." );
PROG_STR( S_HELP_REBOOT,              "Restart the firmware." );
PROG_STR( S_HELP_SET_TIMEZONE,        "Set the time zone." );
PROG_STR( S_HELP_SET_DATE,            "Set the clock." );
PROG_STR( S_HELP_DATE,                "Display the current time and time zone" );
PROG_STR( S_HELP_NTPSYNC,             "Synchronize the clock using the configured NTP server" );
PROG_STR( S_HELP_LOGS,                "Print the event log stored on the SD card." );
PROG_STR( S_HELP_NET_STATUS,          "Show the status of the WiFi connection." );
PROG_STR( S_HELP_NET_CONFIG,          "Configure the network settings.");
PROG_STR( S_HELP_NET_RESTART,         "Restart the WiFi manager." );
PROG_STR( S_HELP_NET_STOP,            "Stop the WiFi manager." );
PROG_STR( S_HELP_NSLOOKUP,            "Query the nameserver for the IP address of the given host." );
PROG_STR( S_HELP_PING,                "Test the reachability of a given host." );
PROG_STR( S_HELP_SETTING_BACKUP,      "Save settings to a file on the SD card." );
PROG_STR( S_HELP_SETTING_RESTORE,     "Restore settings from a file on the SD card." );
PROG_STR( S_HELP_FACTORY_RESET,       "Restore settings to their default values." );


// ----------------------------------------
// Command usage help
// ----------------------------------------
PROG_STR( S_USAGE_NSLOOKUP,           "nslookup [hostname]" );
PROG_STR( S_USAGE_PING,               "ping [host]" );


// ----------------------------------------
// Commands listed on the help menu
// ----------------------------------------
#define CONSOLE_HELP_MENU_ITEMS       16
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
    S_COMMAND_SETTING_BACKUP,
    S_COMMAND_SETTING_RESTORE,
    S_COMMAND_FACTORY_RESET,
    S_COMMAND_REBOOT,
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
    S_HELP_SETTING_BACKUP,
    S_HELP_SETTING_RESTORE,
    S_HELP_FACTORY_RESET,
    S_HELP_REBOOT,
};

enum { 
    CTRL_SEQ_CLEAR_SCREEN,
    CTRL_SEQ_CLEAR_SCROLLBACK,
    CTRL_SEQ_CURSOR_POSITION,
    CTRL_SEQ_ERASE_LINE,
    CTRL_SEQ_CURSOR_COLUMN,
    CTRL_SEQ_CURSOR_LEFT,

};





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
    uint16_t _taskIndex;
    
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
    void startTaskPrintHelp();
    void runTaskPrintHelp();
    
    /* 'net restart' command */
    bool startTaskNetRestart();
    void runTaskNetRestart();

    /* 'net start' command */
    bool startTaskNetStart();

    /* 'net stop' command */
    bool startTaskNetStop();
    void runTaskNetStop();

    /* 'net status' command */
    void printNetStatus();

    /* 'nslookup' command */
    bool startTaskNslookup();
    void runTaskNsLookup();

    /* 'ping' command */
    bool startTaskPing();
    void runTaskPing();

    /* 'net config' command */
    bool startTaskNetworkConfig();
    void runTaskNetworkConfig();
    
    /* 'set date' command */
    bool startTaskSetDate();
    void runTaskSetDate();

    /* 'date' command */
    void runTaskPrintDateTime();

    /* 'set timezone' and 'tz set' command */
    bool startTaskSetTimeZone();
    void runTaskSetTimeZone();

    /* 'tz info' command */
    void showTimezoneInfo();
    
    /* 'config backup' command */
    bool startTaskConfigBackup();
    void runTaskConfigBackup();

    /* 'config restore' command */
    bool startTaskConfigRestore();
    void runTaskConfigRestore();

    /* 'factory reset' command */
    bool startTaskFactoryReset();
    void runTaskFactoryReset();

    /* 'ntp sync' command */
    bool startTaskNtpSync();
    void runTaskNtpSync();

};



#endif  /* CONSOLE_H */