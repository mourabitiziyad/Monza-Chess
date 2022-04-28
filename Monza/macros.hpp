//
//  macros.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 27/4/2022.
//

#ifndef macros_hpp
#define macros_hpp

#include <stdio.h>

# define mate_score 48000
# define mate_val 49000
# define Infinity 50000

# define clear_t_table() \
    memset(transposition_table, 0, sizeof(transposition_table));

// Bitboard Macros

// # define pop_bit(board, square) (bit_on_square(board, square) ? (board) ^= (1ULL << square) : 0)
# define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
(source)            | \
(target << 6)       | \
(piece << 12)       | \
(promoted << 16)    | \
(capture << 20)     | \
(double << 21)      | \
(enpassant << 22)   | \
(castling << 23)

// extract source square
# define get_move_source(move) (move & 0x3f)

// extract target square
# define get_move_target(move) ((move & 0xfc0) >> 6)

// extract piece
# define get_move_piece(move) ((move & 0xf000) >> 12)

// extract promoted piece
# define get_move_promoted(move) ((move & 0xf0000) >> 16)

// extract capture flag
# define get_move_capture(move) (move & 0x100000)

// extract double pawn push flag
# define get_move_double(move) (move & 0x200000)

// extract enpassant flag
# define get_move_enpassant(move) (move & 0x400000)

// extract castling flag
# define get_move_castling(move) (move & 0x800000)

# define bit_on_square(board, square) ((board) & (1ULL << square))
# define set_bit(board, square) ((board) |= (1ULL << square))
# define pop_bit(board, square) ((board) &= ~(1ULL << (square)))
# define count_bits(bitboard) __builtin_popcountll(bitboard)
# define ls1b(bitboard) (__builtin_ctzll(bitboard))
// Returns the number of trailing 0-bits in x, starting at the least significant bit position. If x is 0, the result is undefined. https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html


// Brian Kernighan’s Algorithm - https://www.geeksforgeeks.org/count-set-bits-in-an-integer/
//
// static inline int count_bits(Bitboard bitboard) {
//    int count = 0;
//    while (bitboard) {
//        count++;
//        bitboard &= bitboard - 1;
//    }
//    return count;
// }

# define empty_board "8/8/8/8/8/8/8/8 b - - "
# define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
# define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
# define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
# define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

# define copy_board()                                                                 \
    Bitboard piece_bitboards_copy[12], piece_occupancy_copy[3];                 \
    int turn_copy, en_passant_copy, castling_rights_copy;                       \
    memcpy(piece_bitboards_copy, piece_bitboards, sizeof(piece_bitboards));     \
    memcpy(piece_occupancy_copy, piece_occupancy, sizeof(piece_occupancy));     \
    turn_copy = turn;                                                           \
    en_passant_copy = en_passant;                                               \
    castling_rights_copy = castling_rights;                                     \
    Bitboard hash_copy = hash_key;

# define take_back()                                                            \
    memcpy(piece_bitboards, piece_bitboards_copy, sizeof(piece_bitboards));     \
    memcpy(piece_occupancy, piece_occupancy_copy, sizeof(piece_occupancy));     \
    turn = turn_copy;                                                           \
    en_passant = en_passant_copy;                                               \
    castling_rights = castling_rights_copy;                                     \
    hash_key = hash_copy;

# define t_size 5600000  // times the size of a tt struct equals about 112 mb
//# define t_size 800000 // about 16 mb
//int t_size = 0;

# define hashfEXACT 0
# define hashfALPHA 1
# define hashfBETA 2
# define no_hash_entry 100000

# define max_ply 64

#endif /* macros_hpp */
