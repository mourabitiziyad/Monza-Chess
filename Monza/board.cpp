//
//  board.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 27/4/2022.
//

#include "board.hpp"
#include "macros.hpp"
#include "enums.h"
#include "globals.h"

#include <iostream>
using namespace std;

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

int find_piece_index(char piece) {
    switch (piece) {
        case 'P':
             return P;
            break;
        case 'N':
             return N;
            break;
        case 'B':
             return B;
            break;
        case 'R':
             return R;
            break;
        case 'Q':
             return Q;
            break;
        case 'K':
             return K;
            break;
        case 'p':
             return p;
            break;
        case 'n':
             return n;
            break;
        case 'b':
             return b;
            break;
        case 'r':
             return r;
            break;
        case 'q':
             return q;
            break;
        case 'k':
             return k;
            break;
        default:
            return -1;
    }
}

char find_promoted_piece_index(int piece) {
    switch (piece) {
        case N:
             return 'n';
            break;
        case B:
             return 'b';
            break;
        case R:
             return 'r';
            break;
        case Q:
             return 'q';
            break;
        case n:
             return 'n';
            break;
        case b:
             return 'b';
            break;
        case r:
             return 'r';
            break;
        case q:
             return 'q';
            break;
        default:
            return -1;
    }
}
