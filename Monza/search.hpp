//
//  search.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#ifndef search_hpp
#define search_hpp

#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "types.h"
#include "globals.h"
#include "enums.h"
#include "time.hpp"
#include "eval.hpp"
#include "defs.h"
#include "zobrist.hpp"
#include "attacks.hpp"
#include "move.hpp"
#include "uci.hpp"
#include "eval.hpp"

static inline int score_move(int move);
void display_move_score(moves * move_list);
static inline void enable_pv_scoring(moves * move_list);
static inline void sort_moves(moves *move_list);
static inline int quiescence(int alpha, int beta);
int negamax(int alpha, int beta, int depth);
void search_position(int depth);

#endif /* search_hpp */
