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
#if defined(WIN64) || defined(WIN32)
    #include <windows.h>
#else
    # include <sys/time.h>
#endif

# define mate_score 48000
# define mate_val 49000
# define Infinity 50000

int turn = -1;
int castling_rights = 0;
int en_passant = -1;
int ply;

int quit = 0;
int movestogo = 30;
int movetime = -1;
int time_ = -1;
int inc = 0;
int starttime = 0;
int stoptime = 0;
int timeset = 0;
int stopped = 0;

void reset_time() {
    quit = 0;
    movestogo = 30;
    movetime = -1;
    time_ = -1;
    inc = 0;
    starttime = 0;
    stoptime = 0;
    timeset = 0;
    stopped = 0;
}

int get_time_ms()
{
    #if defined(WIN64) || defined(WIN32)
        return GetTickCount();
    #else
        struct timeval time_value;
        gettimeofday(&time_value, NULL);
        return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    #endif
}

int input_waiting()
{
    #ifndef WIN32
        fd_set readfds;
        struct timeval tv;
        FD_ZERO (&readfds);
        FD_SET (fileno(stdin), &readfds);
        tv.tv_sec=0; tv.tv_usec=0;
        select(16, &readfds, 0, 0, &tv);

        return (FD_ISSET(fileno(stdin), &readfds));
    #else
        static int init = 0, pipe;
        static HANDLE inh;
        DWORD dw;

        if (!init)
        {
            init = 1;
            inh = GetStdHandle(STD_INPUT_HANDLE);
            pipe = !GetConsoleMode(inh, &dw);
            if (!pipe)
            {
                SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
                FlushConsoleInputBuffer(inh);
            }
        }
        
        if (pipe)
        {
           if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
           return dw;
        }
        
        else
        {
           GetNumberOfConsoleInputEvents(inh, &dw);
           return dw <= 1 ? 0 : dw;
        }

    #endif
}

void read_input()
{
    
    int bytes;
    char input[256] = "", *endc;
    if (input_waiting())
    {
        stopped = 1;
        do {
            bytes=read(fileno(stdin), input, 256);
        }
        while (bytes < 0);
        endc = strchr(input,'\n');
        if (endc) *endc=0;
        if (strlen(input) > 0)
        {
            if (!strncmp(input, "quit", 4))
            {
                quit = 1;
            }
            else if (!strncmp(input, "stop", 4))    {
                quit = 1;
            }
        }
    }
}

static void communicate() {
    // if time is up break here
    if(timeset == 1 && get_time_ms() > stoptime) {
        // tell engine to stop calculating
        stopped = 1;
    }
    
    // read GUI input
    read_input();
}

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

const int castling_rights_squares[64] = {
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

# define max_ply 64

int pv_length[max_ply];
int pv_table[max_ply][max_ply];

int is_following_pv, score_pv;

int killer[2][max_ply];
int history_moves[12][64];

typedef struct moves {
    int moves[256];
    int count = 0;
} moves;

void add_move(moves *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

typedef unsigned long long Bitboard;

Bitboard piece_bitboards[12];
Bitboard piece_occupancy[3]; // white, black, both

Bitboard piece_keys[12][64];
Bitboard enpassant_keys[64];
Bitboard castling_keys[16];
Bitboard turn_key;
Bitboard hash_key;
Bitboard repetitions[500];

Bitboard file_masks[64];
Bitboard rank_masks[64];
Bitboard isolated_pawn_masks[64];
Bitboard white_passed_pawn_masks[64];
Bitboard black_passed_pawn_masks[64];

int repetition_index = 0;

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
    rook,
    bishop
};

enum {
    white,
    black,
    both
};

enum {
    wq = 2,
    bq = 8,
    wk = 1,
    bk = 4
};

enum {
    P, N, B, R, Q, K, p, n, b, r, q, k
};

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

const int get_rank[64] = {
    7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0
};

const int double_pawn_penalty = -10;
const int isolated_pawn_penalty = -10;
const int passed_pawn_bonus[8] = { 0, 10, 30, 50, 75, 100, 150, 200 };
const int semi_open_file_score = 10;
const int open_file_score = 15;
const int king_safety_bonus = 5;

static int mvv_lva[12][12] = {
    105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600,
    105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600
};

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

# define t_size 5600000  // times the size of a tt struct equals about 112 mb
//# define t_size 800000 // about 16 mb
//int t_size = 0;

# define hashfEXACT 0
# define hashfALPHA 1
# define hashfBETA 2

typedef struct tagHASHE {
    Bitboard key;
    int depth;
    int flag;
    int value;
} t_table;

t_table transposition_table[t_size];
//t_table *transposition_table = NULL;

# define clear_t_table() \
    memset(transposition_table, 0, sizeof(transposition_table));
//void clear_t_table() {
//    t_table *hash_entry;
//    for (hash_entry = transposition_table; hash_entry < transposition_table + t_size; hash_entry++) {
//        hash_entry->value = 0;
//        hash_entry->depth = 0;
//        hash_entry->flag = 0;
//        hash_entry->key = 0;
//    }
//}

# define no_hash_entry 100000

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

static inline int probe_hash(int alpha, int beta, int depth) {
    t_table *hash_entry = &transposition_table[hash_key % t_size];
    if (hash_entry->key == hash_key) {
        if (hash_entry->depth >= depth) {
            int score = hash_entry->value;
            if (score < -mate_score) score += ply;
            if (score > mate_score) score -= ply;
            if (hash_entry->flag == hashfEXACT) return score;
            if ((hash_entry->flag == hashfALPHA) && (score <= alpha)) return alpha;
            if ((hash_entry->flag == hashfBETA) && (score >= beta)) return beta;
        }
    }
    return no_hash_entry;
}

static inline void record_hash(int depth, int value, int hash) {
    t_table *hash_entry = &transposition_table[hash_key % t_size];
    
    if (value < -mate_score) value -= ply;
    if (value > mate_score) value += ply;
    
    hash_entry->key = hash_key;
    hash_entry->value = value;
    hash_entry->depth = depth;
    hash_entry->flag = hash;
}

static inline int is_repetition() {
    for (int i = 0; i < repetition_index; i++) if (repetitions[i] == hash_key) return 1;
    return 0;
}

const Bitboard notAFile  = 18374403900871474942ULL; // 0xfefefefefefefefe; // ~0x0101010101010101
const Bitboard notABFile = 18229723555195321596ULL;
const Bitboard notHFile  = 9187201950435737471ULL; // 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080
const Bitboard notHGFile = 4557430888798830399ULL;

const Bitboard bishop_magics[64] = {
    0x420c80100408202ULL,
    0x1204311202260108ULL,
    0x2008208102030000ULL,
    0x24081001000caULL,
    0x488484041002110ULL,
    0x1a080c2c010018ULL,
    0x20a02a2400084ULL,
    0x440404400a01000ULL,
    0x8931041080080ULL,
    0x200484108221ULL,
    0x80460802188000ULL,
    0x4000090401080092ULL,
    0x4000011040a00004ULL,
    0x20011048040504ULL,
    0x2008008401084000ULL,
    0x102422a101a02ULL,
    0x2040801082420404ULL,
    0x8104900210440100ULL,
    0x202101012820109ULL,
    0x248090401409004ULL,
    0x44820404a00020ULL,
    0x40808110100100ULL,
    0x480a80100882000ULL,
    0x184820208a011010ULL,
    0x110400206085200ULL,
    0x1050010104201ULL,
    0x4008480070008010ULL,
    0x8440040018410120ULL,
    0x41010000104000ULL,
    0x4010004080241000ULL,
    0x1244082061040ULL,
    0x51060000288441ULL,
    0x2215410a05820ULL,
    0x6000941020a0c220ULL,
    0xf2080100020201ULL,
    0x8010020081180080ULL,
    0x940012060060080ULL,
    0x620008284290800ULL,
    0x8468100140900ULL,
    0x418400aa01802100ULL,
    0x4000882440015002ULL,
    0x420220a11081ULL,
    0x401a26030000804ULL,
    0x2184208000084ULL,
    0xa430820a0410c201ULL,
    0x640053805080180ULL,
    0x4a04010a44100601ULL,
    0x10014901001021ULL,
    0x422411031300100ULL,
    0x824222110280000ULL,
    0x8800020a0b340300ULL,
    0xa8000441109088ULL,
    0x404000861010208ULL,
    0x40112002042200ULL,
    0x2141006480b00a0ULL,
    0x2210108081004411ULL,
    0x2010804070100803ULL,
    0x7a0011010090ac31ULL,
    0x18005100880400ULL,
    0x8010001081084805ULL,
    0x400200021202020aULL,
    0x4100342100a0221ULL,
    0x404408801010204ULL,
    0x6360041408104012ULL
};

const Bitboard rook_magics[64] = {
    0xa8002c000108020ULL,
    0x6c00049b0002001ULL,
    0x100200010090040ULL,
    0x2480041000800801ULL,
    0x280028004000800ULL,
    0x900410008040022ULL,
    0x280020001001080ULL,
    0x2880002041000080ULL,
    0xa000800080400034ULL,
    0x4808020004000ULL,
    0x2290802004801000ULL,
    0x411000d00100020ULL,
    0x402800800040080ULL,
    0xb000401004208ULL,
    0x2409000100040200ULL,
    0x1002100004082ULL,
    0x22878001e24000ULL,
    0x1090810021004010ULL,
    0x801030040200012ULL,
    0x500808008001000ULL,
    0xa08018014000880ULL,
    0x8000808004000200ULL,
    0x201008080010200ULL,
    0x801020000441091ULL,
    0x800080204005ULL,
    0x1040200040100048ULL,
    0x120200402082ULL,
    0xd14880480100080ULL,
    0x12040280080080ULL,
    0x100040080020080ULL,
    0x9020010080800200ULL,
    0x813241200148449ULL,
    0x491604001800080ULL,
    0x100401000402001ULL,
    0x4820010021001040ULL,
    0x400402202000812ULL,
    0x209009005000802ULL,
    0x810800601800400ULL,
    0x4301083214000150ULL,
    0x204026458e001401ULL,
    0x40204000808000ULL,
    0x8001008040010020ULL,
    0x8410820820420010ULL,
    0x1003001000090020ULL,
    0x804040008008080ULL,
    0x12000810020004ULL,
    0x1000100200040208ULL,
    0x430000a044020001ULL,
    0x280009023410300ULL,
    0xe0100040002240ULL,
    0x200100401700ULL,
    0x2244100408008080ULL,
    0x8000400801980ULL,
    0x2000810040200ULL,
    0x8010100228810400ULL,
    0x2000009044210200ULL,
    0x4080008040102101ULL,
    0x40002080411d01ULL,
    0x2005524060000901ULL,
    0x502001008400422ULL,
    0x489a000810200402ULL,
    0x1004400080a13ULL,
    0x4000011008020084ULL,
    0x26002114058042ULL
};

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
# define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
(source)            | \
(target << 6)       | \
(piece << 12)       | \
(promoted << 16)    | \
(capture << 20)     | \
(double << 21)      | \
(enpassant << 22)   | \
(castling << 23)

// extract source square
# define get_move_source(move) (move & 0x3f)

// extract target square
# define get_move_target(move) ((move & 0xfc0) >> 6)

// extract piece
# define get_move_piece(move) ((move & 0xf000) >> 12)

// extract promoted piece
# define get_move_promoted(move) ((move & 0xf0000) >> 16)

// extract capture flag
# define get_move_capture(move) (move & 0x100000)

// extract double pawn push flag
# define get_move_double(move) (move & 0x200000)

// extract enpassant flag
# define get_move_enpassant(move) (move & 0x400000)

// extract castling flag
# define get_move_castling(move) (move & 0x800000)

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

const int material_score[12] = {
    100,      // white pawn score
    300,      // white knight scrore
    350,      // white bishop score
    500,      // white rook score
   1000,      // white queen score
  10000,      // white king score
   -100,      // black pawn score
   -300,      // black knight scrore
   -350,      // black bishop score
   -500,      // black rook score
  -1000,      // black queen score
 -10000,      // black king score
};

// pawn positional score
const int pawn_scores[64] =
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_scores[64] =
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_scores[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0

};

// rook positional score
const int rook_scores[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_scores[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// mirror positional score tables for opposite side
const int mirror_scores[128] =
{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

# define empty_board "8/8/8/8/8/8/8/8 b - - "
# define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
# define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
# define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
# define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

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

void init_rand_keys() {
    
    initial_random = 1804289383;
    
    for (int piece = P; piece <= k; piece++) for (int sq = 0; sq < 64; sq++) piece_keys[piece][sq] = random_U64_number();
    for (int sq = 0; sq < 64; sq++) enpassant_keys[sq] = random_U64_number();
    for (int idx = 0; idx < 16; idx++) castling_keys[idx] = random_U64_number();
    
    turn_key = random_U64_number();
}

Bitboard generate_hash() {
    Bitboard key = 0ULL;
    Bitboard bitboard_copy;
    
    for (int piece = P; piece <= k; piece++) {
        bitboard_copy = piece_bitboards[piece];
        while (bitboard_copy) {
            int sq = ls1b(bitboard_copy);
            key ^= piece_keys[piece][sq];
            pop_bit(bitboard_copy, sq);
        }
    }
    
    if (en_passant != -1) key ^= enpassant_keys[en_passant];
    key ^= castling_keys[castling_rights];
    if (turn == black) key ^= turn_key;
    
    return key;
}

Bitboard set_mask(int file, int rank) {
    Bitboard mask = 0ULL;
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            int square = r * 8 + f;
            if (file != -1) {
                if (f == file)
                    mask |= set_bit(mask, square);
            }
            else if (rank != -1) {
                if (r == rank)
                    mask |= set_bit(mask, square);
            }
        }
    }
    return mask;
}
