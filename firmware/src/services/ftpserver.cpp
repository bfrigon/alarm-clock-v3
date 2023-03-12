//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/ftpserver.cpp
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

#include "ftpserver.h"
#include "logger.h"
#include <time.h>
#include <drivers/rtc.h>
#include <timezone.h>


/*******************************************************************************
 *
 * @brief   Class constructor
 *
 */
FTPServer::FTPServer( SDCardManager* sdcard ) {
    
    _sdcard = sdcard;
    _stateControl = FTP_STATE_WAIT_WIFI_CONNECTION;
    _stateData = FTP_STATE_DISABLED;
    _serverEnabled = false;
    _stateAuth = FTP_AUTH_STATE_NONE;
    _workingDir = nullptr;
    _listenerControl = -1;
    _listenerData = -1;
    _dataHost = INADDR_NONE;
    _dataPort = 0;
    _nmatches = 0;
}


/*******************************************************************************
 *
 * @brief   Convert a 64-bit unsigned integer to a character array to be used
 *          for printf functions since it doesn't support 64 bits integer.
 * 
 * @param   value  uint64_t variable to convert
 * 
 * @return  Pointer to the result character array. 
 *
 */
char* FTPServer::uint64tostr( uint64_t value ) {
  
    static char buffer[ 22 ];
    char* ptr = &buffer[ sizeof( buffer ) -1 ];
    
    *ptr = '\0';
    do {
        *--ptr = '0' + ( value % 10 );
        value /= 10;
    } while ( value > 0 );
    return ptr;
}


/*******************************************************************************
 *
 * @brief   Parse a timestamp argument. If a valid timestamp format is found, 
 *          advance the parameter pointer to the next argument. 
 *          Accept the following format YYYYMMDDhhmmss or YYYYMMDD.
 * 
 * @param   param   Pointer to the pointer of the parameter buffer.
 * @param   year    Pointer to a variable where to store the parsed year.
 * @param   month   Pointer to a variable where to store the parsed month.
 * @param   day     Pointer to a variable where to store the parsed day.
 * @param   hour    Pointer to a variable where to store the parsed hour.
 * @param   min     Pointer to a variable where to store the parsed minute.
 * @param   sec     Pointer to a variable where to store the parsed seconds.
 * 
 * @return  TRUE if a valid timestamp was found, FALSE otherwise.
 * 
 */
bool FTPServer::parseParamTimestamp( char **param, uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec ) {

    *year = 0;
    *month = 0;
    *day = 0;
    *hour = 0;
    *min = 0;
    *sec = 0;

    /* Get the length of the timestamp parameter */
    uint8_t length = 0;
    char *ptr = *param;
    while( isdigit( *ptr ) == true ) {
        length++;
        ptr++;
    }

    /* Digits must be followed by a space */
   if( isspace( *ptr ) == false ) {
    	return false;
    }

    /* Invalid datetime format */
    if( length != 8 && length != 14 ) {
        return false;
    }

    if( length > 8 ) {

        *( *param + 14 ) = '\0';
        *sec = constrain( atoi( *param + 12 ), 0, 59 );
        
        *( *param + 12 ) = '\0';
        *min = constrain( atoi( *param + 10 ), 0, 59 );
        
        *( *param + 10 ) = '\0';
        *hour = constrain( atoi( *param + 8 ), 0, 23 );
    }

    *( *param + 8 ) = '\0';
    *day = constrain( atoi( *param + 6 ), 1, 31 );
    
    *( *param + 6 ) = '\0';
    *month = constrain( atoi( *param + 4 ), 1, 12 );
    
    *( *param + 4 ) = '\0';
    *year = atoi( *param );

    /* Set parameter pointer to the next parameter */
    *param += length + 1;

    return true;
}

/*******************************************************************************
 *
 * @brief   Send a response message on the control connection.
 * 
 * @param   msg Formatted message to send.
 * @param   ... Arguments
 * 
 * @return  TRUE if connected or FALSE otherwise.
 * 
 */
bool FTPServer::sendResponse( const char *msg, ... ) {
    
    va_list args;
    va_start( args, msg );

    char *buffer = nullptr;
    int msgLength;

    if(( msgLength = vasprintf_P( &buffer, msg, args )) < 0 ) {

        if( buffer != nullptr ) {
            free( buffer );
        }

        return false;
    }

    va_end( args );

    int len = _control.write( buffer, msgLength );

    free( buffer );

    return ( len == msgLength );
}


/*******************************************************************************
 *
 * @brief   End current FTP session
 * 
 */
void FTPServer::closeSession() {
    _control.stop();
    

    _stateAuth = FTP_AUTH_STATE_NONE;

    if( _workingDir != nullptr ) {
        free( _workingDir );
        _workingDir = nullptr;
    }

    g_log.add( EVENT_FTP_CLIENT_DISCONNECT, false );

    /* Client disconnected, go back to listening. */
    _stateControl = FTP_STATE_LISTENING;
    
    /* Disable passive mode data server */
    this->endDataMode();
}


/*******************************************************************************
 *
 * @brief   Return whether or not if a client is currently connected.
 * 
 * @return  TRUE if connected or FALSE otherwise.
 * 
 */
bool FTPServer::clientConnected() {
    return _control.connected();
}


/*******************************************************************************
 *
 * @brief   Return whether or not if the server is currently enabled.
 * 
 * @return  TRUE if enabled or FALSE otherwise.
 * 
 */
bool FTPServer::serverEnabled() {
    return _serverEnabled;
}


/*******************************************************************************
 *
 * @brief   Create the server socket.
 * 
 * @return  TRUE if successful or FALSE otherwise.
 * 
 */
bool FTPServer::startServer() {
    if( _stateControl != FTP_STATE_WAIT_WIFI_CONNECTION ) {
        return false;
    }

    if( g_wifi.connected() == false ) {
        return false;
    }

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = _htons( FTP_PORT );
    addr.sin_addr.s_addr = 0;

    /* Close previous listener socket */
    if( _listenerControl != -1 && g_wifisocket.listening( _listenerControl )) {

        g_wifisocket.close( _listenerControl );
    }

    /* Create the control listener socket */
    if(( _listenerControl = g_wifisocket.create( AF_INET, SOCK_STREAM, 0 )) < 0 ) {

        return false;
    }

    _stateControl = FTP_STATE_SOCKET_REQ_BIND;

    /* Bind the port to the listening socket */
    if( g_wifisocket.requestBind( _listenerControl, (struct sockaddr *)&addr, sizeof( struct sockaddr_in )) == false ) {
        this->stopServer();

        return false;
    }

    return true;
}


/*******************************************************************************
 *
 * @brief   Disconnect the client and stop accepting connections.
 * 
 * @return  TRUE if successful or FALSE otherwise.
 * 
 */
void FTPServer::stopServer() {

    /* close ftp connection */
    this->closeSession();

    /* Close the control listener socket */
    if( _listenerControl != -1 ) {

        g_wifisocket.close( _listenerControl );
        _listenerControl = -1;
    }

    if( _stateControl == FTP_STATE_CONNECTED ) {

        g_log.add( EVENT_FTP_CLIENT_DISCONNECT );
    }

    _stateControl = FTP_STATE_WAIT_WIFI_CONNECTION;
}


/*******************************************************************************
 *
 * @brief   Sets whether or not the server is enabled.
 * 
 * @param   enabled    TRUE to accept connections, FALSE otherwise.
 * 
 */
void FTPServer::enableServer( bool enabled ) {
    if( _serverEnabled == enabled ) {
        return;
    }

    _serverEnabled = enabled;

    if( enabled ) {
        _stateControl = FTP_STATE_WAIT_WIFI_CONNECTION;

    } else {
        stopServer();
    }

    if( enabled ) {
        g_log.add( EVENT_FTP_SERVICE_ENABLED );
        
    } else {
        g_log.add( EVENT_FTP_SERVICE_DISABLED );
    }
}


/*******************************************************************************
 *
 * @brief   Begin passive transfer mode. Open a socket for listening on a random 
 *          port and wait for client data connection.
 * 
 * @return  TRUE if successful or FALSE otherwise.
 * 
 */
bool FTPServer::beginPassiveDataMode() {

    if( g_wifi.connected() == false ) {
        return false;
    }

    /* Close current data connection is still active */
    this->endDataMode();

    /* Generate random port number */
    randomSeed( analogRead( PIN_A0 ));
    _dataPort = random( 50000, 59999 );
    _dataHost = INADDR_NONE;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = _htons( _dataPort );
    addr.sin_addr.s_addr = 0;

    /* Init socket */
    if(( _listenerData = g_wifisocket.create( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
        return false;
    }

    _stateData = FTP_STATE_SOCKET_REQ_BIND;

    if( g_wifisocket.requestBind( _listenerData, (struct sockaddr *)&addr, sizeof( struct sockaddr_in )) == false ) {
        this->endDataMode();

        return false;
    }

    return true;
}


/*******************************************************************************
 *
 * @brief   Begin active transfer mode. Connect to the client using the 
 *          host address and port it specified with the PORT command.
 * 
 * @return  TRUE if successful or FALSE otherwise.
 * 
 */
bool FTPServer::beginActiveDataMode() {

    if( _dataHost == INADDR_NONE || _dataPort == 0 ) {
        return false;
    }

    /* Close current data connection is still active */
    this->endDataMode();

    if( _data.connect( _dataHost, _dataPort ) == 0 ) {
        return false;
    }

    _stateData = FTP_STATE_CONNECTING;
    return true;
}


/*******************************************************************************
 *
 * @brief   Stop data connection and close socket.
 * 
 */
void FTPServer::endDataMode() {

    /* Close client data connection */
    _data.stop();
    _stateData = FTP_STATE_DISABLED;

    /* Close data listening socket */
    if( _listenerData != -1 ) {
        g_wifisocket.close( _listenerData );
        _listenerData = -1;
    }
}


/*******************************************************************************
 *
 * @brief   Process FTP commands received from the client
 * 
 * @param   command  Command name
 * @param   param    Command parameter
 *
 */
void FTPServer::processIncommingCommand( char *command, char *param ) {

    /* QUIT : Disconnect request */
    /* ------------------------- */
    if( strcasecmp_P( command, FTP_COMMAND_QUIT ) == 0 ) {
        this->sendResponse( FTP_REPLY_221_DISCONNECT );
        this->closeSession();
        return;

    /* HELP : Display implemented commands */
    /* ----------------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_HELP ) == 0 ) {

        this->sendResponse( FTP_REPLY_214_HELP );
        return;    

    /* AUTH : Not implemented */
    /* ----------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_AUTH ) == 0 ) {

        this->sendResponse( FTP_REPLY_502_NOT_IMPLEMENTED );
        return;

    /* NOOP : Keep-alive */
    /* ----------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_NOOP ) == 0 ) {

        this->sendResponse( FTP_REPLY_200_ZZZ );
        return;

    /* SYST : Request system name */
    /* -------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_SYST ) == 0 ) {

        this->sendResponse( FTP_REPLY_215_SYST_NAME );
        return;

    /* FEAT : Get extended feature list */
    /* -------------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_FEAT ) == 0 ) {

        this->sendResponse( FTP_REPLY_211_FEATURES );
        return;

    /* USER : Send auth username */
    /* ------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_USER ) == 0 ) {

        /* No authentication required */
        if( strlen( g_config.network.ftp_username ) == 0 ) {
            this->sendResponse( FTP_REPLY_230_LOGGED_IN );
            _stateAuth = FTP_AUTH_STATE_AUTHORIZED;
            return;
        }

       /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        if( strcmp( param, g_config.network.ftp_username ) == 0 ) {

            /* No password required */
            if( strlen( g_config.network.ftp_password ) == 0 ) {
                this->sendResponse( FTP_REPLY_230_LOGGED_IN );
                _stateAuth = FTP_AUTH_STATE_AUTHORIZED;

            /* Ask for a password */
            } else {
                this->sendResponse( FTP_REPLY_331_NEED_PWD );
                _stateAuth = FTP_AUTH_STATE_USER_OK;
            }
        } else {

            this->sendResponse( FTP_REPLY_530_USER_NOT_FOUND );
            this->closeSession();
            _stateAuth = FTP_AUTH_STATE_NONE;
        }
        return;

    /* PASS : Send auth password */
    /* ------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_PASS ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* Need command USER first */
        if( _stateAuth != FTP_AUTH_STATE_USER_OK ) {
            this->sendResponse( FTP_REPLY_503_BAD_SEQUENCE );
            return;
        }

        if( strcmp( param, g_config.network.ftp_password ) == 0 ) {

            this->sendResponse( FTP_REPLY_230_LOGGED_IN );
            _stateAuth = FTP_AUTH_STATE_AUTHORIZED;

        } else {

            this->sendResponse( FTP_REPLY_530_WRONG_PASSWORD );
            this->closeSession();
            _stateAuth = FTP_AUTH_STATE_NONE;
        }
        return;

    /* SITE : Site functions */
    /* --------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_SITE ) == 0 ) {
        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* List available commands */
        if( strcasecmp_P( param, PSTR( "HELP" )) == 0 ) {
            this->sendResponse( FTP_REPLY_214_SITE_HELP );
            return;    
        }

        /* Get timezone offset from UTC in minutes. */
        if( strcasecmp_P( param, PSTR( "ZONE" )) == 0 ) {

            DateTime now;
            now = g_rtc.now();

            int16_t offset;
            offset = ( g_timezone.isDST(&now) == true ) ? g_timezone.getDstUtcOffset() : g_timezone.getStdUtcOffset();

            this->sendResponse( FTP_REPLY_210_SITE_ZONE, offset );
            return;
        }

        this->sendResponse( FTP_REPLY_500_UNKNOWN_SITE_CMD );
        return;
    }


    /* --------------------------------------------------------------- */
    /* Commands bellow requires user authentication. */
    /* --------------------------------------------------------------- */
    if( _stateAuth != FTP_AUTH_STATE_AUTHORIZED ) {
        this->sendResponse( FTP_REPLY_530_NOT_LOGGED_IN );
        return;
    }
    

    /* TYPE : Representation type */
    /* -------------------------- */
    if( strcasecmp_P( command, FTP_COMMAND_TYPE ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        if( strcasecmp( param, "I") == 0 ) {
            this->sendResponse( FTP_REPLY_200_TYPE_BINARY );
        } else if( strcasecmp( param, "A") == 0 ) {
            this->sendResponse( FTP_REPLY_200_TYPE_ASCII );
        } else {
            this->sendResponse( FTP_REPLY_504_NOT_SUPPORTED );
        }
        return;

    /* MODE : Transfer mode */
    /* -------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_MODE ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }
        
        /* Only support STREAM mode */
        if( strcasecmp( param, "S") == 0 ) {
            this->sendResponse( FTP_REPLY_200_MODE_STREAM );
        } else {
            this->sendResponse( FTP_REPLY_504_NOT_SUPPORTED );
        }
        return;

    /* STRU : File structure */
    /* --------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_STRU ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }
        
        /* Only support FILE structure */
        if( strcasecmp( param, "F") == 0 ) {
            this->sendResponse( FTP_REPLY_200_OK );
        } else {
            this->sendResponse( FTP_REPLY_504_NOT_SUPPORTED );
        }
        return;

    /* STAT: Server status */
    /* ------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_STAT ) == 0 && strlen( param ) == 0 ) {

        IPAddress ip;
        ip = _control.remoteIP();

        this->sendResponse( FTP_REPLY_211_SERVER_STAT, 
                            ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ],
                            g_config.network.ftp_username );
        return;

    /* ABOR: Abort current transfer */
    /* ---------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_ABOR ) == 0 ) {
        this->endDataMode();

        if( _currentFile.isOpen() == true ) {
            _currentFile.close();
        }

        if( this->getCurrentTask() != TASK_NONE ) {
            this->endTask( ERR_FTP_XFER_ABORTED );
        }

        this->sendResponse( FTP_REPLY_226_DATA_CLOSED );
        return;

    /* PASV : Enter passive data mode */
    /* ------------------------------------ */
    } else if( strcasecmp_P( command, FTP_COMMAND_PASV ) == 0 ) {

        this->beginPassiveDataMode();
        return;
    
    /* PORT : Enter active data mode */
    /* ------------------------------------ */
    } else if( strcasecmp_P( command, FTP_COMMAND_PORT ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        IPAddress host;
        uint16_t port = 0;

        char *ptr, *token = strtok_rP( param, PSTR( "," ), &ptr );
        uint8_t ntoken = 0;
        while(token != nullptr ) {

            if( ntoken < 4 ) {
                host[ ntoken ] = atoi( token );

            } else if( ntoken == 4 ) {
                port = atoi( token ) << 8;

            } else if( ntoken == 5 ) {
                port += atoi( token );

            } else {
                break;
            }

            token = strtok_rP( nullptr, PSTR( "," ), &ptr );
            ntoken++;
        }

        if( ntoken < 6 ) {
            this->sendResponse( FTP_REPLY_501_INCORRECT_PARAM );
            return;
        }

        _dataPort = port;
        _dataHost = host;
        _stateData = FTP_STATE_IDLE;

        this->sendResponse( FTP_REPLY_200_OK );

        return;
    }


    /* --------------------------------------------------------------- */
    /* Commands bellow requires an SD card present */
    /* --------------------------------------------------------------- */
    if( g_sdcard.isCardPresent() == false ) {
        this->sendResponse( FTP_REPLY_550_NO_SDCARD );
        return;
    }

    /* CWD: Change working directory */
    /* ----------------------------- */
    if( strcasecmp_P( command, FTP_COMMAND_CWD ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }
        
        if( this->setWorkingDirectory( param ) == false ) {
            this->sendResponse( FTP_REPLY_550_DIR_NOT_FOUND, param );
            return;
        }

        this->sendResponse( FTP_REPLY_250_NEW_WD, _workingDir );
        return;

    /* CDUP: Change working directory to parent directory */
    /* -------------------------------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_CDUP ) == 0 ) {
        
        if( this->setWorkingDirectory( ".." ) == false ) {
            this->sendResponse( FTP_REPLY_550_DIR_NOT_FOUND, param );
            return;
        }

        this->sendResponse( FTP_REPLY_250_NEW_WD, _workingDir );
        return;

    /* PWD: Get current working directory */
    /* ---------------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_PWD ) == 0 ) {
        
        this->sendResponse( FTP_REPLY_257_CURRENT_DIR, _workingDir );
        return;

    /* SIZE: Get the size of a file */
    /* ---------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_SIZE ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
        directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        FsFile file;
        if( file.open( param, O_READ ) == false ) {
            this->sendResponse( FTP_REPLY_550_FILE_NOT_FOUND, param );
            return;
        }

        this->sendResponse( FTP_REPLY_213_SIZE, this->uint64tostr( file.size() ));
        file.close();

        return;

    /* STAT: File status */
    /* ----------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_STAT ) == 0 && strlen( param ) != 0 ) {

        char *path = param;
        
        /* Some FTP clients send '-a' to force listing hidden files, which is against 
           the FTP specs. Always show hidden files. Ignore option and set path to the 
           working directory */
        if( path[0] == '-' && strcasecmp_P( command, FTP_COMMAND_LIST ) == 0 ) {
            path = _workingDir;
        }

        /* If dir is not an absolute directory, check if the working 
           directory is still accessible. */
        if( path[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        /* List current working directory */
        if( strcmp_P( path, PSTR( "." )) == 0 ) {
            path = _workingDir;
        }

        if( _currentFile.open( path ) == false ) {
            this->sendResponse( FTP_REPLY_550_FILE_NOT_FOUND, path );
            return;
        }

        if( _currentFile.isDir() == true ) {

            this->sendResponse( FTP_REPLY_212_DIR_STAT_FOLLOW );

            this->startTask( FTP_TASK_DIR_STAT );
            _nmatches = 0;

        } else {
            this->sendResponse( FTP_REPLY_213_FILE_STAT_FOLLOW );
            this->sendDirectoryEntry( &_control, &_currentFile, nullptr, FTP_LIST_TYPE_UNIX );
            this->sendResponse( FTP_REPLY_213_FILE_STAT_END );
        }
        
        return;    

    /* LIST, MLSD, NLST: List directory content */
    /* ---------------------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_LIST ) == 0 
              || strcasecmp_P( command, FTP_COMMAND_MLSD ) == 0 
              || strcasecmp_P( command, FTP_COMMAND_NLST ) == 0) {


        if( _stateData == FTP_STATE_DISABLED ) {
            this->sendResponse( FTP_REPLY_425_DATA_DISABLED );
            return;
        }

        char* dir;
        if( strlen( param ) != 0 ) {
            dir = param;
        } else {
            dir = _workingDir;
        }

        /* Some FTP clients send '-a' to force listing hidden files, which is against 
           the FTP specs. Always show hidden files. Ignore option and set path to the 
           working directory */
        if( param[0] == '-' && strcasecmp_P( command, FTP_COMMAND_LIST ) == 0 ) {
            dir = _workingDir;
        }

        /* If dir is not an absolute directory, check if the working 
           directory is still accessible. */
        if( dir[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        if( _currentFile.open( dir ) == false ) {
            this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, dir );
            return;
        }

        if( _currentFile.isDir() == false ) {
            this->sendResponse( FTP_REPLY_550_NOT_A_DIR, dir );
            _currentFile.close();

            return;
        }

        /* Connect to client if active mode is used */
        if( _dataHost != INADDR_NONE ) {
            if( this->beginActiveDataMode() == false ) {

                this->sendResponse( FTP_REPLY_425_DATA_TIMEOUT );
                _currentFile.close();
                return;
            }
        }

        this->sendResponse( FTP_REPLY_150_XFER_READY );
        

        if( strcasecmp_P( command, FTP_COMMAND_LIST ) == 0 ) {
            this->startTask( FTP_TASK_UNIX_LISTING );

        } else if( strcasecmp_P( command, FTP_COMMAND_NLST ) == 0 ) {
            this->startTask( FTP_TASK_NAME_LISTING );

        } else {
            this->startTask( FTP_TASK_MACHINE_LISTING );
        }

        _nmatches = 0;
        
        return;

    /* MLST: Get machine listing for a single file or directory */
    /* -------------------------------------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_MLST ) == 0 ) {

        char* dir;
        if( strlen( param ) != 0 ) {
            dir = param;
        } else {
            dir = _workingDir;
        }

        /* If dir is not an absolute directory, check if the working 
           directory is still accessible. */
        if( dir[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        if( _currentFile.open( dir ) == false ) {
            this->sendResponse( FTP_REPLY_550_FILE_NOT_FOUND, dir );
            return;
        }

        this->sendResponse( FTP_REPLY_250_LISTING, dir );
        this->sendDirectoryEntry( &_control, &_currentFile, nullptr, FTP_LIST_TYPE_MACHINE );
        this->sendResponse( FTP_REPLY_250_END );
        return;

    /* RETR: Download file */
    /* ------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_RETR ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
           directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        /* Open the file to download */
        if( _currentFile.open( param ) == false ) {
            this->sendResponse( FTP_REPLY_550_FILE_NOT_FOUND, param );
            return;
        }

        /* Connect to client if active mode is used */
        if( _dataHost != INADDR_NONE ) {
            if( this->beginActiveDataMode() == false ) {

                this->sendResponse( FTP_REPLY_425_DATA_TIMEOUT );
                return;
            }
        }

        this->sendResponse( FTP_REPLY_150_XFER_READY );

        this->startTask( FTP_TASK_DOWNLOAD );
        return;

    /* STOR,APPE: Upload file */
    /* ---------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_STOR ) == 0 
               || strcasecmp_P( command, FTP_COMMAND_APPE ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        if( this->setWorkingDirectory( nullptr ) == false ) {
            this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
            return;
        }

        oflag_t flags;
        if( strcasecmp_P( command, FTP_COMMAND_STOR ) == 0 ) {
            flags = O_WRITE | O_CREAT;
        } else {
            flags = O_WRITE | O_APPEND;
        }

        if( _currentFile.open( param, flags ) == false ) {
            this->sendResponse( FTP_REPLY_450_CANT_WRITE, param );
            return;
        }

        /* Connect to client if active mode is used */
        if( _dataHost != INADDR_NONE ) {
            if( this->beginActiveDataMode() == false ) {

                this->sendResponse( FTP_REPLY_425_DATA_TIMEOUT );
                return;
            }
        }

        this->sendResponse( FTP_REPLY_150_OK_TO_SEND );

        this->startTask( FTP_TASK_UPLOAD );
        return;

    /* MKD: Create directory */
    /* --------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_MKD ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
           directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        /* Check if directory exists */
        if( _sdcard->exists( param ) == true ) {
            this->sendResponse( FTP_REPLY_553_ALREADY_EXIST, param );
            return;
        }

        /* Create directory */
        if( _sdcard->mkdir( param, true ) == false ) {
            this->sendResponse( FTP_REPLY_550_CANNOT_CREATE, param );
            return;
        }

        this->sendResponse( FTP_REPLY_257_DIR_CREATED, param );
        return;

    /* DELE: Delete file */
    /* ----------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_DELE ) == 0 ) {
        
        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
           directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        /* Check if file exists */
        SdFile file;
        if( file.open( param ) == false ) {
            this->sendResponse( FTP_REPLY_550_FILE_NOT_FOUND, param );
            return;
        }

        /* Delete the file or directory */
        if(( file.isDir() == true ? file.rmdir() : _sdcard->remove( param )) == true ) {
            this->sendResponse( FTP_REPLY_250_DELETED, param );
        } else {
            this->sendResponse( FTP_REPLY_550_DELETE_FAILED, param );
        }

        file.close();
        return;

    /* RMD: Remove directory */
    /* --------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_RMD ) == 0 ) {
        
        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
           directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        /* Check if file exists */
        if( _sdcard->exists( param ) == false ) {
            this->sendResponse( FTP_REPLY_550_DIR_NOT_FOUND, param );
            return;
        }

        /* Delete file */
        if( _sdcard->rmdir( param ) == true ) {
            this->sendResponse( FTP_REPLY_250_DELETED, param );
        } else {
            this->sendResponse( FTP_REPLY_550_CANNOT_REMOVE, param );
        }
        return;

    /* RNFR: Rename file from */
    /* ---------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_RNFR ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
           directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        /* Check if source file exists. */
        if( _currentFile.open( param ) == false ) {
            this->sendResponse( FTP_REPLY_550_FILE_NOT_FOUND, param );
            return;
        }        

        this->sendResponse( FTP_REPLY_350_RNFR_ACCEPT, param );
        return;

    /* RNTO: Rename file to */
    /* -------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_RNTO ) == 0 ) {

        /* Required parameter */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
           directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        /* File to be renamed must be opened with the command RNFR first. */
        if( _currentFile.isOpen() == false ) {
            this->sendResponse( FTP_REPLY_553_NEED_RNFR );
            return;
        }

        /* Rename the file */
        if( _currentFile.rename( param ) == true ) {
            this->sendResponse( FTP_REPLY_250_RENAMED, param );
        } else {
            this->sendResponse( FTP_REPLY_450_CANT_RENAME, param );
        }
        _currentFile.close();
        return;


    /* MDTM, MFMT: Get or set file last modified timestamp */
    /* -------------------------------------- */
    } else if( strcasecmp_P( command, FTP_COMMAND_MDTM ) == 0 
               || strcasecmp_P( command, FTP_COMMAND_MFMT ) == 0) {

        /* Parse the timestamp from the parameters */
        uint16_t year;
        uint8_t month, day, hour, min, sec;
        bool setTimestamp = this->parseParamTimestamp( &param, &year, &month, &day, &hour, &min, &sec );


        /* For MFMT command, both timestamp and filename are required parameters */
        if( setTimestamp == false && strcasecmp_P( command, FTP_COMMAND_MFMT ) == 0) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* Filename parameter is required */
        if( strlen( param ) == 0 ) {
            this->sendResponse( FTP_REPLY_501_PARAM_REQUIRED );
            return;
        }

        /* If param is not an absolute directory, check if the working 
        directory is still accessible. */
        if( param[0] != '/' ) {
            if( this->setWorkingDirectory( nullptr ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_OPEN_DIR, _workingDir );
                return;
            }
        }

        FsFile file;
        if( file.open( param, O_READ ) == false ) {
            this->sendResponse( FTP_REPLY_550_FILE_NOT_FOUND, param );
            return;
        }

        if( setTimestamp == true ) {

            /* Set the file last modified timestamp */
            if( file.timestamp( T_WRITE, year, month, day, hour, min, sec ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_SET_TIMESTAMP, param );
                return;
            }
            
        } else {

            /* Retreive the file last modified timestamp */
            uint16_t date, time;
            if( file.getModifyDateTime( &date, &time ) == false ) {
                this->sendResponse( FTP_REPLY_550_CANT_GET_TIMESTAMP, param );
                return;
            }

            year = FS_YEAR( date );
            month = FS_MONTH( date );
            day = FS_DAY( date );
            hour = FS_HOUR( time );
            min = FS_MINUTE( time );
            sec = FS_SECOND( time );
        }


        this->sendResponse( FTP_REPLY_213_MOD_TIMESTAMP, year, month, day, hour, min, sec );
        return;
    }

    /* Unknown command */
    this->sendResponse( FTP_REPLY_500_UNKNOWN_COMMAND );    
}


/*******************************************************************************
 *
 * @brief   Check receive buffer for incomming packets.
 * 
 * @return  TRUE if a packed was received and processed, FALSE otherwise.
 */
bool FTPServer::checkIncommingPacket() {
    
    if( _control.connected() == 0 || _control.available() == 0 ) {
        return false;
    }

    /* Allocate memory for the command buffer */
    size_t bufSize = _control.available();
    char* buffer = nullptr;
    
    if( bufSize <= MAX_FTP_CMD_LENGTH ) {
        buffer = ( char* )malloc( bufSize + 1 );
    }

    if( buffer == nullptr ) {
        this->sendResponse( FTP_REPLY_421_ALLOC_ERROR );
        g_log.add( EVENT_FTP_CANNOT_ALLOCATE_MEM );

        this->closeSession();
        return false;
    }

    
    if( _control.readBytes( buffer, bufSize ) != bufSize ) {
        free( buffer );
        return false;
    }
    buffer[ bufSize - 1 ] = 0x00;

    /* Remove trailing spaces and CRLF from buffer */
    char *p = buffer + strlen( buffer ) - 1;
    while( p >= buffer && isspace(( unsigned char )*p )) p--;
    p[ 1 ] = '\0';
    
    /* Empty buffer */
    if( strlen( buffer ) == 0 ) {
        this->sendResponse( FTP_REPLY_500_SYNTAX_ERROR );
        free( buffer );

        return false;
    }

    /* split command and parameter */
    char *command, *param;
    command = strtok_rP( buffer, PSTR( "\x20" ), &param );

    /* Remove leading spaces from parameter */
    while( isspace(( unsigned char )*param )) param++;
    
    /* Process the command */    
    this->processIncommingCommand( command, param );

    free( buffer );
    return true;
}


/*******************************************************************************
 *
 * @brief   Set the current working directory to the specified directory.
 * 
 * @param   dirname  Path to the directory
 * 
 * @return  TRUE if directory was successfully changed, FALSE otherwise.
 */
bool FTPServer::setWorkingDirectory( const char *dirname ) {

    char* newWorkingDir = nullptr;

    /* Set working directory to root if not set */
    if( _workingDir == nullptr ) {

        _workingDir = ( char* )malloc( 2 );
        if( _workingDir == nullptr ) {
            return false;
        }

        strcpy( _workingDir, "/" );
    }

    if( dirname != nullptr ) {
        
        char* pdst = ( char* )dirname;
        char* psrc = ( char* )dirname;

        while( *psrc != '\0' ) {

            /* Remove duplicate '/' characters from directory name */
            if( *psrc == '/' && *( psrc + 1 ) == '/' ) {
                
                psrc++;
                continue;
            }

            /* Remove the slash if it is the last character and if it is not the 
               only slash in the directory name */
            if( *psrc == '/' && *( psrc + 1 ) == '\0' && pdst != dirname ) {
                psrc++;
                continue;
            }

            *pdst = *psrc;
            psrc++;
            pdst++;
        }

        *pdst = '\0';
    }

    /* Set to current directory */
    if( dirname == nullptr || strcmp( dirname, "." ) == 0 ) {
        
        return _sdcard->chdir( _workingDir );

    /* Absolute directory */
    } else if( dirname[0] == '/' ) {

        asprintf( &newWorkingDir, "%s", dirname );

    /* Go to parent directory */
    } else if ( strcmp( dirname, ".." ) == 0 ) {

        char* ptr = strrchr( _workingDir, '/' );
        if( ptr != nullptr ) {
            *ptr = '\0';
        }

        if( strlen( _workingDir ) == 0 ) {
            strcpy( _workingDir, "/" );
        }

        return true;

    /* Relative directory */
    } else {

        /* Remove trailing '/ from current working dir */
        char* ptr = _workingDir + strlen( _workingDir ) - 1;
        while(ptr >= _workingDir && *ptr == '/') ptr--;
        ptr[1] = '\0';

        asprintf( &newWorkingDir, "%s/%s", _workingDir, dirname );

        if( strlen( _workingDir ) == 0 ) {
            strcpy( _workingDir, "/" );
        }
    }


    if( newWorkingDir == nullptr ) {
        return false;
    }

    /* Check if the requested directory exists */
    FsFile dir;
    if( dir.open( newWorkingDir ) == false ) {
        free( newWorkingDir );
        return false;
    }

    if( dir.isDir() == false ) {
        free( newWorkingDir );
        dir.close();

        return false;
    }

    dir.close();    

    /* Set working directory to the requested directory */
    if( _workingDir != nullptr ) {
        free( _workingDir );
    }
    _workingDir = newWorkingDir;

    return _sdcard->chdir( _workingDir );
}


/*******************************************************************************
 *
 * @brief   Create a buffer containing the long file name of a given FsFile object
 * 
 * @param   file  FsFile object
 * 
 * @return  Pointer to the buffer containing the long file name or nullptr if
 *          an error occured.
 */
char* FTPServer::getLongFilename( FsFile* file ) {

    /* Create a dummy print class to get the required buffer length */
    class DummyPrint : public Print {
        size_t write( uint8_t ) { return 1; }
    } dummy;

    size_t length = file->printName( &dummy );

    char* buffer = ( char* )malloc( length + 1);
    if( buffer == nullptr ) {
        return nullptr;
    }
    
    if( file->getName( buffer, length + 1 ) < length ) {
        free( buffer );
        return nullptr;
    }

    return buffer;
}


/*******************************************************************************
 *
 * @brief   Send a file listing entry to the specified TCP client.
 * 
 * @param   client    TCP client to send data to.
 * @param   file      FsFile object used to generate listing entry
 * @param   filename  Override filename to display in the listing entry
 * @param   listType  Type of listing to send (FTP_LIST_TYPE_MACHINE, FTP_LIST_TYPE_UNIX, FTP_LIST_TYPE_NAMES)
 * 
 * @return  TRUE if successful, FALSE otherwise.
 */
void FTPServer::sendDirectoryEntry( TCPClient *client, FsFile* file, char* filename, uint8_t listType ) {

    char perms[11] = "-rw-rw-rw-";
    if( file->isDir() == true ) {
        perms[0] = 'd';
        perms[3] = 'x';
        perms[6] = 'x';
        perms[9] = 'x';
    }

    if( file->isReadOnly() == true ) {
        perms[1] = '-';
        perms[4] = '-';
        perms[7] = '-';
    }

    /* Get the file last modify time (UTC) */
    uint16_t date, time;
    file->getModifyDateTime( &date, &time );
    DateTime mdate( FS_YEAR( date ), FS_MONTH( date ), FS_DAY( date ), 
                    FS_HOUR( time ), FS_MINUTE( time ), FS_SECOND( time ));

    /* Get the file creation time (UTC) */
    file->getCreateDateTime( &date, &time );
    DateTime cdate( FS_YEAR( date ), FS_MONTH( date ), FS_DAY( date ), 
                    FS_HOUR( time ), FS_MINUTE( time ), FS_SECOND( time ));


    /* Get long file name */
    char* pfname;
    if( filename != nullptr ) {
        pfname = filename;
    } else {
        pfname = this->getLongFilename( file );
    }

    if( pfname == nullptr ) {
        return;
    }

    size_t length;
    char* buffer;

    /* Standard listing (UNIX ls format) */
    if( listType == FTP_LIST_TYPE_UNIX ) {
        DateTime now = g_rtc.now();

        /* Convert UTC to local time for standard listing */
        g_timezone.toLocal( &now );
        g_timezone.toLocal( &mdate );

        /* If more than 6 months in the past, display the month, day and year only OR 
           display the month, day, hour and minute if more recent. */
        if(( now - 16070400 ) > mdate ) {

            length = asprintf_P( &buffer, PSTR( "%s  1 %s ftpusers %13s %S %2d %5d %s\r\n" ), 
                            perms, g_config.network.ftp_username, this->uint64tostr( file->fileSize() ),
                            getMonthName( mdate.month(), true ), mdate.day(), mdate.year(), 
                            pfname );
        } else {
            length = asprintf_P( &buffer, PSTR( "%s  1 %s ftpusers %13s %S %2d %02d:%02d %s\r\n" ), 
                            perms, g_config.network.ftp_username, this->uint64tostr( file->fileSize() ),
                            getMonthName( mdate.month(), true ), mdate.day(), mdate.hour(), mdate.minute(),
                            pfname );
        }
        
    /* Name list only (NLST) */
    } else if( listType == FTP_LIST_TYPE_NAMES ) {
        length = asprintf_P( &buffer, PSTR( "%s\r\n" ), pfname );

    /* Machine listing type (MLSD/MLST) */
    } else {

        const char *fmt = PSTR( "Type=%S;Size=%s;Modify=%d%02d%02d%02d%02d00;Create=%d%02d%02d%02d%02d00;"
                                "Perm=%s;UNIX.owner=%s;UNIX.group=ftpusers %s\r\n" );
        
        length = asprintf_P( &buffer, fmt, 
                             file->isDir() == true ? PSTR( "dir") : PSTR( "file" ),
                             this->uint64tostr( file->fileSize() ),
                             mdate.year(), mdate.month(), mdate.day(), mdate.hour(), mdate.minute(),
                             cdate.year(), cdate.month(), cdate.day(), cdate.hour(), cdate.minute(),
                             perms, g_config.network.ftp_username, pfname );      
    }
    
    client->write( buffer, length );

    free( buffer );

    if( filename == nullptr ) {
        free( pfname );
    }
}


/*******************************************************************************
 *
 * @brief   Prints server status on the console.
 * 
 * @param   console    ConsoleBase object to print results to.
 * 
 */
void FTPServer::printServerStatus( ConsoleBase *console ) {

    console->println_P( _serverEnabled == true ? S_CONSOLE_FTP_ENABLED : S_CONSOLE_FTP_DISABLED );

    if( g_wifi.connected() == false ) {
        console->println_P( S_CONSOLE_NET_NOT_CONNECTED );

    } else {

        if( _serverEnabled == true ) {
            if( _control.connected() ) {

                IPAddress ip;
                ip = _control.remoteIP();

                console->printf_P( S_CONSOLE_FTP_SESS_ACTIVE, ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ], _control.remotePort() );
                console->println();
                
            } else {

                console->println_P( S_CONSOLE_FTP_NO_SESS );
            }
        }
    }
}


/*******************************************************************************
 *
 * @brief   Handle control connection events
 * 
 */
void FTPServer::handleControlConnState() {

    switch( _stateControl ) {

        /* Waiting for WiFi to be connected before starting the server. */
        case FTP_STATE_WAIT_WIFI_CONNECTION: {

            if( g_wifi.connected() == true ) {
                this->startServer();
            }

        }
        break;

        /* Wait for a confirmation that the server socket is bound. */
        case FTP_STATE_SOCKET_REQ_BIND: {

            if( _listenerControl < 0 || g_wifisocket.bound( _listenerControl ) == 0 ) {
                return;
            }

            _stateControl = FTP_STATE_SOCKET_REQ_LISTEN;

            if( g_wifisocket.requestListen( _listenerControl, 0 ) == false ) {
                this->stopServer();

                return;
            }
        }
        break;

        /* Wait for a confirmation that the server socket is listening. */
        case FTP_STATE_SOCKET_REQ_LISTEN: {

            if( g_wifisocket.listening( _listenerControl )) {

                _stateControl = FTP_STATE_LISTENING;
            }
        }
        break;

        /* Wait for clients to connect */
        case FTP_STATE_LISTENING: {

            SOCKET child;
            child = g_wifisocket.accepted( _listenerControl );

            /* No client available yet */
            if( child < 0 ) {
                return;
            }

            _stateControl = FTP_STATE_CONNECTED;
            _stateAuth = FTP_AUTH_STATE_NONE;
            _control = child;

            /* Set working directory to root */
            this->setWorkingDirectory( "/" );

            this->sendResponse( FTP_REPLY_220_WELCOME );

            g_log.add( EVENT_FTP_CLIENT_CONNECT, _control.remoteIP() );
        }
        break;

        /* Serve connected client */
        case FTP_STATE_CONNECTED: {
                
            if( _control.connected() == 0 ) {

                this->closeSession();
                return;
            }

            /* Check if other clients are trying to connect */
            SOCKET child;
            child = g_wifisocket.accepted( _listenerControl );

            /* If so, refuse connection */
            if( child > 0 ) {
                char buffer[ sizeof( FTP_REPLY_421_SERVICE_BUSY ) ];
                strcpy_P( buffer, FTP_REPLY_421_SERVICE_BUSY );

                g_wifisocket.write( child, (uint8_t*)buffer, sizeof( buffer ) - 1 );

                g_wifisocket.close( child );
            }

            /* Check for incomming commands */
            this->checkIncommingPacket();
        }
        break;
   }
}


/*******************************************************************************
 *
 * @brief   Handle data connection events
 * 
 */
void FTPServer::handleDataConnState() {

    if( _stateData == FTP_STATE_DISABLED ) {
        return;
    }


    switch( _stateData ) {
        
        /* Wait for a confirmation that the server socket is bound. */
        case FTP_STATE_SOCKET_REQ_BIND: {

            if( _listenerData < 0 || g_wifisocket.bound( _listenerData ) == 0 ) {
                return;
            }

            _stateData = FTP_STATE_SOCKET_REQ_LISTEN;

            if( g_wifisocket.requestListen( _listenerData, 0 ) == false ) {
                this->endDataMode();
                this->sendResponse( FTP_REPLY_425_DATA_TIMEOUT );

                this->endTask( ERR_FTP_NO_DATA_CONNECTION );
                return;
            }
        }
        break;

        /* Wait for a confirmation that the server socket is listening. */
        case FTP_STATE_SOCKET_REQ_LISTEN: {

            if( g_wifisocket.listening( _listenerData ) == 0 ) {
                return;
            }

            _stateData = FTP_STATE_LISTENING;

            IPAddress ip = g_wifi.getLocalIP();

            this->sendResponse( FTP_REPLY_227_ENTER_PASV, 
                                ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ], 
                                _dataPort >> 8, _dataPort & 0xFF );
        }
        break;

        /* Wait for clients to connect */
        case FTP_STATE_LISTENING: {

            SOCKET child;
            child = g_wifisocket.accepted( _listenerData );

            /* No client available yet */
            if( child < 0 ) {
                return;
            }

            _stateData = FTP_STATE_CONNECTED;
            _data = child;

            this->resetTaskTimer();
        }
        break;

        /* Wait for connection to client */
        case FTP_STATE_CONNECTING: {

            /* Connection not established yet. */
            if( _data.connected() == 0 ) {
                return;
            }

            _stateData = FTP_STATE_CONNECTED;
            
            this->resetTaskTimer();
        }
        break;

        /* Serve connected client */
        case FTP_STATE_CONNECTED: {
                
            /* Check if other clients are trying to connect */
            SOCKET child;
            child = g_wifisocket.accepted( _listenerData );

            /* If so, refuse connection */
            if( child > 0 ) {
                g_wifisocket.close( child );
            }
        }
        break;
    }
}


/*******************************************************************************
 *
 * @brief   Run server tasks.
 * 
 */
void FTPServer::runTasks() {
    if( _serverEnabled == false ) {
        return;
    }

    /* Stop server if WiFi connection is lost. */
    if( g_wifi.connected() == false && _stateControl != FTP_STATE_WAIT_WIFI_CONNECTION ) {
        
        this->stopServer();
        return;
    }

    /* Handle control connections */
    this->handleControlConnState();

    /* Handle data connections */
    this->handleDataConnState();

    /* No current task running */
    if( this->getCurrentTask() == TASK_NONE ) {
        return;
    }

    /* Waiting for data connection */
    if( _stateData == FTP_STATE_LISTENING || _stateData == FTP_STATE_CONNECTING ) {

        /* Data connection timeout */
        if( this->getTaskRunningTime() > FTP_DATA_CONNECT_TIMEOUT ) {

            this->endDataMode();
            _currentFile.close();

            this->sendResponse( FTP_REPLY_425_DATA_TIMEOUT );

            this->endTask( ERR_FTP_NO_DATA_CONNECTION );
            return;
        }

        /* Continue to wait */
        return;
    }    

    /* Execute current task */
    switch( this->getCurrentTask() ) {

        /* LIST, MSLD, NLST, STAT : Directory listing */
        /* -------------------------------------------------- */
        case FTP_TASK_UNIX_LISTING: 
        case FTP_TASK_DIR_STAT: {

            if( _nmatches == 0 ) {

                /* For STAT, the listing is sent over control connection 
                   instead of the data connection */
                TCPClient *client;
                client = ( this->getCurrentTask() == FTP_TASK_DIR_STAT ? &_control : &_data );

                this->sendDirectoryEntry( client, &_currentFile, ( char* )".", FTP_LIST_TYPE_UNIX );
                this->sendDirectoryEntry( client, &_currentFile, ( char* )"..", FTP_LIST_TYPE_UNIX );

                _nmatches += 2;
            }
        }

        /* Fall-through */
        case FTP_TASK_MACHINE_LISTING:
        case FTP_TASK_NAME_LISTING: {

            /* Go to the next file in the directory */
            FsFile file;
            file.openNext( &_currentFile, O_READ );

            /* End of directory listing, close data connection */
            if( file.isOpen() == false ) {
                
                this->endDataMode();
                _currentFile.close();

                if( this->getCurrentTask() == FTP_TASK_DIR_STAT ) {
                    this->sendResponse( FTP_REPLY_212_DIR_STAT_END );
                } else {
                    this->sendResponse( FTP_REPLY_226_LIST_END, _nmatches );
                }

                this->endTask( TASK_SUCCESS );
                return;
            }

            /* Generate directory list entry with the specified format */
            switch( this->getCurrentTask() ) {
                
                case FTP_TASK_DIR_STAT:

                    /* For STAT, the listing is sent over control connection 
                    instead of the data connection */
                    this->sendDirectoryEntry( &_control, &file, nullptr, FTP_LIST_TYPE_UNIX );
                    break;

                case FTP_TASK_UNIX_LISTING:
                    this->sendDirectoryEntry( &_data, &file, nullptr, FTP_LIST_TYPE_UNIX );
                    break;

                case FTP_TASK_NAME_LISTING:
                    this->sendDirectoryEntry( &_data, &file, nullptr, FTP_LIST_TYPE_NAMES );
                    break;

                default:
                    this->sendDirectoryEntry( &_data, &file, nullptr, FTP_LIST_TYPE_MACHINE );
                    break;
            }

            _nmatches++;
        }
        break;

        /* RETR : File download */
        /* -------------------------------------------------- */
        case FTP_TASK_DOWNLOAD: {
            
            /* Allocate the buffer for the next block */
            char* buffer = ( char* )malloc( min( MAX_FTP_TRANSFER_BUFFER, _currentFile.available() ));
            if( buffer == nullptr ) {

                this->endDataMode();
                _currentFile.close();

                this->sendResponse( FTP_REPLY_421_ALLOC_ERROR );

                this->endTask( ERR_FTP_ALLOCATE_ERROR );
                return;
            }

            /* Read the file block */
            size_t nread = _currentFile.read( buffer, min( MAX_FTP_TRANSFER_BUFFER, _currentFile.available() ));

            if( nread > 0 ) {

                /* Send the file data */
                _data.write( buffer, nread );
                free( buffer );

            } else {
                free( buffer );

                this->endDataMode();
                _currentFile.close();

                this->sendResponse( FTP_REPLY_226_XFER_DONE );
                
                this->endTask( TASK_SUCCESS );
                return;
            }
        }
        break;
            
        /* STOR, APPE : File upload */
        /* -------------------------------------------------- */
        case FTP_TASK_UPLOAD: {

            int nbytes;
            nbytes = min( _data.available(), MAX_FTP_TRANSFER_BUFFER );

            if( nbytes > 0 ) {

                /* Allocate the buffer for the next block */
                char* buffer = ( char* )malloc( nbytes );
                if( buffer == nullptr ) {

                    this->endDataMode();
                    _currentFile.close();

                    this->sendResponse( FTP_REPLY_421_ALLOC_ERROR );

                    this->endTask( ERR_FTP_ALLOCATE_ERROR );
                    return;
                }

                /* Read from client */
                nbytes = _data.read(( uint8_t * )buffer, nbytes );

                /* Write data to the file */
                if( nbytes > 0 ) {
                    _currentFile.write( buffer, nbytes );
                }

                free( buffer );
            }

            if( _data.connected() == 0 ) {

                this->endDataMode();
                _currentFile.close();

                this->sendResponse( FTP_REPLY_226_XFER_DONE );

                this->endTask( TASK_SUCCESS );
            }
        }
        break;
    }
}