//
//  types.h
//  Monza
//
//  Created by Ziyad Mourabiti on 27/4/2022.
//

#ifndef types_h
#define types_h

#include "macros.hpp"

int pv_length[max_ply];
int pv_table[max_ply][max_ply];

int is_following_pv, score_pv;

int killer[2][max_ply];
int history_moves[12][64];

typedef struct moves {
    int moves[256];
    int count = 0;
} moves;

typedef unsigned long long Bitboard;

Bitboard piece_bitboards[12];
Bitboard piece_occupancy[3]; // white, black, both

Bitboard piece_keys[12][64];
Bitboard enpassant_keys[64];
Bitboard castling_keys[16];
Bitboard turn_key;
Bitboard hash_key;
Bitboard repetitions[500];

Bitboard file_masks[64];
Bitboard rank_masks[64];
Bitboard isolated_pawn_masks[64];
Bitboard white_passed_pawn_masks[64];
Bitboard black_passed_pawn_masks[64];

Bitboard pawn_attack_moves[2][64]; // [Side] [Square]
Bitboard knight_attack_moves[64];
Bitboard king_attack_moves[64];

Bitboard bishop_attack_masks[64];
Bitboard bishop_attack_moves[64][512];

Bitboard rook_attack_masks[64];
Bitboard rook_attack_moves[64][4096];

typedef struct tagHASHE {
    Bitboard key;
    int depth;
    int flag;
    int value;
} t_table;

t_table transposition_table[t_size];
//t_table *transposition_table = NULL;

long nodes;

#endif /* types_h */

