# Monza - A Chess Engine Written in C++ — Estimated Rating: 2000

Monza is in early development and is a built for the purpose of it being a capstone project.

"Monza" is a city that's just northeast of Italy and it is used as a reference to Monza Circuit; one of the highest speed Formula 1 tracks and is known as the temple of speed.

## Currently working on:

- Board Representation
  - [X] Piece Centric Representation using Bitboards
  - [X] Defining macros for Bitboard Operations (SET/GET/POP)
  - [X] Encoding Pieces on the board
  - [X] FEN Parser
- Move Generation
  - [X] pre-calculated pawn attack table.
  - [X] pre-calculated knight attack table.
  - [X] pre-calculated king attack table.
  - [X] Generate Rook attack squares.
  - [X] Generate Bishop attack squares.
  - [X] Limit Rook / Bishop attackes to blocked squares.
  - [X] Generating Rook / Bishop Magic Numbers
  - [X] pre-calculated sliding pieces attack tables (rook/bishop...) - _Magic Bitboards_
- Other
  - [X] Define macros for ls1b and count bits
    - ls1b:
      - Using `__builtin_ctzll` from C++.
    - count_bits
      - Using `__builtin_popcountll`.
      - it seems that the above implementation is faster than using _Brian Kernighan’s Algorithm_ which uses `bitboard &= bitboard - 1;` within a loop to count how many times the bits are eliminated.
    

## Features / Todo of Monza
- Some optimization
- A much better Evaluation mechanism.
- GUI
- Code Cleaning, Folder Structure.

## Resources / Inspiration
### Sources that I looked at extensively
#### _In no particular order_

- Maksim Korzh - BBC (Bitboard Chess Engine)
- Jay Honnold - Berserk Chess Engine
- Andrew Grant - Ethereal Chess Engine
- Pradyumna Kannan - Magic Move-Bitboard Generation in Computer Chess
- Chess Programming Wiki
- Trevor Fenner & Mark Levene - Move Generation with Perfect Hash Functions
- Bluefever Software
- Among a plethora of resources here: https://github.com/mourabitiziyad/Monza-Chess-Engine-Report#7references
