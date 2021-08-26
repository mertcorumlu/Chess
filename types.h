//
// Created by Mert Çorumlu on 23.08.2021.
//

#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include <immintrin.h>
#include <vector>
#include <bit>

using U8 = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;

using U256 = __m256i;


constexpr static U64 AFile             = 0x0101010101010101;
constexpr static U64 BFile             = 0x0202020202020202;
constexpr static U64 GFile             = 0x4040404040404040;
constexpr static U64 HFile             = 0x8080808080808080;
constexpr static U64 rank1             = 0x00000000000000FF;
constexpr static U64 rank8             = 0xFF00000000000000;
constexpr static U64 a1h8_diagonal     = 0x8040201008040201;
constexpr static U64 h1a8_antidiagonal = 0x0102040810204080;

enum Square {
    A1, A2, A3, A4, A5, A6, A7, A8,
    B1, B2, B3, B4, B5, B6, B7, B8,
    C1, C2, C3, C4, C5, C6, C7, C8,
    D1, D2, D3, D4, D5, D6, D7, D8,
    E1, E2, E3, E4, E5, E6, E7, E8,
    F1, F2, F3, F4, F5, F6, F7, F8,
    G1, G2, G3, G4, G5, G6, G7, G8,
    H1, H2, H3, H4, H5, H6, H7, H8
};

namespace Piece {

    enum Color : bool {
        WHITE, BLACK
    };

    enum Type : U8 {
        PAWN = 1 << 1, KNIGHT = 2 << 1,
        BISHOP = 3 << 1, ROOK = 4 << 1,
        QUEEN = 5 << 1, KING = 6 << 1
    };

    enum Piece : U8 {
        Empty = 0b0000,
        wPawn = 0b0010, wKnight = 0b0100, wBishop = 0b0110, wRook = 0b1000, wQueen = 0b1010, wKing = 0b1100,
        bPawn = 0b0011, bKnight = 0b0101, bBishop = 0b0111, bRook = 0b1001, bQueen = 0b1011, bKing = 0b1101
    };

}

namespace Move {
    /*
   * bits[15-12] : Type
   * bits[11-6]  : To
   * bits[5-0]  : From
   *
   */
    enum Move : U16 {
        M_NONE,
        M_NULL = 65
    };

    enum Type : U8 {
        NORMAL,
        PROMOTION,
        EN_PASSANT,
        CASTLING
    };
}

inline Move::Move make_move(Square from, Square to, Move::Type type){
    return Move::Move(from | (to << 6) | (type << 12));
}

enum Castling{
    NO_CASTLING,

    WHITE_Q, // 1
    WHITE_K = WHITE_Q << 1, // 2
    BLACK_Q = WHITE_K << 1, // 4
    BLACK_K = BLACK_Q << 1, // 8

    QUEEN = WHITE_Q | BLACK_Q,
    KING  = WHITE_K | BLACK_K,
    WHITE = WHITE_Q | WHITE_K,
    BLACK = BLACK_Q | BLACK_K,

    ANY = QUEEN | KING
};

using MoveList = std::vector<Move::Move>;

inline Square lsb(U64 a) {
    return Square(std::__libcpp_ctz(a));
}

inline Square pop_lsb(U64& a) {
    Square tmp = lsb(a);
    a &= a - 1;
    return tmp;
}

enum Direction {
    UP = 8,
    DOWN = -UP,
    RIGHT = 1,
    LEFT = -1
};

inline U64 shift(U64 b, Direction d) {
    if (d > 0)
        return b << d;
    else
        return b >> -d;
}

#endif //CHESS_TYPES_H