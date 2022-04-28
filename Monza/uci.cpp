//
//  uci.cpp
//  Monza
//
//  Created by Ziyad Mourabiti on 28/4/2022.
//

#include "uci.hpp"

void parse_fen(char * fen_string) {

    memset(piece_bitboards, 0ULL, sizeof(piece_bitboards));
    memset(piece_occupancy, 0ULL, sizeof(piece_occupancy));
    
    castling_rights = 0;
    turn = -1;
    en_passant = -1;
    
    repetition_index = 0;
    memset(repetitions, 0, sizeof(repetitions));
    
    for (int square = 0; square < 64 && *fen_string && *fen_string!=' ';) {
        if (isalpha(*fen_string)) {
            int piece_index = find_piece_index(*fen_string++);
            set_bit(piece_bitboards[piece_index], square++);
        }
        else if (isdigit(*fen_string)) square += (*fen_string++ - '0');
        else if (*fen_string == '/') fen_string++;
        else memset(piece_bitboards, 0ULL, sizeof(piece_bitboards));
    }
    *(++fen_string) == 'b' ? (turn = black) : (turn = white);
    (fen_string += 2);
    while (*fen_string != ' ') {
        switch (*fen_string++) {
            case 'K': castling_rights |= wk; break;
            case 'Q': castling_rights |= wq; break;
            case 'k': castling_rights |= bk; break;
            case 'q': castling_rights |= bq; break;
            case '-': break;
            default: break;
        }
    }
    if (*(++fen_string) != '-') {
        int file = *fen_string++ - 'a';
        int rank = 8 - (*fen_string - '0');
        en_passant = rank * 8 + file;
    } else {
        en_passant = -1;
    }
    
    for (int piece = P; piece <= K; piece++) {
        piece_occupancy[white] |= piece_bitboards[piece];
    }
    
    for (int piece = p; piece <= k; piece++) {
        piece_occupancy[black] |= piece_bitboards[piece];
    }
    
    piece_occupancy[both] |= (piece_occupancy[white] | piece_occupancy[black]);
    
    hash_key = generate_hash();
}

void display_UCI_move(int move) {
    if (get_move_promoted(move)) {
        printf("%s%s%c",  squares[get_move_source(move)],
                            squares[get_move_target(move)],
//                            promoted_pieces[get_move_promoted(move)]);
               find_promoted_piece_index(get_move_promoted(move)));
    } else {
        printf("%s%s",  squares[get_move_source(move)],
                            squares[get_move_target(move)]);
    }
}

int parse_move(char *move_string) {
    moves move_list[1];
    generate_all_moves(move_list);
    int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
    int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        int move = move_list->moves[move_count];
        if (source_square == get_move_source(move) && target_square == get_move_target(move))
        {
            int promoted_piece = get_move_promoted(move);
            if (promoted_piece)
            {
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q')
                    return move;
                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r')
                    return move;
                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b')
                    return move;
                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n')
                    return move;
                continue;
            }
            
            // return legal move
            return move;
        }
    }
    // return illegal move
    return 0;
}

void parse_position(char *command) // taken from BBC.c (Code Monkey King)
{
    // shift pointer to the right where next token begins
    command += 9;
    
    // init pointer to the current character in the command string
    char *current_char = command;
    
    // parse UCI "startpos" command
    if (!strncmp(command, "startpos", 8))
        // init chess board with start position
        parse_fen((char *) starting_position);
    
    // parse UCI "fen" command
    else
    {
        // make sure "fen" command is available within command string
        current_char = strstr(command, "fen");
        
        // if no "fen" command is available within command string
        if (current_char == NULL)
            // init chess board with start position
            parse_fen((char *)starting_position);
            
        // found "fen" substring
        else
        {
            // shift pointer to the right where next token begins
            current_char += 4;
            
            // init chess board with position from FEN string
            parse_fen(current_char);
        }
    }
    
    // parse moves after position
    current_char = strstr(command, "moves");
    
    // moves available
    if (current_char != NULL)
    {
        // shift pointer to the right where next token begins
        current_char += 6;
        
        // loop over moves within a move string
        while(*current_char)
        {
            // parse next move
            int move = parse_move(current_char);
            
            // if no more moves
            if (move == 0)
                // break out of the loop
                break;
            
            repetition_index++;
            repetitions[repetition_index] = hash_key;
            
            // make move on the chess board
            make_move(move, all);
            
            // move current character mointer to the end of current move
            while (*current_char && *current_char != ' ') current_char++;
            
            // go to the next move
            current_char++;
        }
    }
    styled_board();
}

//void parse_go(char * command) {
//    int depth = -1;
//    char * current_depth = NULL;
//    current_depth = strstr(command, "depth");
//    if (current_depth) {
//        depth = atoi(current_depth + 6);
//    } else depth = 6;
//
//    search_position(depth);
//    // call search pos given depth
//}

void parse_go(char *command) {
    reset_time();
    int depth = -1;
    char *argument = NULL;
    if ((argument = strstr(command,"infinite"))) {}
    if ((argument = strstr(command,"binc")) && turn == black)
        inc = atoi(argument + 5);
    if ((argument = strstr(command,"winc")) && turn == white)
        inc = atoi(argument + 5);
    if ((argument = strstr(command,"wtime")) && turn == white)
        time_ = atoi(argument + 6);
    if ((argument = strstr(command,"btime")) && turn == black)
        time_ = atoi(argument + 6);
    if ((argument = strstr(command,"movestogo")))
        movestogo = atoi(argument + 10);
    if ((argument = strstr(command,"movetime")))
        movetime = atoi(argument + 9);
    if ((argument = strstr(command,"depth")))
        depth = atoi(argument + 6);
    if(movetime != -1) {
        time_ = movetime;
        movestogo = 1;
    }
    starttime = get_time_ms();
    depth = depth;
    if(time_ != -1) {
        timeset = 1;
        time_ /= movestogo;
        if (time_ > 1500) time_ -= 50;
        stoptime = starttime + time_ + inc;
    }
    
    if (depth == -1) depth = 64;
    printf("time: %d start: %u stop: %u depth: %d timeset: %d\n",
    time_, starttime, stoptime, depth, timeset);

    // search position
    search_position(depth);
}

void UCI_loop() {
    
//    int max_hash = 128;
//    int mb = 64;
    
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    
    char input[3000];
    
    printf("id name Monza\n");
    printf("id author Ziyad Mourabiti\n");
//    printf("option name Hash type spin default 64 min 6 max %d\n", max_hash);
    printf("uciok\n"); // chess engine speaks UCI
    
    while (1) {
        memset(input, 0, sizeof(input));
        fflush(stdout);
        if (!fgets(input, 3000, stdin) || input[0] == '\n') continue;
        
        else
            if (!strncmp(input, "isready", 7)) {
            cout << "readyok" << endl;
            continue;
        }
        else if (!strncmp(input, "position", 8)) {
            parse_position(input);
        }
        else if (!strncmp(input, "ucinewgame", 10)) {
            parse_position((char *)"position startpos");
            clear_t_table();
        }
        else if (!strncmp(input, "go", 2)) {
            parse_go(input);
        }
        else if (!strncmp(input, "quit", 4)) break;
        else if (!strncmp(input, "uci", 3)) {
            printf("id name Monza\n");
            printf("id author Ziyad Mourabiti\n");
            printf("uciok\n"); // chess engine speaks UCI
        }
//        else if (!strncmp(input, "setoption name Hash value ", 26)) {
//            sscanf(input,"%*s %*s %*s %*s %d", &mb);
//            if (mb < 4) mb = 4;
//            if (mb > max_hash) mb = max_hash;
//            printf("    Set hash table size to %dMB\n", mb);
//            init_t_table(mb);
//        }
    }
}
