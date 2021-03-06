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

    void add(Piece::Piece p, Square square);
    Piece::Piece remove(Square square);
    void move(Square from, Square to);

    Piece::Piece pieceAt(Square square) const;

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

    static Square squareOf(U8 x, U8 y);
    static U256 getTypeArray(Piece::Piece p);

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

private:

    // Bitboard
    U256 quadboard;

    constexpr static U256 type_arr[13] = {
            {0, 0, 0, 0},
            // Careful! reverse order
            {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0},
            {0, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1},
            {1, 1, 0, 0}, {1, 0, 1, 0}, {1, 1, 1, 0},
            {1, 0, 0, 1}, {1, 1, 0, 1}, {1, 0, 1, 1}
    };

};

std::ostream& operator<<(std::ostream &strm, const Board &board);
std::ostream& operator<<(std::ostream& strm, const Piece::Piece& piece);
std::ostream& operator<<(std::ostream& strm, const Square& square);
std::ostream& operator<<(std::ostream& strm, const Castling& castling);
std::ostream& operator<<(std::ostream& strm, const Move::Move& move);



#endif //CHESS_BOARD_H
