//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/ftpserver.h
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
#ifndef FTPSERVER_H
#define FTPSERVER_H


#include <Arduino.h>
#include <asprintf.h>
#include <drivers/wifi/wifi.h>
#include <drivers/wifi/wifisocket.h>
#include <drivers/wifi/tcpclient.h>
#include <drivers/sdcard.h>
#include <console/console_base.h>
#include <task_errors.h>
#include <config.h>



/* Timing */
#define FTP_DATA_CONNECT_TIMEOUT            10000       /* Maximum amount of time to wait for a client data connection. */

/* Limits */
#define MAX_FTP_TRANSFER_BUFFER             1024        /* Maximum chunk size when downloading/uploading a file. */
#define MAX_FTP_CMD_LENGTH                  256         /* Maximum length of received commands from the client. */

/* Ports */
#define FTP_PORT                            21          /* default FTP port */

/* FTP responses */
PROG_STR( FTP_REPLY_150_XFER_READY,         "150 File status okay; about to open data connection\r\n" );
PROG_STR( FTP_REPLY_150_OK_TO_SEND,         "150 Ok to send data\r\n" );
PROG_STR( FTP_REPLY_200_TYPE_BINARY,        "200 Type set to BINARY\r\n" );
PROG_STR( FTP_REPLY_200_TYPE_ASCII,         "200 Type set to ASCII\r\n" );
PROG_STR( FTP_REPLY_200_MODE_STREAM,        "200 Mode set to STREAM\r\n" );
PROG_STR( FTP_REPLY_200_OK,                 "200 OK\r\n" );
PROG_STR( FTP_REPLY_200_ZZZ,                "200 Zzz...\r\n" );
PROG_STR( FTP_REPLY_210_SITE_ZONE,          "210 UTC%+d\r\n" );
PROG_STR( FTP_REPLY_211_FEATURES,           "211-Extensions suported:\r\n" \
                                            " MLST type*;modify*;create*;size*;\r\n" \
                                            " MLSD\r\n" \
                                            " SIZE\r\n" \
                                            " MDTM\r\n" \
                                            " MFMT\r\n" \
                                            " SITE ZONE\r\n" \
                                            "211 End\r\n" );
PROG_STR( FTP_REPLY_211_SERVER_STAT,        "211-Server status:\r\n" \
                                            " Connected to %d.%d.%d.%d\r\n" \
                                            " Logged in as %s\r\n" \
                                            "211 End of status\r\n" );
PROG_STR( FTP_REPLY_212_DIR_STAT_FOLLOW,    "212-Status follows\r\n" );
PROG_STR( FTP_REPLY_212_DIR_STAT_END,       "212 End of status\r\n" );
PROG_STR( FTP_REPLY_213_SIZE,               "213 %s\r\n" );
PROG_STR( FTP_REPLY_213_MOD_TIMESTAMP,      "213 %04d%02d%02d%02d%02d%02d\r\n" );
PROG_STR( FTP_REPLY_213_FILE_STAT_FOLLOW,   "213-Status follows\r\n" );
PROG_STR( FTP_REPLY_213_FILE_STAT_END,      "213 End of status\r\n" );
PROG_STR( FTP_REPLY_214_SITE_HELP,          "214 ZONE HELP\r\n" );
PROG_STR( FTP_REPLY_214_HELP,               "214- Commands implemented:\r\n" \
                                            " USER PASS QUIT NOOP STRU TYPE MODE FEAT SYST\r\n" \
                                            " CDUP CWD  PWD  LIST NLST MLSD MLST MDTM MFMT\r\n" \
                                            " RETR STOR APPE SIZE PORT PASV ABOR MKD  RMD \r\n" \
                                            " DELE RNFR RNTO STAT HELP\r\n" \
                                            "214 Help OK\r\n"); 
PROG_STR( FTP_REPLY_215_SYST_NAME,          "215 UNIX emulated\r\n" );
PROG_STR( FTP_REPLY_220_WELCOME,            "220- Alarm clock V3 FTP server (fw " FIRMWARE_VERSION ")\r\n" \
                                            "220- https://github.com/bfrigon/alarm-clock-v3\r\n" \
                                            "220 \r\n" );
PROG_STR( FTP_REPLY_221_DISCONNECT,         "221 Bye\r\n" );
PROG_STR( FTP_REPLY_226_LIST_END,           "226 Directory send OK, %hu matches found.\r\n" );
PROG_STR( FTP_REPLY_226_XFER_DONE,          "226 Transfer complete\r\n" );
PROG_STR( FTP_REPLY_226_DATA_CLOSED,        "226 Data connection closed\r\n" );
PROG_STR( FTP_REPLY_227_ENTER_PASV,         "227 Entering passive mode (%d,%d,%d,%d,%d,%d)\r\n" );
PROG_STR( FTP_REPLY_230_LOGGED_IN,          "230 Logged in\r\n" );
PROG_STR( FTP_REPLY_250_NEW_WD,             "250 Working directory changed to \"%s\"\r\n" );
PROG_STR( FTP_REPLY_250_LISTING,            "250- Listing \"%s\"\r\n\x20" );
PROG_STR( FTP_REPLY_250_END,                "250 End\r\n" );
PROG_STR( FTP_REPLY_250_DELETED,            "250 Deleted \"%s\".\r\n" );
PROG_STR( FTP_REPLY_250_RENAMED,            "250 Successfuly renamed to \"%s\".\r\n" );
PROG_STR( FTP_REPLY_257_CURRENT_DIR,        "257 \"%s\"\r\n" );
PROG_STR( FTP_REPLY_257_DIR_CREATED,        "257 \"%s\" created\r\n" );
PROG_STR( FTP_REPLY_331_NEED_PWD,           "331 Need password.\r\n" );
PROG_STR( FTP_REPLY_350_RNFR_ACCEPT,        "350 Rename from \"%s\", need RNTO command\r\n" );
PROG_STR( FTP_REPLY_421_SERVICE_BUSY,       "421 Number of simultaneous connections exceeded.\r\n" );
PROG_STR( FTP_REPLY_421_ALLOC_ERROR,        "421 Cannot allocate memory.\r\n" );
PROG_STR( FTP_REPLY_425_DATA_TIMEOUT,       "425 Data connection timeout.\r\n" );
PROG_STR( FTP_REPLY_425_DATA_DISABLED,      "425 Use PASV or PORT first.\r\n" );
PROG_STR( FTP_REPLY_450_CANT_RENAME,        "450 Cannot rename to \"%s\".\r\n" );
PROG_STR( FTP_REPLY_450_CANT_WRITE,         "450 Cannot open \"%s\" for writing.\r\n" );
PROG_STR( FTP_REPLY_500_UNKNOWN_COMMAND,    "500 Unknown command.\r\n" );
PROG_STR( FTP_REPLY_500_UNKNOWN_SITE_CMD,   "500 Unknown SITE command.\r\n" );
PROG_STR( FTP_REPLY_500_SYNTAX_ERROR,       "500 Systax error.\r\n" );
PROG_STR( FTP_REPLY_501_PARAM_REQUIRED,     "501 Parameter required\r\n" );
PROG_STR( FTP_REPLY_501_INCORRECT_PARAM,    "501 Incorrect parameters\r\n" );
PROG_STR( FTP_REPLY_502_NOT_IMPLEMENTED,    "502 Command not implemented.\r\n" );
PROG_STR( FTP_REPLY_503_BAD_SEQUENCE,       "503 Bad sequence.\r\n" );
PROG_STR( FTP_REPLY_504_NOT_SUPPORTED,      "504 Not supported.\r\n" );
PROG_STR( FTP_REPLY_504_SITE_FUNC_UNKNOWN,  "504 Unknown site function.\r\n" );
PROG_STR( FTP_REPLY_530_USER_NOT_FOUND,     "530 User not found.\r\n" );
PROG_STR( FTP_REPLY_530_WRONG_PASSWORD,     "530 Incorrect password.\r\n" );
PROG_STR( FTP_REPLY_530_NOT_LOGGED_IN,      "530 Not logged in.\r\n" );
PROG_STR( FTP_REPLY_550_NO_SDCARD,          "550 No SD card present.\r\n" );
PROG_STR( FTP_REPLY_550_DIR_NOT_FOUND,      "550 Directory \"%s\" not found \r\n" );
PROG_STR( FTP_REPLY_550_FILE_NOT_FOUND,     "550 Cannot find \"%s\".\r\n" );
PROG_STR( FTP_REPLY_550_CANT_OPEN_DIR,      "550 Cannot open directory \"%s\".\r\n" );
PROG_STR( FTP_REPLY_550_NOT_A_DIR,          "550 \"%s\" is not a directory.\r\n" );
PROG_STR( FTP_REPLY_550_CANNOT_CREATE,      "550 Cannot create \"%s\".\r\n" );
PROG_STR( FTP_REPLY_550_CANNOT_REMOVE,      "550 Cannot remove \"%s\".\r\n" );
PROG_STR( FTP_REPLY_550_DELETE_FAILED,      "550 Cannot delete \"%s\".\r\n" );
PROG_STR( FTP_REPLY_550_CANT_GET_TIMESTAMP, "550 Cannot get timestamp for \"%s\".\r\n" );
PROG_STR( FTP_REPLY_550_CANT_SET_TIMESTAMP, "550 Cannot set timestamp for \"%s\".\r\n" );
PROG_STR( FTP_REPLY_553_NEED_RNFR,          "553 Need RNFR before RNTO.\r\n" );
PROG_STR( FTP_REPLY_553_ALREADY_EXIST,      "553 \"%s\" already exists.\r\n" );                                           

/* FTP commands */
PROG_STR( FTP_COMMAND_NOOP,   "NOOP" );     /* Keep alive */
PROG_STR( FTP_COMMAND_AUTH,   "AUTH" );     /* not implemented */
PROG_STR( FTP_COMMAND_USER,   "USER" );     /* Send login username */
PROG_STR( FTP_COMMAND_PASS,   "PASS" );     /* Send login password */
PROG_STR( FTP_COMMAND_SYST,   "SYST" );     /* Request system name */
PROG_STR( FTP_COMMAND_MODE,   "MODE" );     /* Set transfer mode */
PROG_STR( FTP_COMMAND_STRU,   "STRU" );     /* Set file structure */
PROG_STR( FTP_COMMAND_TYPE,   "TYPE" );     /* Set representation type*/
PROG_STR( FTP_COMMAND_FEAT,   "FEAT" );     /* Get a list of supported extended features */
PROG_STR( FTP_COMMAND_QUIT,   "QUIT" );     /* Disconnect */
PROG_STR( FTP_COMMAND_PASV,   "PASV" );     /* Enter passive transfer mode (client->server) */
PROG_STR( FTP_COMMAND_PORT,   "PORT" );     /* Enter active transfer mode (server->client) */
PROG_STR( FTP_COMMAND_ABOR,   "ABOR" );     /* Abort current transfer */
PROG_STR( FTP_COMMAND_LIST,   "LIST" );     /* Get UNIX type directory listing */
PROG_STR( FTP_COMMAND_NLST,   "NLST" );     /* Get list of file names */
PROG_STR( FTP_COMMAND_MLSD,   "MLSD" );     /* Get machine readable listing of a directory */
PROG_STR( FTP_COMMAND_MLST,   "MLST" );     /* Get machine readable listing for a single file */
PROG_STR( FTP_COMMAND_RETR,   "RETR" );     /* Download a file */
PROG_STR( FTP_COMMAND_STOR,   "STOR" );     /* Upload a file, overwrite existing file */
PROG_STR( FTP_COMMAND_APPE,   "APPE" );     /* Upload a file, append existing file */
PROG_STR( FTP_COMMAND_CWD,    "CWD"  );     /* Change working directory */
PROG_STR( FTP_COMMAND_CDUP,   "CDUP" );     /* Go to parent directory */
PROG_STR( FTP_COMMAND_PWD,    "PWD"  );     /* Get current working directory */
PROG_STR( FTP_COMMAND_MKD,    "MKD"  );     /* Create a sub-directory */
PROG_STR( FTP_COMMAND_DELE,   "DELE" );     /* Delete a file */
PROG_STR( FTP_COMMAND_RMD,    "RMD"  );     /* Remove a directory*/
PROG_STR( FTP_COMMAND_RNFR,   "RNFR" );     /* Set the file to rename, followed by RNTO */
PROG_STR( FTP_COMMAND_RNTO,   "RNTO" );     /* Rename the file */
PROG_STR( FTP_COMMAND_SIZE,   "SIZE" );     /* Get a file size */
PROG_STR( FTP_COMMAND_MDTM,   "MDTM" );     /* Get a file modification date/time */
PROG_STR( FTP_COMMAND_MFMT,   "MFMT" );     /* Set a file modification date/time */
PROG_STR( FTP_COMMAND_STAT,   "STAT" );     /* Get the status of a file or directory */
PROG_STR( FTP_COMMAND_SITE,   "SITE" );     /* Site functions (HELP, ZONE) */
PROG_STR( FTP_COMMAND_HELP,   "HELP" );     /* Display implemented commands */

/* Control and data connection state */
enum {
    FTP_STATE_DISABLED = 0,                 /* Socket closed */
    FTP_STATE_IDLE,                         /* For data connection, accepted PORT command, waiting */
    FTP_STATE_WAIT_WIFI_CONNECTION,         /* Wait for WIFI connection to establish */
    FTP_STATE_SOCKET_REQ_BIND,              /* Socket created, requesting binding to a specific port */
    FTP_STATE_SOCKET_REQ_LISTEN,            /* Bind successful, requesting listening mode */
    FTP_STATE_LISTENING,                    /* Listening socket ready, waiting for client */
    FTP_STATE_CONNECTING,                   /* Connecting to client in active mode */
    FTP_STATE_CONNECTED,                    /* Client */
};

/* Authentication status */
enum {
    FTP_AUTH_STATE_NONE = 0,                /* No username specified */
    FTP_AUTH_STATE_USER_OK,                 /* Username accepted, waiting password */
    FTP_AUTH_STATE_AUTHORIZED,              /* Authentication successful */
};

/* FTP Tasks */
enum {
    FTP_TASK_UNIX_LISTING = 1,              /* Generate a directory listing (UNIX format) */
    FTP_TASK_MACHINE_LISTING,               /* Generate a directory listing (machine readable format, RFC3659) */
    FTP_TASK_NAME_LISTING,                  /* Generate a directory listing (filenames only) */
    FTP_TASK_DIR_STAT,                      /* Generate a directory listing, but print it to the control connection. */
    FTP_TASK_DOWNLOAD,                      /* Send a file to the client. */
    FTP_TASK_UPLOAD,                        /* Receive a file from the client. */
};

/* Directory listing types */
enum {
    FTP_LIST_TYPE_MACHINE = 0,              /* Machine readable format (RFC3659 - MLSD)*/
    FTP_LIST_TYPE_UNIX,                     /* Unix format */
    FTP_LIST_TYPE_NAMES,                    /* File name list only */
};



/*******************************************************************************
 *
 * @brief   Provides access to SD card files via FTP.
 * 
 *******************************************************************************/
class FTPServer : private ITask {

  public:
    FTPServer( SDCardManager* sdcard );
    void runTasks();
    bool startServer();
    void stopServer();
    bool clientConnected();
    bool serverEnabled();
    void enableServer( bool enabled );
    void printServerStatus( ConsoleBase *console );

  private:
    void handleControlConnState();
    void handleDataConnState();
    bool setWorkingDirectory( const char *dirname );
    bool checkIncommingPacket();
    void processIncommingCommand( char *command, char *param );
    bool beginPassiveDataMode();
    bool beginActiveDataMode();
    void endDataMode();
    char* getLongFilename( FsFile *file );
    void sendDirectoryEntry( TCPClient *client, FsFile *file, char *filename = nullptr, uint8_t listType = FTP_LIST_TYPE_MACHINE );
    bool sendResponse( const char *msg, ... );
    void closeSession();
    char* uint64tostr( uint64_t value );
    bool parseParamTimestamp( char **buffer, uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second );
    
    SOCKET _listenerControl;
    SOCKET _listenerData;
    TCPClient _control;
    TCPClient _data;
    uint8_t _stateControl;
    uint8_t _stateData;
    uint16_t _dataPort;
    IPAddress _dataHost;
    bool _serverEnabled;
    uint8_t _stateAuth;
    SDCardManager* _sdcard;
    char* _workingDir;
    FsFile _currentFile;
    uint16_t _nmatches;
};

/* FTP Server */
extern FTPServer g_ftpServer;

#endif /* FTPSERVER_H */