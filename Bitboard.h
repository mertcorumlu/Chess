//
// Created by Mert Çorumlu on 20.08.2021.
//

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include <cstdint>
#include <cstddef>
#include <string>

using namespace std;

void printBits(uint32_t a, bool b);

struct Bitboard {

    using U64 = uint64_t;
    using U32 = uint32_t;

    const static uint8_t type_mask = 0x7;
    const static uint8_t color_mask = 0x8;

    enum Direction {
        UP_LEFT = 7,
        UP = 8,
        UP_RIGHT = 9,
        RIGHT = 1,
        DOWN_RIGHT = -7,
        DOWN = -8,
        DOWN_LEFT = -9,
        LEFT = -1,
    };

    enum Color {
        White,
        Black
    };

    enum Type {
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King,
        None
    };

    enum Piece {
        Empty,
        wPawn,
        wKnight,
        wBishop,
        wRook,
        wQueen,
        wKing,
        bPawn = 9,
        bKnight = 10,
        bBishop = 11,
        bRook = 12,
        bQueen = 13,
        bKing = 14
    };

    // Bitboard
    U64 bitboards[6];
    U64 occupies[2];
    U64 attacking[2];

    //CCR [y][x]
    U32 crc[8]; // as vertical lines

    struct Wrapper {
        Wrapper(U32 s) : s{s} {}

        U32 s;

        Piece operator[](size_t x) const;
    };

    template<Type t>
    U64 fixedAttackingSquares(Color c);

    template<Direction d, Type t>
    U64 slidingAttackingSquares(Color c);

    static inline void setBitAt(U64 &s, size_t i);

    static inline void removeBitAt(U64 &s, size_t i);

    static inline void setCrc(U32 &s, Piece p, size_t x);

    static inline size_t indexOf(size_t x, size_t y);

public:
    Bitboard(string &&fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    void add(Piece p, size_t x, size_t y);

    void remove(size_t x, size_t y);

    void move(size_t x, size_t y, size_t _x, size_t _y);

    void updateAttackingSquares(Color c);

    Color colorAt(size_t x, size_t y) const;

    Type typeAt(size_t x, size_t y) const;

    Piece pieceAt(size_t x, size_t y) const;

    static inline Color colorOf(Piece p);

    static inline Type typeOf(Piece p);

    static inline Piece merge(Color, Type);

    Wrapper operator[](size_t y) const;
};


#endif //CHESS_BITBOARD_H
