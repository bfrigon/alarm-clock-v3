//******************************************************************************
//
// Project : Alarm Clock V3
// File    : src/services/console.h
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

#include "../console/console_base.h"



class Console : public ConsoleBase {
  public:
    void runTasks();
    void begin( unsigned long baud);
    void resetConsole();

  private:
    size_t _print( char c );
    int _read();
    int _peek();
    int _available();

    void exitConsole( bool timeout );
    
};

extern Console g_console;


#endif /* CONSOLE_H */