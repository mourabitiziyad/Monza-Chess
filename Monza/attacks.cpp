//
//  attacks.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "attacks.hpp"

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

// https://www.chessprogramming.org/Magic_Bitboards#Plain

Bitboard get_bishop_attacks(int square, Bitboard occupancy) {
    
    occupancy &= bishop_attack_masks[square];
    occupancy *= bishop_magics[square];
    occupancy >>= 64 - bishop_relevant_occupancy_bits[square];
    return bishop_attack_moves[square][occupancy];
    
}

Bitboard get_rook_attacks(int square, Bitboard occupancy) {
    
    occupancy &= rook_attack_masks[square];
    occupancy *= rook_magics[square];
    occupancy >>= 64 - rook_relevant_occupancy_bits[square];
    return rook_attack_moves[square][occupancy];
    
}

Bitboard get_queen_attacks(int square, Bitboard occupancy) {
    return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);
}
