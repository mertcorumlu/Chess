//
// Created by Mert Çorumlu on 20.08.2021.
//

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <immintrin.h>

using namespace std;

void printBits(uint64_t a, bool b);

using U8 = uint8_t;
using U64 = uint64_t;
using U32 = uint32_t;
using U256 = __m256i;

#define SHL(quadboard, index) _mm256_slli_epi64(quadboard, index);

struct Bitboard {

    constexpr static U64 AFile             = 0x0101010101010101;
    constexpr static U64 BFile             = 0x0202020202020202;
    constexpr static U64 GFile             = 0x4040404040404040;
    constexpr static U64 HFile             = 0x8080808080808080;
    constexpr static U64 rank1             = 0x00000000000000FF;
    constexpr static U64 rank8             = 0xFF00000000000000;
    constexpr static U64 a1h8_diagonal     = 0x8040201008040201;
    constexpr static U64 h1a8_antidiagonal = 0x0102040810204080;
    constexpr static U64 light_squares     = 0x55AA55AA55AA55AA;
    constexpr static U64 dark_squares      = 0xAA55AA55AA55AA55;

    constexpr static U8 type_mask  = 0b1110;
    constexpr static U8 color_mask = ~type_mask;

    constexpr static U256 type_arr[13] = {
            {0, 0, 0, 0},
            {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0},
            {0, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1},
            {1, 1, 0, 0}, {1, 0, 1, 0}, {1, 1, 1, 0},
            {1, 0, 0, 1}, {1, 1, 0, 1}, {1, 0, 1, 1}
    };

    enum Direction {
        UP = 8, DOWN = -8,
        UP_RIGHT = 9, RIGHT = 1, DOWN_RIGHT = -7,
        DOWN_LEFT = -9, LEFT = -1, UP_LEFT = 7,
    };

    enum Color {
        White,
        Black
    };

    enum Type {
        None,
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King
    };

    enum Piece {
        Empty   = 0b0000,
        wPawn   = 0b0010,
        wKnight = 0b0100,
        wBishop = 0b0110,
        wRook   = 0b1000,
        wQueen  = 0b1010,
        wKing   = 0b1100,
        bPawn   = 0b0011,
        bKnight = 0b0101,
        bBishop = 0b0111,
        bRook   = 0b1001,
        bQueen  = 0b1011,
        bKing   = 0b1101
    };

    // Bitboard
    U256 quadboard;
    U64 attacking[2];

    struct Proxy {
        Proxy(const Bitboard& board, const U8& x) : board{board}, x{x} {}
        const Bitboard& board; const U8& x;
        Piece operator[](U8 y) const;
    };

    template<Type t>
    U64 fixedAttackingSquares(Color c);

    U64 slidingAttackingSquares(Color c);

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
    Bitboard(string &&fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    void add(Piece p, U8 x, U8 y);
    Piece remove(U8 x, U8 y);
    void move(U8 x, U8 y, U8 _x, U8 _y);

    void updateAttackingSquares(Color c);

    [[nodiscard]] Color colorAt(U8 x, U8 y) const;
    [[nodiscard]] Type typeAt(U8 x, U8 y) const;
    [[nodiscard]] Piece pieceAt(U8 x, U8 y) const;

    static inline Color colorOf(Piece p);
    static inline Type typeOf(Piece p);
    static inline Piece merge(Color, Type);

    Proxy operator[](const U8& x) const;
};


#endif //CHESS_BITBOARD_H
