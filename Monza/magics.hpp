//
//  magics.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#ifndef magics_hpp
#define magics_hpp

#include <stdio.h>
#include "types.h"
#include "random.h"
#include "attacks.hpp"

#include <iostream>
using namespace std;

#include <string.h>

Bitboard get_occupancy_set(int idx, int bits, Bitboard attacks);
Bitboard find_magic(int square, int bits, int bishop);


#endif /* magics_hpp */
