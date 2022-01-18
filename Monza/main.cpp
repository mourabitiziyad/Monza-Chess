//
//  main.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 8/1/2022.
//

#include "constants.hpp"

#include <iostream>
using namespace std;

//      +---+---+---+---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ Printing the Board ---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+

void board(Bitboard bitboard) {
    for (int rank=0; rank<8; rank++) {
        cout << "   +---+---+---+---+---+---+---+---+\n";
        for (int file=0; file<8; file++) {
            
            !file && cout << (8 - rank) << "  ";
            
            cout << "| " << (bit_on_square(bitboard, (rank * 8 + file)) ? 1 : 0) << " ";
        }
        cout << "|\n";
    }
    cout << "   +---+---+---+---+---+---+---+---+\n\n  ";
    for (char i='A'; i<='H'; i++)
        cout << "   " << i;
    cout << "\n\n     Bitboard: " << bitboard;
    cout << "\n\n";
}

//      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ Pre-Calculated Attacks ---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+

Bitboard pawn_attack_moves[2][64]; // [Side] [Square]
Bitboard knight_attack_moves[64];

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

//      +---+---+---+---+---+---+---+---+---+---+
//      +---+---+---+---+ main  +---+---+---+---+
//      +---+---+---+---+---+---+---+---+---+---+

int main(int argc, const char * argv[]) {
//    set_bit(bitboard, e4);
//    set_bit(bitboard, h1);
//    board();
//    pop_bit(bitboard, h1);
//    board();
    board(mask_knight_attacks(b2));

    return 0;
}
