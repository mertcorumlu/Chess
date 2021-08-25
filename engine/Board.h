//
// Created by Mert Çorumlu on 20.08.2021.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <string>
#include "../types.h"

using namespace std;

#define SHL(quadboard, index) _mm256_slli_epi64(quadboard, index);

class Board {
public:
    Board(string &&fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    void add(Piece::Piece p, U8 x, U8 y);
    Piece::Piece remove(U8 x, U8 y);
    void move(U8 x, U8 y, U8 _x, U8 _y);

    Piece::Color colorAt(U8 x, U8 y) const;
    Piece::Type typeAt(U8 x, U8 y) const;
    Piece::Piece pieceAt(U8 x, U8 y) const;

    static Piece::Color colorOf(Piece::Piece p);
    static Piece::Type typeOf(Piece::Piece p);
    static Piece::Piece merge(Piece::Color, Piece::Type);

private:
    class Proxy {
    public:
        Proxy(const Board& board, const U8& x) : board{board}, x{x} {}
        Piece::Piece operator[](U8 y) const;
    private:
        const Board& board; const U8& x;
    };

public:
    Proxy operator[](const U8& x) const;

    static inline U8 indexOf(U8 x, U8 y);
    static inline U256 getTypeArray(Piece::Piece p);

    U64 getBlack() const;
    U64 getWhite() const;
    U64 getOccupied() const ;
    U64 getOdd() const;
    U64 getSlidingOrthogonal() const;
    U64 getSlidingDiagonal() const;
    U64 getBishops() const;
    U64 getRooks() const;
    U64 getQueens() const;
    U64 getKnights() const;
    U64 getKings() const;
    U64 getPawns() const;

    static void printBoard(uint64_t a);

private:

    // Bitboard
    U256 quadboard;

    constexpr static U8 type_mask  = 0b1110;
    constexpr static U8 color_mask = ~type_mask;

    constexpr static U256 type_arr[13] = {
            {0, 0, 0, 0},
            // Careful! reverse order
            {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0},
            {0, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1},
            {1, 1, 0, 0}, {1, 0, 1, 0}, {1, 1, 1, 0},
            {1, 0, 0, 1}, {1, 1, 0, 1}, {1, 0, 1, 1}
    };

};


#endif //CHESS_BOARD_H
