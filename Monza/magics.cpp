//
//  magics.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "magics.hpp"

Bitboard get_occupancy_set(int idx, int bits, Bitboard attacks) {
    Bitboard occupancy = 0ULL;
    for (int x = 0; x < bits; x++) {
        int square = ls1b(attacks);
        pop_bit(attacks, square);
        if (idx & (1 << x)) occupancy |= (1ULL << square); // What are we checking for here?
    }
    return occupancy;
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
