//
//  time.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 27/4/2022.
//

#ifndef time_hpp
#define time_hpp


#include <stdio.h>

int quit = 0;
int movestogo = 30;
int movetime = -1;
int time_ = -1;
int inc = 0;
int starttime = 0;
int stoptime = 0;
int timeset = 0;
int stopped = 0;

int get_time_ms();
int input_waiting();
void read_input();
void communicate();
void reset_time();

#endif /* time_hpp */
