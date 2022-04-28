//
//  move.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "move.hpp"

void add_move(moves *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

int make_move (int move, int move_flag) {
    if (move_flag == all) {
        // preserve the current *legal* board state in case of making an illegal move
        // we are also providing the option to generate only captures to mitigate the "Horizon Effect"
        // "The horizon effect can be mitigated by extending the search algorithm with a quiescence search. This gives the search algorithm ability to look beyond its horizon for a certain class of moves of major importance to the game state, such as captures in chess."
        copy_board();
        
        int source = get_move_source(move);
        int target = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int doublepush = get_move_double(move);
        int enpassant = get_move_enpassant(move);
        int castling = get_move_castling(move);
        
        pop_bit(piece_bitboards[piece], source);
        set_bit(piece_bitboards[piece], target);
        
        // hashing
        hash_key ^= piece_keys[piece][source];
        hash_key ^= piece_keys[piece][target];
        
        if (capture) {

            int start = (turn == white) ? p : P;
            int end = (turn == white) ? k : K;
            
            for (int bitboard_piece = start; bitboard_piece <= end; bitboard_piece++) {
                if (bit_on_square(piece_bitboards[bitboard_piece], target)) {
                    pop_bit(piece_bitboards[bitboard_piece], target);
                    hash_key ^= piece_keys[bitboard_piece][target];
                    break;
                }
            }
        }
        
        if (promoted) {
            if (turn == white) {
                pop_bit(piece_bitboards[P], target);
                hash_key ^= piece_keys[P][target];
            } else {
                pop_bit(piece_bitboards[p], target);
                hash_key ^= piece_keys[p][target];
            }
            set_bit(piece_bitboards[promoted], target);
            hash_key ^= piece_keys[promoted][target];
        }
        
        if (enpassant) {
            if (turn == white) {
                pop_bit(piece_bitboards[p], target + 8);
                hash_key ^= piece_keys[p][target + 8];
            } else {
                pop_bit(piece_bitboards[P], target - 8);
                hash_key ^= piece_keys[P][target - 8];
            }
        }
        
        if (en_passant != -1) hash_key ^= enpassant_keys[en_passant];
        
        en_passant = -1;
        
        if (doublepush) {
            if (turn == white) {
                en_passant = target + 8;
                hash_key ^= enpassant_keys[target + 8];
            } else {
                en_passant = target - 8;
                hash_key ^= enpassant_keys[target - 8];
            }
        }
        
        if (castling) {
            switch (target) {
                case g1:
                    pop_bit(piece_bitboards[R], h1);
                    set_bit(piece_bitboards[R], f1);
                    hash_key ^= piece_keys[R][h1];
                    hash_key ^= piece_keys[R][f1];
                    break;
                case c1:
                    pop_bit(piece_bitboards[R], a1);
                    set_bit(piece_bitboards[R], d1);
                    hash_key ^= piece_keys[R][a1];
                    hash_key ^= piece_keys[R][d1];
                    break;
                case g8:
                    pop_bit(piece_bitboards[r], h8);
                    set_bit(piece_bitboards[r], f8);
                    hash_key ^= piece_keys[r][h8];
                    hash_key ^= piece_keys[r][f8];
                    break;
                case c8:
                    pop_bit(piece_bitboards[r], a8);
                    set_bit(piece_bitboards[r], d8);
                    hash_key ^= piece_keys[r][a8];
                    hash_key ^= piece_keys[r][d8];
                    break;
                default:
                    break;
            }
        }
        
        hash_key ^= castling_keys[castling_rights];
        
        castling_rights &= (castling_rights_squares[source] & castling_rights_squares[target]);
        
        hash_key ^= castling_keys[castling_rights];
        
        memset(piece_occupancy, 0ULL, sizeof(piece_occupancy));
                
        for (int piece = P; piece <= K; piece++)
            piece_occupancy[white] |= piece_bitboards[piece];
        
        for (int piece = p; piece <= k; piece++)
            piece_occupancy[black] |= piece_bitboards[piece];

        piece_occupancy[both] |= piece_occupancy[white] | piece_occupancy[black];
        
        turn ^= 1;
        
        hash_key ^= turn_key;
        
        if (is_square_attacked((turn == white) ? ls1b(piece_bitboards[k]) : ls1b(piece_bitboards[K]), turn)) {
            take_back();
            return 0;
        }
        return 1;
        
    } else
        if (get_move_capture(move))
            make_move(move, all);
        return 0;
}

int is_square_attacked(int square, int side) {
    if (side == white) {
        return ((pawn_attack_moves[black][square] & piece_bitboards[P])
        | (knight_attack_moves[square] & piece_bitboards[N])
        | (king_attack_moves[square] & piece_bitboards[K])
        | (get_bishop_attacks(square, piece_occupancy[both]) & piece_bitboards[B])
        | (get_rook_attacks(square, piece_occupancy[both]) & piece_bitboards[R])
        | (get_queen_attacks(square, piece_occupancy[both]) & piece_bitboards[Q])) ? 1 : 0;
    } else if (side == black){
        return ((pawn_attack_moves[white][square] & piece_bitboards[p])
        | (knight_attack_moves[square] & piece_bitboards[n])
        | (king_attack_moves[square] & piece_bitboards[k])
        | (get_bishop_attacks(square, piece_occupancy[both]) & piece_bitboards[b])
        | (get_rook_attacks(square, piece_occupancy[both]) & piece_bitboards[r])
        | (get_queen_attacks(square, piece_occupancy[both]) & piece_bitboards[q])) ? 1 : 0;
    }
    return 0;
}


void generate_all_moves(moves *move_list) {
    int source, target;
    Bitboard bitboard_copy, attacks;
    for (int piece = P; piece <= k; piece++) {
        bitboard_copy = piece_bitboards[piece];
        if (turn == white) { // white (defining the side) !turn
            // TODO white pawns
            if (piece == P) {
                while (bitboard_copy) {
                    source = ls1b(bitboard_copy);
                    target = source - 8;
                    if (!(target < a8) && !bit_on_square(piece_occupancy[both], target)) {
                        if (source >= a7 && source <= h7) {
                            add_move(move_list, encode_move(source, target, piece, Q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, R, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, B, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, N, 0, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                            if ((source >= a2 && source <= h2)
                                && !bit_on_square(piece_occupancy[both], (target - 8))) {
                                add_move(move_list, encode_move(source, (target - 8), piece, 0, 0, 1, 0, 0));
                            }
                        }
                        // pawn push
                        // double pawn push
                    }
                    
                    attacks = pawn_attack_moves[turn][source] & piece_occupancy[black];
                    
                    while (attacks) {
                        target = ls1b(attacks);
                        
                        if (source >= a7 && source <= h7) {
                            add_move(move_list, encode_move(source, target, piece, Q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, R, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, B, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, N, 1, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                        }
                        pop_bit(attacks, target);
                    }
                    
                    if (en_passant != -1) {
                        Bitboard en_passant_capture = pawn_attack_moves[turn][source] & (1ULL << en_passant);
                        if (en_passant_capture) {
                            int target_en_passant = ls1b(en_passant_capture);
                            add_move(move_list, encode_move(source, target_en_passant, piece, 0, 1, 0, 1, 0));
                        }
                    }
                    
                    pop_bit(bitboard_copy, source);
                }
            }
            if (piece == K) {
                
                if (castling_rights & wk) {
                    if (!bit_on_square(piece_occupancy[both], f1) && !bit_on_square(piece_occupancy[both], g1)) {
                        if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black)) {
                            add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
                
                if (castling_rights & wq) {
                    if (!bit_on_square(piece_occupancy[both], d1) && !bit_on_square(piece_occupancy[both], c1) && !bit_on_square(piece_occupancy[both], b1)) {
                        if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black)) {
                            add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }
        } else if (turn == black) {
            // TODO black pawns
            if (piece == p) {
                while (bitboard_copy) {
                    source = ls1b(bitboard_copy);
                    target = source + 8;
                    if (!(target > h1) && !bit_on_square(piece_occupancy[both], target)) {
                        if (source >= a2 && source <= h2) {
                            add_move(move_list, encode_move(source, target, piece, q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, r, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, b, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, n, 0, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                            if ((source >= a7 && source <= h7)
                                && !bit_on_square(piece_occupancy[both], (target + 8))) {
                                add_move(move_list, encode_move(source, (target + 8), piece, 0, 0, 1, 0, 0));
                            }

                        }
                        // pawn push
                        // double pawn push
                    }
                    
                    attacks = pawn_attack_moves[turn][source] & piece_occupancy[white];
                    
                    while (attacks) {
                        target = ls1b(attacks);
                        
                        if (source >= a2 && source <= h2) {
                            add_move(move_list, encode_move(source, target, piece, q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, r, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, b, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source, target, piece, n, 1, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                        }
                        pop_bit(attacks, target);
                    }
                    
                    if (en_passant != -1) {
                        Bitboard en_passant_capture = pawn_attack_moves[turn][source] & (1ULL << en_passant);
                        if (en_passant_capture) {
                            int target_en_passant = ls1b(en_passant_capture);
                            add_move(move_list, encode_move(source, target_en_passant, piece, 0, 1, 0, 1, 0));
                        }
                    }
                    
                    pop_bit(bitboard_copy, source);
                }
            }
            
            if (piece == k) {
                
                if (castling_rights & bk) {
                    if (!bit_on_square(piece_occupancy[both], f8) && !bit_on_square(piece_occupancy[both], g8)) {
                        if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white))
                            add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                    }
                }
                
                if (castling_rights & bq) {
                    if (!bit_on_square(piece_occupancy[both], d8) && !bit_on_square(piece_occupancy[both], c8) && !bit_on_square(piece_occupancy[both], b8)) {
                        if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white))
                            add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                    }
                }
            }
        }
        
        // knight
        if ((turn == white) ? piece == N : piece == n) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = knight_attack_moves[source] & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));

                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // bishop
        if ((turn == white) ? piece == B : piece == b) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = get_bishop_attacks(source, piece_occupancy[both]) & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));

                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // rooks
        if ((turn == white) ? piece == R : piece == r) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = get_rook_attacks(source, piece_occupancy[both]) & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // queens
        if ((turn == white) ? piece == Q : piece == q) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = get_queen_attacks(source, piece_occupancy[both]) & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
        // kings
        if ((turn == white) ? piece == K : piece == k) {
            while (bitboard_copy) {
                source = ls1b(bitboard_copy);
                attacks = king_attack_moves[source] & ~piece_occupancy[turn];
                while (attacks) {
                    target = ls1b(attacks);
                    if (!bit_on_square(piece_occupancy[!turn], target))
                        add_move(move_list, encode_move(source, target, piece, 0, 0, 0, 0, 0));
                    else
                        add_move(move_list, encode_move(source, target, piece, 0, 1, 0, 0, 0));
                    pop_bit(attacks, target);
                }
                pop_bit(bitboard_copy, source);
            }
            
        }
        
    }
}

void print_attacked_squares(int side)
{
    printf("\n");
    
    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // print ranks
            if (!file)
                printf("  %d ", 8 - rank);
            
            // check whether current square is attacked or not
            printf(" %d", is_square_attacked(square, side) ? 1 : 0);
        }
        
        // print new line every rank
        printf("\n");
    }
    
    // print files
    printf("\n     a b c d e f g h\n\n");
}

void display_move_list(moves *move_list) {
    printf("\nMove       Piece       isCapture?      isDoublePush?       Enpassant?        Castling?\n\n");
    for (int count = 0; count < move_list->count; count++) {
        printf("%s%s%c      %s           %d               %d                   %d                 %d\n",
                            squares[get_move_source(move_list->moves[count])],
                            squares[get_move_target(move_list->moves[count])],
                            get_move_promoted(move_list->moves[count]) ?
//               promoted_pieces[get_move_promoted(move_list->moves[count])]
               find_promoted_piece_index(get_move_promoted(move_list->moves[count]))
               : ' ',
                            unicode_pieces[get_move_piece(move_list->moves[count])],
                            get_move_capture(move_list->moves[count]) ? 1 : 0,
                            get_move_double(move_list->moves[count]) ? 1 : 0,
                            get_move_enpassant(move_list->moves[count]) ? 1 : 0,
                            get_move_castling(move_list->moves[count]) ? 1 : 0);

    }
    printf("\nNumber of Moves: %d\n\n", move_list->count);
}
