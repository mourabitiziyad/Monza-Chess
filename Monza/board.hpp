//
//  board.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 27/4/2022.
//

#ifndef board_hpp
#define board_hpp

#include <stdio.h>
#include "types.h"

void board(Bitboard bitboard);
void styled_board();
int find_piece_index(char piece);
char find_promoted_piece_index(int piece);
#endif /* board_hpp */
