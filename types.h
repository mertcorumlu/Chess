//
// Created by Mert Çorumlu on 23.08.2021.
//

#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include <immintrin.h>
#include <vector>
#include <bit>
#include <iostream>

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
    NONE = -1,
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

namespace Piece {

    enum Color : bool {
        WHITE, BLACK
    };

    enum Type : U8 {
        NONE,
        PAWN, KNIGHT,
        BISHOP, ROOK,
        QUEEN, KING
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

inline void bin(U64 a) {
    for (int i = 0; i < 8; ++i) {
        uint8_t s = (a & 0xff00000000000000) >> 56;
        std::cout << 8 - i << "  ";
        for (int j = 0; j < 8; ++j) {
            std::cout << ((s & 0x1) ? '1' : '.') << "  ";
            s >>= 1;
        }
        std::cout << std::endl;
        a <<= 8;
    }

    std::cout << "   a  b  c  d  e  f  g  h" << std::endl;
    std::cout << std::endl;
}

//inline void bin(U32 s) {
//    for (int i = 0; i < 32; ++i) {
//        std::cout << ((s >> (31 - i)) & 1);
//    }
//}

#endif //CHESS_TYPES_H
