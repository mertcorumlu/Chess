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

namespace Piece {

    enum Color : bool {
        White, Black
    };

    enum Type : U8 {
        Pawn = 1 << 1, Knight = 2 << 1,
        Bishop = 3 << 1, Rook = 4 << 1,
        Queen = 5 << 1, King = 6 << 1
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
        None,
        Null = 65
    };

    enum Type : U8 {
        Normal,
        Promotion,
        En_Passant,
        Castling
    };
}

inline Move::Move make_move(U8 from, U8 to, Move::Type type){
    return Move::Move(from | (to << 6) | (type << 12));
}

enum Castling{
    NO_CASTLING,

    WHITE_Q,
    WHITE_K = WHITE_Q << 1,
    BLACK_Q = WHITE_K << 1,
    BLACK_K = BLACK_Q << 1,

    QUEEN = WHITE_Q | BLACK_Q,
    KING  = WHITE_K | BLACK_K,
    WHITE = WHITE_Q | WHITE_K,
    BLACK = BLACK_Q | BLACK_K,

    ANY = QUEEN | KING
};

using MoveList = std::vector<Move::Move>;

inline U8 lsb(U64 a) {
    return std::__libcpp_ctz(a);
}

inline U8 pop_lsb(U64& a) {
    U8 tmp = lsb(a);
    a &= a - 1;
    return tmp;
}

#endif //CHESS_TYPES_H
