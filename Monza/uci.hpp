//
//  uci.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#ifndef uci_hpp
#define uci_hpp

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "search.hpp"
#include "time.hpp"
#include "types.h"
#include "globals.h"
#include "enums.h"
#include "board.hpp"

void parse_fen(char * fen_string);
void display_UCI_move(int move);
int parse_move(char *move_string);
void parse_position(char *command);
void parse_go(char *command);
void UCI_loop();

#endif /* uci_hpp */
