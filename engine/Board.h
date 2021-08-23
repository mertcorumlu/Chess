//
// Created by Mert Çorumlu on 20.08.2021.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <string>
#include <immintrin.h>
#include "Attack.h"


using namespace std;

#define SHL(quadboard, index) _mm256_slli_epi64(quadboard, index);

using U256 = __m256i;

struct Board {

    constexpr static U8 type_mask  = 0b1110;
    constexpr static U8 color_mask = ~type_mask;

    constexpr static U256 type_arr[13] = {
            {0, 0, 0, 0},
            {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0},
            {0, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1},
            {1, 1, 0, 0}, {1, 0, 1, 0}, {1, 1, 1, 0},
            {1, 0, 0, 1}, {1, 1, 0, 1}, {1, 0, 1, 1}
    };

    enum Color {
        White, Black
    };

    enum Type {
        None,
        Pawn, Knight,
        Bishop,Rook,
        Queen, King
    };

    enum Piece {
        Empty   = 0b0000,
        wPawn   = 0b0010, wKnight = 0b0100, wBishop = 0b0110, wRook   = 0b1000, wQueen  = 0b1010, wKing   = 0b1100,
        bPawn   = 0b0011, bKnight = 0b0101, bBishop = 0b0111, bRook   = 0b1001, bQueen  = 0b1011, bKing   = 0b1101
    };

    // Bitboard
    U256 quadboard;

    struct Proxy {
        Proxy(const Board& board, const U8& x) : board{board}, x{x} {}
        const Board& board; const U8& x;
        Piece operator[](U8 y) const;
    };

    static inline U8 indexOf(U8 x, U8 y);
    static inline U256 getTypeArray(Piece p);

    inline U64 getBlack();
    inline U64 getWhite();
    inline U64 getOccupied();
    inline U64 getOdd();
    inline U64 getSlidingOrthogonal();
    inline U64 getSlidingDiagonal();
    inline U64 getBishops();
    inline U64 getRooks();
    inline U64 getQueens();
    inline U64 getKnights();
    inline U64 getKings();
    inline U64 getPawns();

public:
    Board(string &&fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    void add(Piece p, U8 x, U8 y);
    Piece remove(U8 x, U8 y);
    void move(U8 x, U8 y, U8 _x, U8 _y);

    [[nodiscard]] Color colorAt(U8 x, U8 y) const;
    [[nodiscard]] Type typeAt(U8 x, U8 y) const;
    [[nodiscard]] Piece pieceAt(U8 x, U8 y) const;

    static Color colorOf(Piece p);
    static Type typeOf(Piece p);
    static Piece merge(Color, Type);

    Proxy operator[](const U8& x) const;

    static void printBoard(uint64_t a);
};


#endif //CHESS_BOARD_H
