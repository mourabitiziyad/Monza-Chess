//
//  time.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 27/4/2022.
//

#include "time.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;

#if defined(WIN64) || defined(WIN32)
    #include <windows.h>
#else
    # include <sys/time.h>
#endif


int get_time_ms()
{
    #if defined(WIN64) || defined(WIN32)
        return GetTickCount();
    #else
        struct timeval time_value;
        gettimeofday(&time_value, NULL);
        return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    #endif
}

int input_waiting()
{
    #ifndef WIN32
        fd_set readfds;
        struct timeval tv;
        FD_ZERO (&readfds);
        FD_SET (fileno(stdin), &readfds);
        tv.tv_sec=0; tv.tv_usec=0;
        select(16, &readfds, 0, 0, &tv);

        return (FD_ISSET(fileno(stdin), &readfds));
    #else
        static int init = 0, pipe;
        static HANDLE inh;
        DWORD dw;

        if (!init)
        {
            init = 1;
            inh = GetStdHandle(STD_INPUT_HANDLE);
            pipe = !GetConsoleMode(inh, &dw);
            if (!pipe)
            {
                SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
                FlushConsoleInputBuffer(inh);
            }
        }
        
        if (pipe)
        {
           if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
           return dw;
        }
        
        else
        {
           GetNumberOfConsoleInputEvents(inh, &dw);
           return dw <= 1 ? 0 : dw;
        }

    #endif
}

void read_input()
{
    
    int bytes;
    char input[256] = "", *endc;
    if (input_waiting())
    {
        stopped = 1;
        do {
            bytes=read(fileno(stdin), input, 256);
        }
        while (bytes < 0);
        endc = strchr(input,'\n');
        if (endc) *endc=0;
        if (strlen(input) > 0)
        {
            if (!strncmp(input, "quit", 4))
            {
                quit = 1;
            }
            else if (!strncmp(input, "stop", 4))    {
                quit = 1;
            }
        }
    }
}

void communicate() {
    // if time is up break here
    if(timeset == 1 && get_time_ms() > stoptime) {
        // tell engine to stop calculating
        stopped = 1;
    }
    
    // read GUI input
    read_input();
}

void reset_time() {
    quit = 0;
    movestogo = 30;
    movetime = -1;
    time_ = -1;
    inc = 0;
    starttime = 0;
    stoptime = 0;
    timeset = 0;
    stopped = 0;
}
