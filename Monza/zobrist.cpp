//
//  zobrist.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "zobrist.hpp"

void init_rand_keys() {
    
//    unsigned int initial_random = 1804289383;
    
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

int probe_hash(int alpha, int beta, int depth) {
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

void record_hash(int depth, int value, int hash) {
    t_table *hash_entry = &transposition_table[hash_key % t_size];
    
    if (value < -mate_score) value -= ply;
    if (value > mate_score) value += ply;
    
    hash_entry->key = hash_key;
    hash_entry->value = value;
    hash_entry->depth = depth;
    hash_entry->flag = hash;
}
