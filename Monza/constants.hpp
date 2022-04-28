//
//  constants.hpp
//  Monza
//
//  Created by Ziyad Mourabiti on 8/1/2022.
//

#ifndef constants_hpp
#define constants_hpp

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
using namespace std;


#endif /* constants_hpp */

#include "types.h"
#include "random.h"
#include "macros.hpp"
#include "enums.h"
#include "globals.h"


/*
          binary move bits                               hexidecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/

// int char_pieces[] = {
//     ['P'] = P,
//     ['N'] = N,
//     ['B'] = B,
//     ['R'] = R,
//     ['Q'] = Q,
//     ['K'] = K,
//     ['p'] = p,
//     ['n'] = n,
//     ['b'] = b,
//     ['r'] = r,
//     ['q'] = q,
//     ['k'] = k
// };

//char promoted_pieces[] = {
//    [Q] = 'q',
//    [R] = 'r',
//    [B] = 'b',
//    [N] = 'n',
//    [q] = 'q',
//    [r] = 'r',
//    [b] = 'b',
//    [n] = 'n',
//};


//void clear_t_table() {
//    t_table *hash_entry;
//    for (hash_entry = transposition_table; hash_entry < transposition_table + t_size; hash_entry++) {
//        hash_entry->value = 0;
//        hash_entry->depth = 0;
//        hash_entry->flag = 0;
//        hash_entry->key = 0;
//    }
//}



//void init_t_table(int mb) {
//    int hash_size = 0x100000 * mb;
//    t_size = hash_size / sizeof(t_table);
//    if (transposition_table != NULL) {
//        printf("Clearing Hash Memory...\n");
//        free(transposition_table);
//    }
//    transposition_table = (t_table *) malloc(t_size * sizeof(t_table));
//    if (transposition_table == NULL) {
//        printf("Couldn't Allocate Memory, trying %dmb...\n", mb/2);
//        init_t_table(mb/2);
//    } else {
//        clear_t_table();
//        printf("Hash Table is initialised with %d entries\n", t_size);
//    }
//}
