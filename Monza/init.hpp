//
//  init.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#ifndef init_hpp
#define init_hpp

#include <stdio.h>
#include "attacks.hpp"
#include "enums.h"
#include "magics.hpp"
#include "zobrist.hpp"

#include <iostream>
using namespace std;

void init_non_sliding_pieces();
void init_magic();
void init_sliding_pieces(int bishop);
void init_eval();
void init();

#endif /* init_hpp */
