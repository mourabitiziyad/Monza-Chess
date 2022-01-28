//
//  constants.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 8/1/2022.
//

#ifndef constants_hpp
#define constants_hpp

#include <stdio.h>
#include <stdlib.h>

#endif /* constants_hpp */

typedef unsigned long long Bitboard;
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
};

enum {
    white,
    black
};

const Bitboard notAFile  = 18374403900871474942ULL; // 0xfefefefefefefefe; // ~0x0101010101010101
const Bitboard notABFile = 18229723555195321596ULL;
const Bitboard notHFile  = 9187201950435737471ULL; // 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080
const Bitboard notHGFile = 4557430888798830399ULL;

const char * squares[64] = {
 "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
 "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
 "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
 "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
 "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
 "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
 "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
 "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1" };
 

/*
 for (int i=8; i>=1; i--)
     printf("\"a%d\", \"b%d\", \"c%d\", \"d%d\", \"e%d\", \"f%d\", \"g%d\", \"h%d\"\n", i,i,i,i,i,i,i,i);
 */

// Bitboard Macros

// # define pop_bit(board, square) (bit_on_square(board, square) ? (board) ^= (1ULL << square) : 0)
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

unsigned int initial_random = 1804289383;

unsigned int random_number(){
    unsigned int new_random = initial_random;
    
    new_random ^= new_random << 13;
    new_random ^= new_random >> 17;
    new_random ^= new_random << 5;
    
    return initial_random = new_random;
}

Bitboard random_U64_number(){
    Bitboard un1, un2, un3, un4;
    un1 = (Bitboard)(random()) & 0xFFFF; un2 = (Bitboard)(random()) & 0xFFFF;
    un3 = (Bitboard)(random()) & 0xFFFF; un4 = (Bitboard)(random()) & 0xFFFF;
    return un1 | (un2 << 16) | (un3 << 32) | (un4 << 48);
}

Bitboard random_U64_fewbits() {
  return random_U64_number() & random_U64_number() & random_U64_number();
}

// the random number generator is to counter act potential compatibility issues with using `random()` function
// in <stdlib.h> across different compilers
// There could obviously be better, more efficient ways of doing this.
// It depends on whether or not its impact is significant enough to adopt it.
// https://en.wikipedia.org/wiki/Xorshift#Example_implementation
