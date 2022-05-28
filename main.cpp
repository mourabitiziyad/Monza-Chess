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

#if defined(WIN64) || defined(WIN32)
    char * unicode_pieces[12] = {(char *)"P",(char *)"N",(char *)"B",(char *)"R",(char *)"Q",(char *)"K",(char *)"p",(char *)"n",(char *)"b",(char *)"r",(char *)"q",(char *)"k"};
#else
    char * unicode_pieces[12] = {(char *)"♟︎", (char *)"♞", (char *)"♝", (char *)"♜", (char *)"♛", (char *)"♚", (char *)"♙", (char *)"♘", (char *)"♗", (char *)"♖", (char *)"♕", (char *)"♔"};
#endif

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
    printf("Castling Rights:   %c%c%c%c\n", castling_rights & wk ? 'K': '-',
                                     castling_rights & wq ? 'Q': '-',
                                     castling_rights & bk ? 'k': '-',
                                     castling_rights & bq ? 'q': '-');
    printf("Hash Key:          %llx\n\n", hash_key);
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

static inline Bitboard get_queen_attacks(int square, Bitboard occupancy) {
    return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);
}

void parse_fen(char * fen_string) {
//    cout << fen_string << '\n';
    memset(piece_bitboards, 0ULL, sizeof(piece_bitboards));
    memset(piece_occupancy, 0ULL, sizeof(piece_occupancy));
    
    castling_rights = 0;
    turn = -1;
    en_passant = -1;
    
    repetition_index = 0;
    memset(repetitions, 0, sizeof(repetitions));
    
    for (int square = 0; square < 64 && *fen_string && *fen_string!=' ';) {
        if (isalpha(*fen_string)) {
            int piece_index = find_piece_index(*fen_string++);
            set_bit(piece_bitboards[piece_index], square++);
        }
        else if (isdigit(*fen_string)) square += (*fen_string++ - '0');
        else if (*fen_string == '/') fen_string++;
        else memset(piece_bitboards, 0ULL, sizeof(piece_bitboards));
    }
    *(++fen_string) == 'b' ? (turn = black) : (turn = white);
    (fen_string += 2);
    while (*fen_string != ' ') {
        switch (*fen_string++) {
            case 'K': castling_rights |= wk; break;
            case 'Q': castling_rights |= wq; break;
            case 'k': castling_rights |= bk; break;
            case 'q': castling_rights |= bq; break;
            case '-': break;
            default: break;
        }
    }
    if (*(++fen_string) != '-') {
        int file = *fen_string++ - 'a';
        int rank = 8 - (*fen_string - '0');
        en_passant = rank * 8 + file;
    } else {
        en_passant = -1;
    }
    
    for (int piece = P; piece <= K; piece++) {
        piece_occupancy[white] |= piece_bitboards[piece];
    }
    
    for (int piece = p; piece <= k; piece++) {
        piece_occupancy[black] |= piece_bitboards[piece];
    }
    
    piece_occupancy[both] |= (piece_occupancy[white] | piece_occupancy[black]);
    
    hash_key = generate_hash();
}

static inline int is_square_attacked(int square, int side) {
    if (side == white) {
        return ((pawn_attack_moves[black][square] & piece_bitboards[P])
        | (knight_attack_moves[square] & piece_bitboards[N])
        | (king_attack_moves[square] & piece_bitboards[K])
        | (get_bishop_attacks(square, piece_occupancy[both]) & piece_bitboards[B])
        | (get_rook_attacks(square, piece_occupancy[both]) & piece_bitboards[R])
        | (get_queen_attacks(square, piece_occupancy[both]) & piece_bitboards[Q])) ? 1 : 0;
    } else if (side == black){
        return ((pawn_attack_moves[white][square] & piece_bitboards[p])
        | (knight_attack_moves[square] & piece_bitboards[n])
        | (king_attack_moves[square] & piece_bitboards[k])
        | (get_bishop_attacks(square, piece_occupancy[both]) & piece_bitboards[b])
        | (get_rook_attacks(square, piece_occupancy[both]) & piece_bitboards[r])
        | (get_queen_attacks(square, piece_occupancy[both]) & piece_bitboards[q])) ? 1 : 0;
    }
    return 0;
}


static inline void generate_all_moves(moves *move_list) {
    int source, target;
    Bitboard bitboard_copy, attacks;
    for (int piece = P; piece <= k; piece++) {
        bitboard_copy = piece_bitboards[piece];
        if (turn == white) { // white (defining the side) !turn
            // TODO white pawns
            if (piece == P) {
                while (bitboard_copy) {
                    source = ls1b(bitboard_copy);
                    target = source - 8;
                    if (!(target < a8) && !bit_on_square(piece_occupancy[both], target)) {
                        if (source >= a7 && source <= h7) {
                            add_move(move_list, encode_move(source, target, piece, Q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, R, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, B, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, N, 0, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                            if ((source >= a2 && source <= h2)
                                && !bit_on_square(piece_occupancy[both], (target - 8))) {
                                add_move(move_list, encode_move(source, (target - 8), piece, 0, 0, 1, 0, 0));
                            }
                        }
                        // pawn push
                        // double pawn push
                    }
                    
                    attacks = pawn_attack_moves[turn][source] & piece_occupancy[black];
                    
                    while (attacks) {
                        target = ls1b(attacks);
                        
                        if (source >= a7 && source <= h7) {
                            add_move(move_list, encode_move(source, target, piece, Q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, R, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, B, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, N, 1, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                        }
                        pop_bit(attacks, target);
                    }
                    
                    if (en_passant != -1) {
                        Bitboard en_passant_capture = pawn_attack_moves[turn][source] & (1ULL << en_passant);
                        if (en_passant_capture) {
                            int target_en_passant = ls1b(en_passant_capture);
                            add_move(move_list, encode_move(source, target_en_passant, piece, 0, 1, 0, 1, 0));
                        }
                    }
                    
                    pop_bit(bitboard_copy, source);
                }
            }
            if (piece == K) {
                
                if (castling_rights & wk) {
                    if (!bit_on_square(piece_occupancy[both], f1) && !bit_on_square(piece_occupancy[both], g1)) {
                        if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black)) {
                            add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
                
                if (castling_rights & wq) {
                    if (!bit_on_square(piece_occupancy[both], d1) && !bit_on_square(piece_occupancy[both], c1) && !bit_on_square(piece_occupancy[both], b1)) {
                        if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black)) {
                            add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }
        } else if (turn == black) {
            // TODO black pawns
            if (piece == p) {
                while (bitboard_copy) {
                    source = ls1b(bitboard_copy);
                    target = source + 8;
                    if (!(target > h1) && !bit_on_square(piece_occupancy[both], target)) {
                        if (source >= a2 && source <= h2) {
                            add_move(move_list, encode_move(source, target, piece, q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, r, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, b, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, n, 0, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                            if ((source >= a7 && source <= h7)
                                && !bit_on_square(piece_occupancy[both], (target + 8))) {
                                add_move(move_list, encode_move(source, (target + 8), piece, 0, 0, 1, 0, 0));
                            }

                        }
                        // pawn push
                        // double pawn push
                    }
                    
                    attacks = pawn_attack_moves[turn][source] & piece_occupancy[white];
                    
                    while (attacks) {
                        target = ls1b(attacks);
                        
                        if (source >= a2 && source <= h2) {
                            add_move(move_list, encode_move(source, target, piece, q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, r, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, b, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, n, 1, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                        }
                        pop_bit(attacks, target);
                    }
                    
                    if (en_passant != -1) {
                        Bitboard en_passant_capture = pawn_attack_moves[turn][source] & (1ULL << en_passant);
                        if (en_passant_capture) {
                            int target_en_passant = ls1b(en_passant_capture);
                            add_move(move_list, encode_move(source, target_en_passant, piece, 0, 1, 0, 1, 0));
                        }
                    }
                    
                    pop_bit(bitboard_copy, source);
                }
            }
            
            if (piece == k) {
                
                if (castling_rights & bk) {
                    if (!bit_on_square(piece_occupancy[both], f8) && !bit_on_square(piece_occupancy[both], g8)) {
                        if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white))
                            add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                    }
                }
                
                if (castling_rights & bq) {
                    if (!bit_on_square(piece_occupancy[both], d8) && !bit_on_square(piece_occupancy[both], c8) && !bit_on_square(piece_occupancy[both], b8)) {
                        if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white))
                            add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                    }
                }
            }
        }
        
        // knight
        if ((turn == white) ? piece == N : piece == n) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = knight_attack_moves[source] & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));

                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // bishop
        if ((turn == white) ? piece == B : piece == b) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = get_bishop_attacks(source, piece_occupancy[both]) & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));

                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // rooks
        if ((turn == white) ? piece == R : piece == r) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = get_rook_attacks(source, piece_occupancy[both]) & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // queens
        if ((turn == white) ? piece == Q : piece == q) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = get_queen_attacks(source, piece_occupancy[both]) & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // kings
        if ((turn == white) ? piece == K : piece == k) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = king_attack_moves[source] & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
    }
}

void print_attacked_squares(int side)
{
    printf("\n");
    
    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // print ranks
            if (!file)
                printf("  %d ", 8 - rank);
            
            // check whether current square is attacked or not
            printf(" %d", is_square_attacked(square, side) ? 1 : 0);
        }
        
        // print new line every rank
        printf("\n");
    }
    
    // print files
    printf("\n     a b c d e f g h\n\n");
}

void display_UCI_move(int move) {
    if (get_move_promoted(move)) {
        printf("%s%s%c",  squares[get_move_source(move)],
                            squares[get_move_target(move)],
//                            promoted_pieces[get_move_promoted(move)]);
               find_promoted_piece_index(get_move_promoted(move)));
    } else {
        printf("%s%s",  squares[get_move_source(move)],
                            squares[get_move_target(move)]);
    }
}

void display_move_list(moves *move_list) {
    printf("\nMove       Piece       isCapture?      isDoublePush?       Enpassant?        Castling?\n\n");
    for (int count = 0; count < move_list->count; count++) {
        printf("%s%s%c      %s           %d               %d                   %d                 %d\n",
                            squares[get_move_source(move_list->moves[count])],
                            squares[get_move_target(move_list->moves[count])],
                            get_move_promoted(move_list->moves[count]) ?
//               promoted_pieces[get_move_promoted(move_list->moves[count])]
               find_promoted_piece_index(get_move_promoted(move_list->moves[count]))
               : ' ',
                            unicode_pieces[get_move_piece(move_list->moves[count])],
                            get_move_capture(move_list->moves[count]) ? 1 : 0,
                            get_move_double(move_list->moves[count]) ? 1 : 0,
                            get_move_enpassant(move_list->moves[count]) ? 1 : 0,
                            get_move_castling(move_list->moves[count]) ? 1 : 0);

    }
    printf("\nNumber of Moves: %d\n\n", move_list->count);
}

# define copy()                                                                 \
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

enum {
    all,
    captures
};

int make_move (int move, int move_flag) {
    if (move_flag == all) {
        // preserve the current *legal* board state in case of making an illegal move
        // we are also providing the option to generate only captures to mitigate the "Horizon Effect"
        // "The horizon effect can be mitigated by extending the search algorithm with a quiescence search. This gives the search algorithm ability to look beyond its horizon for a certain class of moves of major importance to the game state, such as captures in chess."
        copy();
        
        int source = get_move_source(move);
        int target = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int doublepush = get_move_double(move);
        int enpassant = get_move_enpassant(move);
        int castling = get_move_castling(move);
        
        pop_bit(piece_bitboards[piece], source);
        set_bit(piece_bitboards[piece], target);
        
        // hashing
        hash_key ^= piece_keys[piece][source];
        hash_key ^= piece_keys[piece][target];
        
        if (capture) {

            int start = (turn == white) ? p : P;
            int end = (turn == white) ? k : K;
            
            for (int bitboard_piece = start; bitboard_piece <= end; bitboard_piece++) {
                if (bit_on_square(piece_bitboards[bitboard_piece], target)) {
                    pop_bit(piece_bitboards[bitboard_piece], target);
                    hash_key ^= piece_keys[bitboard_piece][target];
                    break;
                }
            }
        }
        
        if (promoted) {
            if (turn == white) {
                pop_bit(piece_bitboards[P], target);
                hash_key ^= piece_keys[P][target];
            } else {
                pop_bit(piece_bitboards[p], target);
                hash_key ^= piece_keys[p][target];
            }
            set_bit(piece_bitboards[promoted], target);
            hash_key ^= piece_keys[promoted][target];
        }
        
        if (enpassant) {
            if (turn == white) {
                pop_bit(piece_bitboards[p], target + 8);
                hash_key ^= piece_keys[p][target + 8];
            } else {
                pop_bit(piece_bitboards[P], target - 8);
                hash_key ^= piece_keys[P][target - 8];
            }
        }
        
        if (en_passant != -1) hash_key ^= enpassant_keys[en_passant];
        
        en_passant = -1;
        
        if (doublepush) {
            if (turn == white) {
                en_passant = target + 8;
                hash_key ^= enpassant_keys[target + 8];
            } else {
                en_passant = target - 8;
                hash_key ^= enpassant_keys[target - 8];
            }
        }
        
        if (castling) {
            switch (target) {
                case g1:
                    pop_bit(piece_bitboards[R], h1);
                    set_bit(piece_bitboards[R], f1);
                    hash_key ^= piece_keys[R][h1];
                    hash_key ^= piece_keys[R][f1];
                    break;
                case c1:
                    pop_bit(piece_bitboards[R], a1);
                    set_bit(piece_bitboards[R], d1);
                    hash_key ^= piece_keys[R][a1];
                    hash_key ^= piece_keys[R][d1];
                    break;
                case g8:
                    pop_bit(piece_bitboards[r], h8);
                    set_bit(piece_bitboards[r], f8);
                    hash_key ^= piece_keys[r][h8];
                    hash_key ^= piece_keys[r][f8];
                    break;
                case c8:
                    pop_bit(piece_bitboards[r], a8);
                    set_bit(piece_bitboards[r], d8);
                    hash_key ^= piece_keys[r][a8];
                    hash_key ^= piece_keys[r][d8];
                    break;
                default:
                    break;
            }
        }
        
        hash_key ^= castling_keys[castling_rights];
        
        castling_rights &= (castling_rights_squares[source] & castling_rights_squares[target]);
        
        hash_key ^= castling_keys[castling_rights];
        
        memset(piece_occupancy, 0ULL, sizeof(piece_occupancy));
                
        for (int piece = P; piece <= K; piece++)
            piece_occupancy[white] |= piece_bitboards[piece];
        
        for (int piece = p; piece <= k; piece++)
            piece_occupancy[black] |= piece_bitboards[piece];

        piece_occupancy[both] |= piece_occupancy[white] | piece_occupancy[black];
        
        turn ^= 1;
        
        hash_key ^= turn_key;
        
        if (is_square_attacked((turn == white) ? ls1b(piece_bitboards[k]) : ls1b(piece_bitboards[K]), turn)) {
            take_back();
            return 0;
        }
        return 1;
        
    } else
        if (get_move_capture(move))
            make_move(move, all);
        return 0;
}

long nodes;

static inline void perft_driver(int depth) {
    if (depth == 0) {
        nodes++;
        return;
    }
    
    moves move_list[1];
    generate_all_moves(move_list);
    
    for (int moves = 0; moves < move_list->count; moves++) {
        copy();
        if (!make_move(move_list->moves[moves], all)) continue;
        perft_driver(depth - 1);
        take_back();
//        display_UCI_move(move_list->moves[moves]);
    }
}

void perft_test(int depth) {
    
    moves move_list[1];
    generate_all_moves(move_list);
    
    auto startTime = chrono::high_resolution_clock::now();
    
    for (int moves = 0; moves < move_list->count; moves++) {
        copy();
        if (!make_move(move_list->moves[moves], all)) continue;
        long cummulative_nodes = nodes;
        perft_driver(depth - 1);
        long old_nodes = nodes - cummulative_nodes;
        take_back();
        printf("     move: %s%s%c  nodes: %ld\n", squares[get_move_source(move_list->moves[moves])],
                                                        squares[get_move_target(move_list->moves[moves])],
                                                        get_move_promoted(move_list->moves[moves])
               ?
//               promoted_pieces[get_move_promoted(move_list->moves[moves])]
               find_promoted_piece_index(get_move_promoted(move_list->moves[moves]))
               : ' ',
                                                        old_nodes);
    }
    auto endTime = chrono::high_resolution_clock::now();
    printf("\n     Depth: %d\n", depth);
    printf("     Nodes: %ld\n", nodes);
    cout << "     Time: " << chrono::duration_cast < chrono::milliseconds > (endTime - startTime).count() << "ms" << endl;
}

int parse_move(char *move_string) {
    moves move_list[1];
    generate_all_moves(move_list);
    int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
    int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        int move = move_list->moves[move_count];
        if (source_square == get_move_source(move) && target_square == get_move_target(move))
        {
            int promoted_piece = get_move_promoted(move);
            if (promoted_piece)
            {
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q')
                    return move;
                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r')
                    return move;
                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b')
                    return move;
                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n')
                    return move;
                continue;
            }
            
            // return legal move
            return move;
        }
    }
    // return illegal move
    return 0;
}

static inline int evaluate() {
    Bitboard board_copy;
    int square;
    int score = 0;
    
    for (int bb_piece = P; bb_piece <= k; bb_piece++) {
        board_copy = piece_bitboards[bb_piece];
        while (board_copy) {
            square = ls1b(board_copy);
            score += material_score[bb_piece];
            switch (bb_piece) {
                case P:
                    score += pawn_scores[square];
                    if (count_bits(piece_bitboards[P] & file_masks[square]) > 1) score += count_bits(piece_bitboards[P] & file_masks[square]) * double_pawn_penalty;
                    if (!(piece_bitboards[P] & isolated_pawn_masks[square])) score += isolated_pawn_penalty;
                    if (!(piece_bitboards[P] & white_passed_pawn_masks[square])) score += passed_pawn_bonus[get_rank[square]];
                    break;
                case N:
                    score += knight_scores[square];
                    break;
                case B:
                    score += bishop_scores[square];
                    score += count_bits(get_bishop_attacks(square, piece_occupancy[both]));
                    break;
                case R:
                    score += rook_scores[square];
                    if (!(piece_bitboards[P] & file_masks[square])) score +=     semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score += open_file_score;
                    break;
                case K:
                    score += king_scores[square];
                    if (!(piece_bitboards[P] & file_masks[square])) score -= semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score -= open_file_score;
                    score += count_bits(king_attack_moves[square] & piece_occupancy[white]) * king_safety_bonus;
                    break;
                case Q:
                    score += count_bits(get_queen_attacks(square, piece_occupancy[both]));
                    break;
                
                case p:
                    score -= pawn_scores[mirror_scores[square]];
                    if (count_bits(piece_bitboards[p] & file_masks[square]) > 1) score -= count_bits(piece_bitboards[P] & file_masks[square]) * double_pawn_penalty;
                    if (!(piece_bitboards[p] & isolated_pawn_masks[square])) score -= isolated_pawn_penalty;
                    if (!(piece_bitboards[p] & black_passed_pawn_masks[square])) score -= passed_pawn_bonus[get_rank[mirror_scores[square]]];
                    break;
                case n:
                    score -= knight_scores[mirror_scores[square]];
                    break;
                case b:
                    score -= bishop_scores[mirror_scores[square]];
                    score -= count_bits(get_bishop_attacks(square, piece_occupancy[both]));
                    break;
                case r:
                    score -= rook_scores[mirror_scores[square]];
                    if (!(piece_bitboards[p] & file_masks[square])) score -= semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score -= open_file_score;
                    break;
                case k:
                    score -= king_scores[mirror_scores[square]];
                    if (!(piece_bitboards[P] & file_masks[square])) score += semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score += open_file_score;
                    score -= count_bits(king_attack_moves[square] & piece_occupancy[black]) * king_safety_bonus;
                    break;
                case q:
                    score -= count_bits(get_queen_attacks(square, piece_occupancy[both]));
                    break;
                default:
                    break;
            }
            pop_bit(board_copy, square);
        }
    }
    if (turn == white) {
        return score;
    } return -score;
}
// half move

static inline int score_move(int move) {
    if (score_pv) {
        if (pv_table[0][ply] == move) {
            score_pv = 0;
            return 20000;
        }
    }
    if (get_move_capture(move)) {
        int target = P;
        
        int start = (turn == white) ? p : P;
        int end = (turn == white) ? k : K;
        
        for (int bitboard_piece = start; bitboard_piece <= end; bitboard_piece++) {
            if (bit_on_square(piece_bitboards[bitboard_piece], get_move_target(move))) {
                target = bitboard_piece;
                break;
            }
        }
        return mvv_lva[get_move_piece(move)][target] + 10000;
    }
    else {
        if (killer[0][ply] == move) return 9000;
        else if (killer[1][ply] == move) return 8000;
        else return history_moves[get_move_piece(move)][get_move_target(move)];

    }
    return 0;
}

void display_move_score(moves * move_list) {
    printf("Move Scores:\n\n");
    for (int count = 0; count < move_list->count; count++) {
        display_UCI_move(move_list->moves[count]);
        printf(" score: %d\n", score_move(move_list->moves[count]));
    }
}

static inline void sort_moves(moves *move_list) {
    int scores[move_list->count];
    for (int cnt = 0; cnt < move_list->count; cnt++) {
        scores[cnt] = score_move(move_list->moves[cnt]);
    }
    for (int current_move = 0; current_move < move_list->count; current_move++)
        {
            // loop over next move within a move list
            for (int next_move = current_move + 1; next_move < move_list->count; next_move++)
            {
                // compare current and next move scores
                if (scores[current_move] < scores[next_move])
                {
                    // swap scores
                    swap(scores[current_move], scores[next_move]);
                    
                    // swap moves
                    swap(move_list->moves[current_move], move_list->moves[next_move]);
                }
            }
        }
}

static inline int quiescence(int alpha, int beta) {
    if((nodes & 2047) == 0) communicate();
    nodes++;
    if (ply > max_ply - 1) return evaluate();
    int evaluation = evaluate();
    
    if (evaluation >= beta) {
        // node (move) fails high
        return beta;
    }
    
    // better move
    if (evaluation > alpha) {
        // pv node (move)
        alpha = evaluation;
    }
    
    moves move_list[1];
    generate_all_moves(move_list);
    sort_moves(move_list);
    for (int count = 0; count < move_list->count; count++) {
        copy();
        ply++;
        
        repetition_index++;
        repetitions[repetition_index] = hash_key;

        if (!make_move(move_list->moves[count], captures)) {
            ply--;
            repetition_index--;
            continue;
        }
    
        int score = -quiescence(-beta, -alpha);
        take_back();
        ply--;
        repetition_index--;
        
        if (stopped) return 0;
        
        // fail-hard beta cutoff
        // the score can't be greater than alpha-beta bounds, soft-hard can.
        // better move
        if (score > alpha) {
            // pv node (move)
            alpha = score;
            
            if (score >= beta) {
                // node (move) fails high
                return beta;
            }
        }
    }
    return alpha;
}

static inline void enable_pv_scoring(moves * move_list) {
    is_following_pv = 0;
    for (int count = 0; count < move_list->count; count++) {
        if (pv_table[0][ply] == move_list->moves[count]) {
            score_pv = 1;
            is_following_pv = 1;
        }
    }
}

const int full_depth_moves = 4;
const int reduction_limit = 3;

int negamax(int alpha, int beta, int depth) {
    int score;
    int flag = hashfALPHA;
    if (ply && is_repetition()) return 0;
//    int pv_node = ((beta - alpha) > 1);
//    if (!pv_node && ply && (score = probe_hash(alpha, beta, depth)) != no_hash_entry) return score;
    if (ply && (score = probe_hash(alpha, beta, depth)) != no_hash_entry) return score;
    if((nodes & 2047) == 0) communicate();
    pv_length[ply] = ply;
    if (depth == 0)
        return quiescence(alpha, beta);
    if (ply > max_ply - 1) {
      return evaluate();
    }
    nodes++;
    int is_king_in_check = is_square_attacked((turn == white ? ls1b(piece_bitboards[K]) : ls1b(piece_bitboards[k])), turn ^ 1);
    if (is_king_in_check) depth++;
    int legal_moves_count = 0;
    if (depth >= 3 && is_king_in_check == 0 && ply) {
        copy();
        ply++;
        repetition_index++;
        repetitions[repetition_index] = hash_key;

        if (en_passant != 1) hash_key ^= enpassant_keys[en_passant];
        turn ^= 1;
        hash_key ^= turn_key;
        en_passant = -1;
        score = -negamax(-beta, -beta + 1, depth - 1 - 2);
        take_back();
        ply--;
        repetition_index--;
        if (stopped) return 0;
        if (score >= beta) return beta;
    }
    moves move_list[1];
    generate_all_moves(move_list);
    if (is_following_pv) {
        enable_pv_scoring(move_list);
    }

    sort_moves(move_list);

    int moves_searched = 0;

    for (int count = 0; count < move_list->count; count++) {
        copy();
        ply++;
        repetition_index++;
        repetitions[repetition_index] = hash_key;

        if (!make_move(move_list->moves[count], all)) {
            ply--;
            repetition_index--;
            continue;
        }

        legal_moves_count++;

        if (!moves_searched)
            score = -negamax(-beta, -alpha, depth - 1);
        else {
            if (moves_searched >= full_depth_moves && depth >= reduction_limit && !is_king_in_check && !get_move_capture(move_list->moves[count]) && !get_move_promoted(move_list->moves[count]))
                score = -negamax(-alpha - 1, -alpha, depth - 2);
            else score = alpha + 1;

            if (score > alpha) {
                score = -negamax(-alpha - 1, -alpha, depth-1);
                if((score > alpha) && (score < beta))
                    score = -negamax(-beta, -alpha, depth-1);
            }
        }

        take_back();
        ply--;
        repetition_index--;

        if(stopped) return 0;
        moves_searched++;


        // better move
        if (score > alpha) {
            flag = hashfEXACT;
            if (!get_move_capture(move_list->moves[count])) {
                history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;
            }

            // pv node (move)

            alpha = score;

            pv_table[ply][ply] = move_list->moves[count];
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++) {
              pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            }
            pv_length[ply] = pv_length[ply + 1];

            // fail-hard beta cutoff
            // the score can't be greater than alpha-beta bounds, soft-hard can.
            if (score >= beta) {
                record_hash(depth, beta, hashfBETA);
                // node (move) fails high
                if (!get_move_capture(move_list->moves[count])) {
                    killer[1][ply] = killer[0][ply];
                    killer[0][ply] = move_list->moves[count];
                }
                return beta;
            }
        }
    }
    if (!legal_moves_count) {
        if (is_king_in_check)
            return -mate_val + ply;
        return 0;
    }

    record_hash(depth, alpha, flag);
    return alpha;
}

void search_position(int depth) {

    nodes = 0;
    int score = 0;
    is_following_pv = 0;
    score_pv = 0;
    stopped = 0;

    memset(killer, 0, sizeof(killer));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));

    int alpha = -Infinity;
    int beta = Infinity;

    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        if (stopped) break;
        is_following_pv = 1;

        score = negamax(alpha, beta, current_depth);

//        if ((score <= alpha) || (score >= beta)) {
//            alpha = -Infinity;
//            beta = Infinity;
//            current_depth--;
//            continue;
//        }
//
//        alpha = score - 50;
//        beta = score + 50;
        
        if (pv_length[0]) {
            if (score > -mate_val && score < -mate_score)
                printf("info score mate %d depth %d nodes %ld time %d pv ", -(score + mate_val) / 2 - 1, current_depth, nodes, get_time_ms() - starttime);
            else if (score > mate_score && score < mate_val)
                printf("info score mate %d depth %d nodes %ld time %d pv ", (mate_val - score) / 2 + 1, current_depth, nodes, get_time_ms() - starttime);
            else
                printf("info score cp %d depth %d nodes %ld time %d pv ", score, current_depth, nodes, get_time_ms() - starttime);

            for (int count = 0; count < pv_length[0]; count++) {
                display_UCI_move(pv_table[0][count]);
                printf(" ");
            }
            printf("\n");
        }
    }

    printf("bestmove ");
    display_UCI_move(pv_table[0][0]);
    printf("\n");

}

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
void parse_position(char *command) // taken from BBC.c (Code Monkey King)
{
    // shift pointer to the right where next token begins
    command += 9;
    
    // init pointer to the current character in the command string
    char *current_char = command;
    
    // parse UCI "startpos" command
    if (!strncmp(command, "startpos", 8))
        // init chess board with start position
        parse_fen((char *) starting_position);
    
    // parse UCI "fen" command
    else
    {
        // make sure "fen" command is available within command string
        current_char = strstr(command, "fen");
        
        // if no "fen" command is available within command string
        if (current_char == NULL)
            // init chess board with start position
            parse_fen((char *)starting_position);
            
        // found "fen" substring
        else
        {
            // shift pointer to the right where next token begins
            current_char += 4;
            
            // init chess board with position from FEN string
            parse_fen(current_char);
        }
    }
    
    // parse moves after position
    current_char = strstr(command, "moves");
    
    // moves available
    if (current_char != NULL)
    {
        // shift pointer to the right where next token begins
        current_char += 6;
        
        // loop over moves within a move string
        while(*current_char)
        {
            // parse next move
            int move = parse_move(current_char);
            
            // if no more moves
            if (move == 0)
                // break out of the loop
                break;
            
            repetition_index++;
            repetitions[repetition_index] = hash_key;
            
            // make move on the chess board
            make_move(move, all);
            
            // move current character mointer to the end of current move
            while (*current_char && *current_char != ' ') current_char++;
            
            // go to the next move
            current_char++;
        }
    }
    styled_board();
}

//void parse_go(char * command) {
//    int depth = -1;
//    char * current_depth = NULL;
//    current_depth = strstr(command, "depth");
//    if (current_depth) {
//        depth = atoi(current_depth + 6);
//    } else depth = 6;
//
//    search_position(depth);
//    // call search pos given depth
//}

void parse_go(char *command) {
    reset_time();
    int depth = -1;
    char *argument = NULL;
    if ((argument = strstr(command,"infinite"))) {}
    if ((argument = strstr(command,"binc")) && turn == black)
        inc = atoi(argument + 5);
    if ((argument = strstr(command,"winc")) && turn == white)
        inc = atoi(argument + 5);
    if ((argument = strstr(command,"wtime")) && turn == white)
        time_ = atoi(argument + 6);
    if ((argument = strstr(command,"btime")) && turn == black)
        time_ = atoi(argument + 6);
    if ((argument = strstr(command,"movestogo")))
        movestogo = atoi(argument + 10);
    if ((argument = strstr(command,"movetime")))
        movetime = atoi(argument + 9);
    if ((argument = strstr(command,"depth")))
        depth = atoi(argument + 6);
    if(movetime != -1) {
        time_ = movetime;
        movestogo = 1;
    }
    starttime = get_time_ms();
    depth = depth;
    if(time_ != -1) {
        timeset = 1;
        time_ /= movestogo;
        if (time_ > 1500) time_ -= 50;
        stoptime = starttime + time_ + inc;
    }
    
    if (depth == -1) depth = 64;
    printf("time: %d start: %u stop: %u depth: %d timeset: %d\n",
    time_, starttime, stoptime, depth, timeset);

    // search position
    search_position(depth);
}

void UCI_loop() {
    
//    int max_hash = 128;
//    int mb = 64;
    
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    
    char input[3000];
    
    printf("id name Monza\n");
    printf("id author Ziyad Mourabiti\n");
//    printf("option name Hash type spin default 64 min 6 max %d\n", max_hash);
    printf("uciok\n"); // chess engine speaks UCI
    
    while (1) {
        memset(input, 0, sizeof(input));
        fflush(stdout);
        if (!fgets(input, 3000, stdin) || input[0] == '\n') continue;
        
        else
            if (!strncmp(input, "isready", 7)) {
            cout << "readyok" << endl;
            continue;
        }
        else if (!strncmp(input, "position", 8)) {
            parse_position(input);
        }
        else if (!strncmp(input, "ucinewgame", 10)) {
            parse_position((char *)"position startpos");
            clear_t_table();
        }
        else if (!strncmp(input, "go", 2)) {
            parse_go(input);
        }
        else if (!strncmp(input, "quit", 4)) break;
        else if (!strncmp(input, "uci", 3)) {
            printf("id name Monza\n");
            printf("id author Ziyad Mourabiti\n");
            printf("uciok\n"); // chess engine speaks UCI
        }
//        else if (!strncmp(input, "setoption name Hash value ", 26)) {
//            sscanf(input,"%*s %*s %*s %*s %d", &mb);
//            if (mb < 4) mb = 4;
//            if (mb > max_hash) mb = max_hash;
//            printf("    Set hash table size to %dMB\n", mb);
//            init_t_table(mb);
//        }
    }
}

void init_eval() {
    
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            file_masks[square] |= set_mask(file, -1);
            rank_masks[square] |= set_mask(-1, rank);
            isolated_pawn_masks[square] |= set_mask(file - 1, -1);
            isolated_pawn_masks[square] |= set_mask(file + 1, -1);
        }
    }
    
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            black_passed_pawn_masks[square] |= set_mask(file - 1, -1);
            black_passed_pawn_masks[square] |= set_mask(file, -1);
            black_passed_pawn_masks[square] |= set_mask(file + 1, -1);
            for (int i = 0; i < rank + 1; i++)
                black_passed_pawn_masks[square] &= ~rank_masks[i * 8 + file];
        }
    }
    
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            white_passed_pawn_masks[square] |= set_mask(file - 1, -1);
            white_passed_pawn_masks[square] |= set_mask(file, -1);
            white_passed_pawn_masks[square] |= set_mask(file + 1, -1);
            for (int i = 0; i < (8 - rank); i++)
                white_passed_pawn_masks[square] &= ~rank_masks[(7 - i) * 8 + file];
          
//            printf("%s\n", squares[square]);
//            board(white_passed_pawn_masks[square]);
        }
    }
}

void init() {
    init_non_sliding_pieces();
    init_sliding_pieces(bishop);
    init_sliding_pieces(rook);
    init_rand_keys();
    init_eval();
//    init_t_table(64);
//    init_magic();
    
}

int main(int argc, const char * argv[]) {
    
    init();
    
//    parse_fen(tricky_position);
//    styled_board();
//    search_position(10);

    
    UCI_loop();
    
//    free(transposition_table);
    
    return 0;
}
