//
//  move.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#ifndef move_hpp
#define move_hpp

#include <stdio.h>
#include "macros.hpp"
#include "types.h"
#include <string.h>
#include "globals.h"
#include "attacks.hpp"
#include "board.hpp"

void add_move(moves *move_list, int move);
int make_move (int move, int move_flag);
int is_square_attacked(int square, int side);
void generate_all_moves(moves *move_list);
void print_attacked_squares(int side);
void display_move_list(moves *move_list);

#endif /* move_hpp */
