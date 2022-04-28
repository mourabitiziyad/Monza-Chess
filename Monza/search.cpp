//
//  search.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "search.hpp"

static inline int score_move(int move) {
    if (score_pv) {
        if (pv_table[0][ply] == move) {
            score_pv = 0;
            return 20000;
        }
    }
    if (get_move_capture(move)) {
        int target = P;
        
        int start = (turn == white) ? p : P;
        int end = (turn == white) ? k : K;
        
        for (int bitboard_piece = start; bitboard_piece <= end; bitboard_piece++) {
            if (bit_on_square(piece_bitboards[bitboard_piece], get_move_target(move))) {
                target = bitboard_piece;
                break;
            }
        }
        return mvv_lva[get_move_piece(move)][target] + 10000;
    }
    else {
        if (killer[0][ply] == move) return 9000;
        else if (killer[1][ply] == move) return 8000;
        else return history_moves[get_move_piece(move)][get_move_target(move)];

    }
    return 0;
}

void display_move_score(moves * move_list) {
    printf("Move Scores:\n\n");
    for (int count = 0; count < move_list->count; count++) {
        display_UCI_move(move_list->moves[count]);
        printf(" score: %d\n", score_move(move_list->moves[count]));
    }
}

static inline void enable_pv_scoring(moves * move_list) {
    is_following_pv = 0;
    for (int count = 0; count < move_list->count; count++) {
        if (pv_table[0][ply] == move_list->moves[count]) {
            score_pv = 1;
            is_following_pv = 1;
        }
    }
}


static inline void sort_moves(moves *move_list) {
    int scores[move_list->count];
    for (int cnt = 0; cnt < move_list->count; cnt++) {
        scores[cnt] = score_move(move_list->moves[cnt]);
    }
    for (int current_move = 0; current_move < move_list->count; current_move++)
        {
            // loop over next move within a move list
            for (int next_move = current_move + 1; next_move < move_list->count; next_move++)
            {
                // compare current and next move scores
                if (scores[current_move] < scores[next_move])
                {
                    // swap scores
                    swap(scores[current_move], scores[next_move]);
                    
                    // swap moves
                    swap(move_list->moves[current_move], move_list->moves[next_move]);
                }
            }
        }
}


static inline int quiescence(int alpha, int beta) {
    if((nodes & 2047) == 0) communicate();
    nodes++;
    if (ply > max_ply - 1) return evaluate();
    int evaluation = evaluate();
    
    if (evaluation >= beta) {
        // node (move) fails high
        return beta;
    }
    
    // better move
    if (evaluation > alpha) {
        // pv node (move)
        alpha = evaluation;
    }
    
    moves move_list[1];
    generate_all_moves(move_list);
    sort_moves(move_list);
    for (int count = 0; count < move_list->count; count++) {
        copy_board();
        ply++;
        
        repetition_index++;
        repetitions[repetition_index] = hash_key;

        if (!make_move(move_list->moves[count], captures)) {
            ply--;
            repetition_index--;
            continue;
        }
    
        int score = -quiescence(-beta, -alpha);
        take_back();
        ply--;
        repetition_index--;
        
        if (stopped) return 0;
        
        // fail-hard beta cutoff
        // the score can't be greater than alpha-beta bounds, soft-hard can.
        // better move
        if (score > alpha) {
            // pv node (move)
            alpha = score;
            
            if (score >= beta) {
                // node (move) fails high
                return beta;
            }
        }
    }
    return alpha;
}


int negamax(int alpha, int beta, int depth) {
    int score;
    int flag = hashfALPHA;
    if (ply && is_repetition()) return 0;
//    int pv_node = ((beta - alpha) > 1);
//    if (!pv_node && ply && (score = probe_hash(alpha, beta, depth)) != no_hash_entry) return score;
    if (ply && (score = probe_hash(alpha, beta, depth)) != no_hash_entry) return score;
    if((nodes & 2047) == 0) communicate();
    pv_length[ply] = ply;
    if (depth == 0)
        return quiescence(alpha, beta);
    if (ply > max_ply - 1) {
      return evaluate();
    }
    nodes++;
    int is_king_in_check = is_square_attacked((turn == white ? ls1b(piece_bitboards[K]) : ls1b(piece_bitboards[k])), turn ^ 1);
    if (is_king_in_check) depth++;
    int legal_moves_count = 0;
    if (depth >= 3 && is_king_in_check == 0 && ply) {
        copy_board();
        ply++;
        repetition_index++;
        repetitions[repetition_index] = hash_key;

        if (en_passant != 1) hash_key ^= enpassant_keys[en_passant];
        turn ^= 1;
        hash_key ^= turn_key;
        en_passant = -1;
        score = -negamax(-beta, -beta + 1, depth - 1 - 2);
        take_back();
        ply--;
        repetition_index--;
        if (stopped) return 0;
        if (score >= beta) return beta;
    }
    moves move_list[1];
    generate_all_moves(move_list);
    if (is_following_pv) {
        enable_pv_scoring(move_list);
    }

    sort_moves(move_list);

    int moves_searched = 0;

    for (int count = 0; count < move_list->count; count++) {
        copy_board();
        ply++;
        repetition_index++;
        repetitions[repetition_index] = hash_key;

        if (!make_move(move_list->moves[count], all)) {
            ply--;
            repetition_index--;
            continue;
        }

        legal_moves_count++;

        if (!moves_searched)
            score = -negamax(-beta, -alpha, depth - 1);
        else {
            if (moves_searched >= full_depth_moves && depth >= reduction_limit && !is_king_in_check && !get_move_capture(move_list->moves[count]) && !get_move_promoted(move_list->moves[count]))
                score = -negamax(-alpha - 1, -alpha, depth - 2);
            else score = alpha + 1;

            if (score > alpha) {
                score = -negamax(-alpha - 1, -alpha, depth-1);
                if((score > alpha) && (score < beta))
                    score = -negamax(-beta, -alpha, depth-1);
            }
        }

        take_back();
        ply--;
        repetition_index--;

        if(stopped) return 0;
        moves_searched++;


        // better move
        if (score > alpha) {
            flag = hashfEXACT;
            if (!get_move_capture(move_list->moves[count])) {
                history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;
            }

            // pv node (move)

            alpha = score;

            pv_table[ply][ply] = move_list->moves[count];
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++) {
              pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            }
            pv_length[ply] = pv_length[ply + 1];

            // fail-hard beta cutoff
            // the score can't be greater than alpha-beta bounds, soft-hard can.
            if (score >= beta) {
                record_hash(depth, beta, hashfBETA);
                // node (move) fails high
                if (!get_move_capture(move_list->moves[count])) {
                    killer[1][ply] = killer[0][ply];
                    killer[0][ply] = move_list->moves[count];
                }
                return beta;
            }
        }
    }
    if (!legal_moves_count) {
        if (is_king_in_check)
            return -mate_val + ply;
        return 0;
    }

    record_hash(depth, alpha, flag);
    return alpha;
}

void search_position(int depth) {

    nodes = 0;
    int score = 0;
    is_following_pv = 0;
    score_pv = 0;
    stopped = 0;

    memset(killer, 0, sizeof(killer));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));

    int alpha = -Infinity;
    int beta = Infinity;

    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        if (stopped) break;
        is_following_pv = 1;

        score = negamax(alpha, beta, current_depth);

//        if ((score <= alpha) || (score >= beta)) {
//            alpha = -Infinity;
//            beta = Infinity;
//            current_depth--;
//            continue;
//        }
//
//        alpha = score - 50;
//        beta = score + 50;
        
        if (pv_length[0]) {
            if (score > -mate_val && score < -mate_score)
                printf("info score mate %d depth %d nodes %ld time %d pv ", -(score + mate_val) / 2 - 1, current_depth, nodes, get_time_ms() - starttime);
            else if (score > mate_score && score < mate_val)
                printf("info score mate %d depth %d nodes %ld time %d pv ", (mate_val - score) / 2 + 1, current_depth, nodes, get_time_ms() - starttime);
            else
                printf("info score cp %d depth %d nodes %ld time %d pv ", score, current_depth, nodes, get_time_ms() - starttime);

            for (int count = 0; count < pv_length[0]; count++) {
                display_UCI_move(pv_table[0][count]);
                printf(" ");
            }
            printf("\n");
        }
    }

    printf("bestmove ");
    display_UCI_move(pv_table[0][0]);
    printf("\n");

}
