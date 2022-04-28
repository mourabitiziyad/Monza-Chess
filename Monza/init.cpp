//
//  init.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "init.hpp"

void init_non_sliding_pieces(){
    for (int square = 0; square < 64; square++) {
        
        pawn_attack_moves[white][square] = mask_pawn_attacks(white, square);
        pawn_attack_moves[black][square] = mask_pawn_attacks(black, square);
        
        knight_attack_moves[square] = mask_knight_attacks(square);
        
        king_attack_moves[square] = mask_king_attacks(square);
    }
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


Bitboard set_mask(int file, int rank) {
    Bitboard mask = 0ULL;
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            int square = r * 8 + f;
            if (file != -1) {
                if (f == file)
                    mask |= set_bit(mask, square);
            }
            else if (rank != -1) {
                if (r == rank)
                    mask |= set_bit(mask, square);
            }
        }
    }
    return mask;
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
