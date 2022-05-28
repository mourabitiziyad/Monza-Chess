//
//  random.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 27/4/2022.
//


#include "types.h"
#include "random.h"

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
    un1 = (Bitboard)(random_number()) & 0xFFFF; un2 = (Bitboard)(random_number()) & 0xFFFF;
    un3 = (Bitboard)(random_number()) & 0xFFFF; un4 = (Bitboard)(random_number()) & 0xFFFF;
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
