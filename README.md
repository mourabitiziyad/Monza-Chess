# Monza - A Chess Engine Written in C++

Monza is in early development and is a built for the purpose of it being a capstone project.

"Monza" is a city that's just northeast of Italy and it is used as a reference to Monza Circuit; one of the highest speed Formula 1 tracks and is known as the temple of speed.

Check out Monza on [Lichess](https://lichess.org/@/MonzaEngine)

## Currently working on:

- Board Representation
  - [x] Piece Centric Representation using Bitboards
  - [x] Defining macros for Bitboard Operations (SET/GET/POP)
  - [ ] Encoding Pieces on the board
- Move Generation
  - [X] pre-calculated pawn attack table.
  - [X] pre-calculated knight attack table.
  - [X] pre-calculated king attack table.
  - [X] Generate Rook attack squares.
  - [X] Generate Bishop attack squares.
  - [X] Limit Rook / Bishop attackes to blocked squares.
  - [ ] pre-calculated sliding pieces attack tables (rook/bishop...) - _Magic Bitboards_
- Other
  - [X] Define macros for ls1b and count bits
    - ls1b:
      - Using `__builtin_ctzll` from C++.
    - count_bits
      - Using `__builtin_popcountll`.
      - it seems that the above implementation is faster than using _Brian Kernighan’s Algorithm_ which uses `bitboard &= bitboard - 1;` within a loop to count how many times the bits are eliminated.
    

## Features / Todo of Monza

- WIP
