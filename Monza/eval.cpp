//
//  eval.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "eval.hpp"

int is_repetition() {
    for (int i = 0; i < repetition_index; i++) if (repetitions[i] == hash_key) return 1;
    return 0;
}

/*
 for (int i=8; i>=1; i--)
     printf("\"a%d\", \"b%d\", \"c%d\", \"d%d\", \"e%d\", \"f%d\", \"g%d\", \"h%d\"\n", i,i,i,i,i,i,i,i);
 */

int evaluate() {
    Bitboard board_copy;
    int square;
    int score = 0;
    
    for (int bb_piece = P; bb_piece <= k; bb_piece++) {
        board_copy = piece_bitboards[bb_piece];
        while (board_copy) {
            square = ls1b(board_copy);
            score += material_score[bb_piece];
            switch (bb_piece) {
                case P:
                    score += pawn_scores[square];
                    if (count_bits(piece_bitboards[P] & file_masks[square]) > 1) score += count_bits(piece_bitboards[P] & file_masks[square]) * double_pawn_penalty;
                    if (!(piece_bitboards[P] & isolated_pawn_masks[square])) score += isolated_pawn_penalty;
                    if (!(piece_bitboards[P] & white_passed_pawn_masks[square])) score += passed_pawn_bonus[get_rank[square]];
                    break;
                case N:
                    score += knight_scores[square];
                    break;
                case B:
                    score += bishop_scores[square];
                    score += count_bits(get_bishop_attacks(square, piece_occupancy[both]));
                    break;
                case R:
                    score += rook_scores[square];
                    if (!(piece_bitboards[P] & file_masks[square])) score +=     semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score += open_file_score;
                    break;
                case K:
                    score += king_scores[square];
                    if (!(piece_bitboards[P] & file_masks[square])) score -= semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score -= open_file_score;
                    score += count_bits(king_attack_moves[square] & piece_occupancy[white]) * king_safety_bonus;
                    break;
                case Q:
                    score += count_bits(get_queen_attacks(square, piece_occupancy[both]));
                    break;
                
                case p:
                    score -= pawn_scores[mirror_scores[square]];
                    if (count_bits(piece_bitboards[p] & file_masks[square]) > 1) score -= count_bits(piece_bitboards[P] & file_masks[square]) * double_pawn_penalty;
                    if (!(piece_bitboards[p] & isolated_pawn_masks[square])) score -= isolated_pawn_penalty;
                    if (!(piece_bitboards[p] & black_passed_pawn_masks[square])) score -= passed_pawn_bonus[get_rank[mirror_scores[square]]];
                    break;
                case n:
                    score -= knight_scores[mirror_scores[square]];
                    break;
                case b:
                    score -= bishop_scores[mirror_scores[square]];
                    score -= count_bits(get_bishop_attacks(square, piece_occupancy[both]));
                    break;
                case r:
                    score -= rook_scores[mirror_scores[square]];
                    if (!(piece_bitboards[p] & file_masks[square])) score -= semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score -= open_file_score;
                    break;
                case k:
                    score -= king_scores[mirror_scores[square]];
                    if (!(piece_bitboards[P] & file_masks[square])) score += semi_open_file_score;
                    if (!((piece_bitboards[P] | piece_bitboards[p]) & file_masks[square])) score += open_file_score;
                    score -= count_bits(king_attack_moves[square] & piece_occupancy[black]) * king_safety_bonus;
                    break;
                case q:
                    score -= count_bits(get_queen_attacks(square, piece_occupancy[both]));
                    break;
                default:
                    break;
            }
            pop_bit(board_copy, square);
        }
    }
    if (turn == white) {
        return score;
    } return -score;
}
// half move
