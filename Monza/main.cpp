//
//  main.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 8/1/2022.
//

#include "constants.hpp"

#include <iostream>
using namespace std;

Bitboard bitboard = 4ULL;

void board() {
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

int main(int argc, const char * argv[]) {
    set_bit(bitboard, e4);
    set_bit(bitboard, h1);
    board();
    pop_bit(bitboard, h1);
    board();

    return 0;
}
