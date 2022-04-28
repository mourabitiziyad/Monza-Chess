//
//  main.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 8/1/2022.
//

#include "constants.hpp"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
using namespace std;

#include "time.hpp"
#include "board.hpp"
#include "attacks.hpp"
#include "init.hpp"
#include "uci.hpp"

//      +---+---+---+---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ Printing the Board ---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+

// https://www.chessprogramming.org/Looking_for_Magics
// https://www.chessprogramming.org/Magic_Bitboards#How_it_works

//                        8  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
//                        7  ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎
//                        6  . . . . . . . .
//                        5  . . . . . . . .
//                        4  . . . . . . . .
//                        3  . . . . . . . .
//                        2  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
//                        1  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖
//                           a b c d e f g h

//char unicode_pieces[] = "♟︎♞♝♜♛♚♙♘♗♖♕♔";

//      https://www.chessprogramming.org/Looking_for_Magics
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ Pre-Calculated Attacks ---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+


//static inline void perft_driver(int depth) {
//    if (depth == 0) {
//        nodes++;
//        return;
//    }
//
//    moves move_list[1];
//    generate_all_moves(move_list);
//
//    for (int moves = 0; moves < move_list->count; moves++) {
//        copy_board();
//        if (!make_move(move_list->moves[moves], all)) continue;
//        perft_driver(depth - 1);
//        take_back();
////        display_UCI_move(move_list->moves[moves]);
//    }
//}
//
//void perft_test(int depth) {
//
//    moves move_list[1];
//    generate_all_moves(move_list);
//
//    auto startTime = chrono::high_resolution_clock::now();
//
//    for (int moves = 0; moves < move_list->count; moves++) {
//        copy_board();
//        if (!make_move(move_list->moves[moves], all)) continue;
//        long cummulative_nodes = nodes;
//        perft_driver(depth - 1);
//        long old_nodes = nodes - cummulative_nodes;
//        take_back();
//        printf("     move: %s%s%c  nodes: %ld\n", squares[get_move_source(move_list->moves[moves])],
//                                                        squares[get_move_target(move_list->moves[moves])],
//                                                        get_move_promoted(move_list->moves[moves])
//               ?
////               promoted_pieces[get_move_promoted(move_list->moves[moves])]
//               find_promoted_piece_index(get_move_promoted(move_list->moves[moves]))
//               : ' ',
//                                                        old_nodes);
//    }
//    auto endTime = chrono::high_resolution_clock::now();
//    printf("\n     Depth: %d\n", depth);
//    printf("     Nodes: %ld\n", nodes);
//    cout << "     Time: " << chrono::duration_cast < chrono::milliseconds > (endTime - startTime).count() << "ms" << endl;
//}
/*
    Example UCI commands to init position on chess board
    
    // init start position
    position startpos
    
    // init start position and make the moves on chess board
    position startpos moves e2e4 e7e5
    
    // init position from FEN string
    position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    
    // init position from fen string and make moves on chess board
    position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves e2a6 e8g8
*/

// parse UCI "position" command

int main(int argc, const char * argv[]) {
    
    init();
    
//    parse_fen(starting_position);
//    board(piece_occupancy[both]);
//    styled_board();
//    perft_test(6);
//    search_position(10);

    
    UCI_loop();
    
//    free(transposition_table);
    
    return 0;
}


//    8  . . . . . . 1 .                  . . . . . . . .                   . . . . . . . .
//    7  . . . . . 1 1 1                  . . . . . . . .                   . . . 1 . . . .
//    6  . 1 1 1 . . . .                  . . . 1 . . . .     Identical     . . . 1 . . . .
//    5  . . . . . . . .    Is masked     . . . . . . . .     to another    . . . . . . . .
//    4  . . . 1 . 1 . .     down to      . . . 1 . 1 . .     position      . . . 1 . 1 . 1
//    3  . . . . . . . .                  . . . . . . . .     where rook    . . . . . . . .
//    2  1 1 . . . . 1 1                  . . . . . . . .     is on D4      . . . . . . . .
//    1  . . . . . . 1 .                  . . . . . . . .                   . . . . . . . .
//
//       A B C D E F G H
