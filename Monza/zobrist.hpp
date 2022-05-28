//
//  zobrist.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#ifndef zobrist_hpp
#define zobrist_hpp

#include <stdio.h>
#include "types.h"
#include "enums.h"
#include "random.h"
#include "globals.h"

void init_rand_keys();
Bitboard generate_hash();
int probe_hash(int alpha, int beta, int depth);
void record_hash(int depth, int value, int hash);

#endif /* zobrist_hpp */
