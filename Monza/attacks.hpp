//
//  attacks.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#ifndef attacks_hpp
#define attacks_hpp

#include <stdio.h>
#include "types.h"
#include "defs.h"

Bitboard generate_rook_attacks(int square, Bitboard blocker_piece);
Bitboard generate_bishop_attacks(int square, Bitboard blocker_piece);
Bitboard mask_bishop_attacks(int square);
Bitboard mask_rook_attacks(int square);
Bitboard mask_king_attacks(int square);
Bitboard mask_knight_attacks(int square);
Bitboard mask_pawn_attacks(int side, int square);
Bitboard get_bishop_attacks(int square, Bitboard occupancy);
Bitboard get_rook_attacks(int square, Bitboard occupancy);
Bitboard get_queen_attacks(int square, Bitboard occupancy);


#endif /* attacks_hpp */
