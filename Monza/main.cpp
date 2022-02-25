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

int turn = -1;
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
    
    for (int square = 0; square < 64 && *fen_string && *fen_string!=' ';) {
        if (isalpha(*fen_string)) set_bit(piece_bitboards[char_pieces[*fen_string++]], square++);
        else if (isdigit(*fen_string)) square += (*fen_string++ - '0');
        else if (*fen_string == '/') *fen_string++;
        else memset(piece_bitboards, 0ULL, sizeof(piece_bitboards));
    }
    *(++fen_string) == 'b' ? (turn = black) : (turn = white);
    *(fen_string += 2);
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
    printf("%s%s%c\n",  squares[get_move_source(move)],
                        squares[get_move_target(move)],
                        promoted_pieces[get_move_promoted(move)]);
}

void display_move_list(moves *move_list) {
    printf("\nMove       Piece       isCapture?      isDoublePush?       Enpassant?        Castling?\n\n");
    for (int count = 0; count < move_list->count; count++) {
        printf("%s%s%c      %s           %d               %d                   %d                 %d\n",
                            squares[get_move_source(move_list->moves[count])],
                            squares[get_move_target(move_list->moves[count])],
                            get_move_promoted(move_list->moves[count]) ? promoted_pieces[get_move_promoted(move_list->moves[count])] : ' ',
                            unicode_pieces[get_move_piece(move_list->moves[count])],
                            get_move_capture(move_list->moves[count]) ? 1 : 0,
                            get_move_double(move_list->moves[count]) ? 1 : 0,
                            get_move_enpassant(move_list->moves[count]) ? 1 : 0,
                            get_move_castling(move_list->moves[count]) ? 1 : 0);

    }
    printf("\nNumber of Moves: %d\n\n", move_list->count);
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
//    parse_fen("8/8/4R3/3b4/8/8/8/8 w - - ");
    parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPpP/R3K2R b KQkq a3 0 1");
    styled_board();
//    board(piece_occupancy[both]);
//    print_attacked_squares(black);
//    generate_all_moves();
//    board(piece_occupancy[both]);
//    board(piece_occupancy[both]);
//    styled_board();
    
//    set_bit(piece_bitboards[P], a2);
//    set_bit(piece_bitboards[P], b2);
    moves move_list[1];
    generate_all_moves(move_list);
    display_move_list(move_list);
    // exract move items
//    int source_square = get_move_source(move);
//    int target_square = get_move_target(move);
//    int piece = get_move_piece(move);
//    int promoted_piece = get_move_promoted(move);
//
//    // print move items
//    printf("source square: %s\n", squares[source_square]);
//    printf("target square: %s\n", squares[target_square]);
//    printf("piece: %s\n", unicode_pieces[piece]);
//    printf("piece: %s\n", unicode_pieces[promoted_piece]);
//    printf("capture flag: %d\n", get_move_capture(move) ? 1 : 0);
//    printf("double pawn push flag: %d\n", get_move_double(move) ? 1 : 0);
//    printf("enpassant flag: %d\n", get_move_enpassant(move) ? 1 : 0);
//    printf("castling flag: %d\n", get_move_castling(move) ? 1 : 0);
//    printf("test: %c\n", (char)char_pieces[2]);
    
    return 0;
}
