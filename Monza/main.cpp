//
//  main.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 8/1/2022.
//

#include "constants.hpp"

#include <stdlib.h>
#include <iostream>
using namespace std;

//      +---+---+---+---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ Printing the Board ---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+

// https://www.chessprogramming.org/Looking_for_Magics
// https://www.chessprogramming.org/Magic_Bitboards#How_it_works

Bitboard get_occupancy_set(int idx, int bits, Bitboard attacks) {
    Bitboard occupancy = 0ULL;
    for (int x = 0; x < bits; x++) {
        int square = ls1b(attacks);
        pop_bit(attacks, square);
        if (idx & (1 << x)) occupancy |= (1ULL << square); // What are we checking for here?
    }
    return occupancy;
}

Bitboard piece_bitboards[12];
Bitboard piece_occupancy[3]; // white, black, both

int turn = 0;
int castling_rights = 0;
int en_passant = -1;

char ascii_pieces[] = "PNBRQKpnbrqk";

//                        8  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
//                        7  ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎
//                        6  . . . . . . . .
//                        5  . . . . . . . .
//                        4  . . . . . . . .
//                        3  . . . . . . . .
//                        2  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
//                        1  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖
//                           a b c d e f g h

char * unicode_pieces[12] = {(char *)"♟︎", (char *)"♞", (char *)"♝", (char *)"♜", (char *)"♛", (char *)"♚", (char *)"♙", (char *)"♘", (char *)"♗", (char *)"♖", (char *)"♕", (char *)"♔"};
//char unicode_pieces[] = "♟︎♞♝♜♛♚♙♘♗♖♕♔";

void board(Bitboard bitboard) {
    for (int rank=0; rank<8; rank++) {
//        cout << "   +---+---+---+---+---+---+---+---+\n";
        for (int file=0; file<8; file++) {
            
            !file && cout << (8 - rank) << "  ";
            
            cout << " " << (bit_on_square(bitboard, (rank * 8 + file)) ? '1' : '.');
        }
        cout << "\n";
    }
//    cout << "   +---+---+---+---+---+---+---+---+\n\n  ";
    cout << "\n   ";
    for (char i='A'; i<='H'; i++)
        cout << " " << i;
    cout << "\n\n    Bitboard: " << bitboard << "\n\n";
    
}

void styled_board() {
    cout << "\n";
    for (int rank=0; rank<8; rank++) {
//        cout << "   +---+---+---+---+---+---+---+---+\n";
        for (int file=0; file<8; file++) {
            
            !file && cout << (8 - rank) << "  ";
            
            int styled_piece = -999;
            
            for (int piece=P; piece <= k; piece++)
                bit_on_square(piece_bitboards[piece], (rank * 8 + file)) && (styled_piece = piece);
            
//            cout << " " << (bit_on_square(bitboard, (rank * 8 + file)) ? '1' : '.');
            printf(" %s", (styled_piece == -999) ? "." : unicode_pieces[styled_piece]);
        }
        cout << "\n";
    }
//    cout << "   +---+---+---+---+---+---+---+---+\n\n  ";
    cout << "\n   ";
    for (char i='A'; i<='H'; i++)
        cout << " " << i;
    printf("\n\nTurn:              %s\n", !turn? "White" : "Black");
    printf("En Passant:        %s\n", (en_passant != -1) ? squares[en_passant] : "No");
    printf("Castling Rights:   %c%c%c%c\n\n", castling_rights & wk ? 'K': '-',
                                     castling_rights & wq ? 'Q': '-',
                                     castling_rights & bk ? 'k': '-',
                                     castling_rights & bq ? 'q': '-');
}

//      https://www.chessprogramming.org/Looking_for_Magics
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ Pre-Calculated Attacks ---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+

Bitboard pawn_attack_moves[2][64]; // [Side] [Square]
Bitboard knight_attack_moves[64];
Bitboard king_attack_moves[64];

Bitboard bishop_attack_masks[64];
Bitboard bishop_attack_moves[64][512];

Bitboard rook_attack_masks[64];
Bitboard rook_attack_moves[64][4096];


const int bishop_relevant_occupancy_bits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

// Count bits of masked sliding piece attacks for every square.
// Consider bishop_relevant_occupancy_bits[0] which is A8.
// There are 6 bits (we exclude the edge) in its attack squares.

const int rook_relevant_occupancy_bits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

Bitboard generate_rook_attacks(int square, Bitboard blocker_piece) {
    
    Bitboard attacks = 0ULL;
    
    int file, rank;
    int t_rank = square / 8;
    int t_file = square % 8;
    
//    cout << "rank: " << t_rank << " file: " << t_file << " " << "\n";
    
    for (rank = t_rank + 1; rank < 8; rank++) {
        Bitboard square = (1ULL << (rank * 8 + t_file));
        attacks |= square;
        if (square & blocker_piece) break;
    }
        
    for (rank = t_rank - 1; rank >= 0; rank--) {
        Bitboard square = (1ULL << (rank * 8 + t_file));
        attacks |= square;
        if (square & blocker_piece) break;
    }
        
    for (file = t_file - 1; file >= 0; file--) {
        Bitboard square = (1ULL << (t_rank * 8 + file));
        attacks |= square;
        if (square & blocker_piece) break;
    }
        
    for (file = t_file + 1; file < 8; file++) {
        Bitboard square = (1ULL << (t_rank * 8 + file));
        attacks |= square;
        if (square & blocker_piece) break;
    }
    
    return attacks;
}

Bitboard generate_bishop_attacks(int square, Bitboard blocker_piece) {
    
    Bitboard attacks = 0ULL;
    
    int file, rank;
    int t_rank = square / 8;
    int t_file = square % 8;
    
    for (rank = t_rank + 1, file = t_file + 1; rank < 8 && file < 8; rank++, file++) {
        Bitboard square = 1ULL << (rank * 8 + file);
        attacks |= square;
        if (square & blocker_piece) break;
    }
    for (rank = t_rank - 1, file = t_file + 1; rank >= 0 && file < 8; rank--, file++) {
        Bitboard square = 1ULL << (rank * 8 + file);
        attacks |= square;
        if (square & blocker_piece) break;
    }
    for (rank = t_rank + 1, file = t_file - 1; rank < 8 && file >= 0; rank++, file--) {
        Bitboard square = 1ULL << (rank * 8 + file);
        attacks |= square;
        if (square & blocker_piece) break;
    }
        
    for (rank = t_rank - 1, file = t_file - 1; rank >= 0 && file >= 0; rank--, file--) {
        Bitboard square = 1ULL << (rank * 8 + file);
        attacks |= square;
        if (square & blocker_piece) break;
    }
    
    return attacks;
}


Bitboard mask_bishop_attacks(int square) {
    
    Bitboard occu_squares = 0ULL;
    
    int file, rank;
    int t_rank = square / 8;
    int t_file = square % 8;
    
    for (rank = t_rank + 1, file = t_file + 1; rank < 7 && file < 7; rank++, file++)
        occu_squares |= (1ULL << (rank * 8 + file));
    for (rank = t_rank - 1, file = t_file + 1; rank >= 1 && file < 7; rank--, file++)
        occu_squares |= (1ULL << (rank * 8 + file));
    for (rank = t_rank + 1, file = t_file - 1; rank < 7 && file >= 1; rank++, file--)
        occu_squares |= (1ULL << (rank * 8 + file));
    for (rank = t_rank - 1, file = t_file - 1; rank >= 1 && file >= 1; rank--, file--)
        occu_squares |= (1ULL << (rank * 8 + file));
    
    return occu_squares; // relevant bishop occupancy squares
}

Bitboard mask_rook_attacks(int square) {
    
    Bitboard occu_squares = 0ULL;
    
    int file, rank;
    int t_rank = square / 8;
    int t_file = square % 8;
    
//    cout << "rank: " << t_rank << " file: " << t_file << " " << "\n";
    
    for (rank = t_rank + 1; rank < 7; rank++)
        occu_squares |= (1ULL << (rank * 8 + t_file));
    for (rank = t_rank - 1; rank >= 1; rank--)
        occu_squares |= (1ULL << (rank * 8 + t_file));
    for (file = t_file - 1; file >= 1; file--)
        occu_squares |= (1ULL << (t_rank * 8 + file));
    for (file = t_file + 1; file < 7; file++)
        occu_squares |= (1ULL << (t_rank * 8 + file));
    
    return occu_squares; // relevant rook occupancy squares
}

Bitboard mask_king_attacks(int square) {
    Bitboard attacks = 0ULL;
    Bitboard bitboard = 0ULL;
    
    set_bit(bitboard, square);
    
    if (bitboard >> 8) attacks |= (bitboard >> 8); // Condition may not be needed
    if ((bitboard >> 9) & notHFile) attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & notAFile) attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & notHFile) attacks |= (bitboard >> 1);
    
    if (bitboard << 8) attacks |= (bitboard << 8); // Condition may not be needed
    if ((bitboard << 9) & notAFile) attacks |= (bitboard << 9);
    if ((bitboard << 7) & notHFile) attacks |= (bitboard << 7);
    if ((bitboard << 1) & notAFile) attacks |= (bitboard << 1);
    
    return attacks;
}

Bitboard mask_knight_attacks(int square) {
    
    Bitboard attacks = 0ULL;
    Bitboard bitboard = 0ULL;
    
    set_bit(bitboard, square);
    
    if ((bitboard >> 17) & notHFile) attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & notAFile) attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & notHGFile) attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & notABFile) attacks |= (bitboard >> 6);
    
    if ((bitboard << 17) & notAFile) attacks |= (bitboard << 17);
    if ((bitboard << 15) & notHFile) attacks |= (bitboard << 15);
    if ((bitboard << 10) & notABFile) attacks |= (bitboard << 10);
    if ((bitboard << 6) & notHGFile) attacks |= (bitboard << 6);
    
    return attacks;
}

Bitboard mask_pawn_attacks(int side, int square) {
    
    Bitboard attacks = 0ULL;
    Bitboard bitboard = 0ULL;
    
    set_bit(bitboard, square);
    
    // if side is black
    if (side) {
        if ((bitboard << 7) & notHFile) attacks |= (bitboard << 7);
        if ((bitboard << 9) & notAFile) attacks |= (bitboard << 9);
    }
    // side is white
    else {
        if ((bitboard >> 7) & notAFile) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & notHFile) attacks |= (bitboard >> 9);
    }
    
    return attacks;
}

void init_non_sliding_pieces(){
    for (int square = 0; square < 64; square++) {
        
        pawn_attack_moves[white][square] = mask_pawn_attacks(white, square);
        pawn_attack_moves[black][square] = mask_pawn_attacks(black, square);
        
        knight_attack_moves[square] = mask_knight_attacks(square);
        
        king_attack_moves[square] = mask_king_attacks(square);
    }
}

// https://www.chessprogramming.org/Looking_for_Magics#Feeding_in_Randoms
Bitboard find_magic(int square, int bits, int bishop) {
    
    Bitboard occupancies[4096];
    Bitboard attacks[4096];
    Bitboard used_attacks[4096];
    
    Bitboard mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);
    Bitboard occupancy_idx = 1 << bits;
//    cout << occupancy_idx << '\n';
    for (int idx = 0; idx < occupancy_idx; idx++){
        occupancies[idx] = get_occupancy_set(idx, bits, mask);
        attacks[idx] = bishop ? generate_bishop_attacks(square, occupancies[idx])
        : generate_rook_attacks(square, occupancies[idx]);
    }
    
    for (int i = 0; i < 100000000; i++) {
        
        Bitboard magic_number = random_U64_fewbits();
        
        if (count_bits((mask * magic_number) & 0xFF00000000000000) < 6) continue;
        
        memset(used_attacks, 0ULL, sizeof(used_attacks));
    
        int index, fail;
        
        for(index = 0, fail = 0; !fail && index < occupancy_idx; index++) {
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - bits));
            if (used_attacks[magic_index] == 0ULL) used_attacks[magic_index] = attacks[index];
            else if (used_attacks[magic_index] != attacks[index]) fail = 1;
            
        }
        if (!fail) return magic_number;
    }
    cout << "Failed!\n";
    return 0ULL;
}

void init_magic() {
    
    for (int square = 0; square < 64; square++) {
        printf("0x%llxULL,\n", find_magic(square, bishop_relevant_occupancy_bits[square], bishop));
    }
    
    cout << "\n\n";
    
    for (int square = 0; square < 64; square++) {
        printf("0x%llxULL,\n", find_magic(square, rook_relevant_occupancy_bits[square], rook));
    }
    cout << "done!\n";
}

void init_sliding_pieces(int bishop) {
    
    for (int square = 0; square < 64; square++) {
        
        bishop_attack_masks[square] = mask_bishop_attacks(square);
        rook_attack_masks[square] = mask_rook_attacks(square);
        
        Bitboard attack = bishop ? bishop_attack_masks[square] : rook_attack_masks[square];
        
        int bit_count = count_bits(attack);
        
        int occupancy_idx = 1 << bit_count;
        
        for (int index = 0; index < occupancy_idx; index++) {
            if (bishop) {
                Bitboard occupancy = get_occupancy_set(index, bit_count, attack);
                int magic_idx = (int)((occupancy * bishop_magics[square]) >> (64 - bishop_relevant_occupancy_bits[square]));
                bishop_attack_moves[square][magic_idx] = generate_bishop_attacks(square, occupancy);
            }
            else {
                Bitboard occupancy = get_occupancy_set(index, bit_count, attack);
                int magic_idx = (int)((occupancy * rook_magics[square]) >> (64 - rook_relevant_occupancy_bits[square]));
                rook_attack_moves[square][magic_idx] = generate_rook_attacks(square, occupancy);
            }
        }
    }
}

// https://www.chessprogramming.org/Magic_Bitboards#Plain

static inline Bitboard get_bishop_attacks(int square, Bitboard occupancy) {
    
    occupancy &= bishop_attack_masks[square];
    occupancy *= bishop_magics[square];
    occupancy >>= 64 - bishop_relevant_occupancy_bits[square];
    return bishop_attack_moves[square][occupancy];
    
}

static inline Bitboard get_rook_attacks(int square, Bitboard occupancy) {
    
    occupancy &= rook_attack_masks[square];
    occupancy *= rook_magics[square];
    occupancy >>= 64 - rook_relevant_occupancy_bits[square];
    return rook_attack_moves[square][occupancy];
    
}

void init() {
    
    init_non_sliding_pieces();
    init_sliding_pieces(bishop);
    init_sliding_pieces(rook);
//    init_magic();
    
}

//      +---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ main  +---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+

int main(int argc, const char * argv[]) {
    
    init();
    
    set_bit(piece_bitboards[P], a2);
    set_bit(piece_bitboards[P], b2);
    set_bit(piece_bitboards[P], c2);
    set_bit(piece_bitboards[P], d2);
    set_bit(piece_bitboards[P], e2);
    set_bit(piece_bitboards[P], f2);
    set_bit(piece_bitboards[P], g2);
    set_bit(piece_bitboards[P], h2);
    
    set_bit(piece_bitboards[N], b1);
    set_bit(piece_bitboards[N], g1);
    
    set_bit(piece_bitboards[B], c1);
    set_bit(piece_bitboards[B], f1);
    
    set_bit(piece_bitboards[R], a1);
    set_bit(piece_bitboards[R], h1);
    
    set_bit(piece_bitboards[Q], d1);
    set_bit(piece_bitboards[K], e1);
    
    
    set_bit(piece_bitboards[p], a7);
    set_bit(piece_bitboards[p], b7);
    set_bit(piece_bitboards[p], c7);
    set_bit(piece_bitboards[p], d7);
    set_bit(piece_bitboards[p], e7);
    set_bit(piece_bitboards[p], f7);
    set_bit(piece_bitboards[p], g7);
    set_bit(piece_bitboards[p], h7);
    
    set_bit(piece_bitboards[n], b8);
    set_bit(piece_bitboards[n], g8);
    
    set_bit(piece_bitboards[b], c8);
    set_bit(piece_bitboards[b], f8);
    
    set_bit(piece_bitboards[r], a8);
    set_bit(piece_bitboards[r], h8);
    
    set_bit(piece_bitboards[q], d8);
    set_bit(piece_bitboards[k], e8);
    
    turn = white;
    
    styled_board();
    return 0;
}
