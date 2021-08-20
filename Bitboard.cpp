//
// Created by Mert Çorumlu on 20.08.2021.
//

#include "Bitboard.h"
#include <iostream>

Bitboard::Bitboard(string &&fen) : bitboards(), occupies(), attacking(), crc() {

    int x = 0, y = 7;

    for(auto& c : fen) {
        auto color = Color(islower(c) ? 1 : 0);
        switch (c) {
            case 'p': case 'P': add(merge(color, Pawn), x++, y); break;
            case 'r': case 'R': add(merge(color, Rook), x++, y); break;
            case 'n': case 'N': add(merge(color, Knight), x++, y); break;
            case 'b': case 'B': add(merge(color, Bishop), x++, y); break;
            case 'q': case 'Q': add(merge(color, Queen), x++, y); break;
            case 'k': case 'K': add(merge(color, King), x++, y); break;

            case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
                for(int i = 0; i < c - '0'; ++i) add(Empty, x++, y);
                break;

            case '/':
                --y;
                x = 0;
                break;

            default: continue;
        }
    }

}

void Bitboard::add(Bitboard::Piece p, size_t x, size_t y) {
    size_t index = indexOf(x, y);

    if (p != Empty) {
        setBitAt(bitboards[typeOf(p)], index);
        setBitAt(occupies[colorOf(p)], index);
    } else{
        removeBitAt(bitboards[typeOf(p)], index);
        removeBitAt(occupies[colorOf(p)], index);
    }

    setCrc(crc[y], p, x);
}

void Bitboard::remove(size_t x, size_t y) {
    Piece p = pieceAt(x, y);
    size_t index = indexOf(x, y);
    removeBitAt(bitboards[typeOf(p)], index);
    removeBitAt(occupies[colorOf(p)], index);
    setCrc(crc[y], Piece::Empty, x);
}

void Bitboard::move(size_t x, size_t y, size_t _x, size_t _y) {
    Piece p = pieceAt(x, y);
    if (p != Empty) {
        // set bits on (x,y), (_x, _y) in a 0 mask
        U64 mask = (0x1L << (63 - indexOf(x, y))) | (0x1L << (63 - indexOf(_x, _y)));
        bitboards[typeOf(p)] ^= mask;
        setCrc(crc[y], Piece::Empty, x);
        setCrc(crc[_y], Piece::Empty, x);
    } else {
        remove(_x, _y);
    }
}

Bitboard::Color Bitboard::colorAt(size_t x, size_t y) const {
    return colorOf(pieceAt(x, y));
}

Bitboard::Type Bitboard::typeAt(size_t x, size_t y) const {
    return typeOf(pieceAt(x, y));
}

Bitboard::Piece Bitboard::pieceAt(size_t x, size_t y) const {
    U32 s = crc[y];

    return Piece((s >> (28 - 4 * x)) & 0xf);
}

Bitboard::Color Bitboard::colorOf(Bitboard::Piece p) {
    return Color((p & color_mask) >> 3);
}

Bitboard::Type Bitboard::typeOf(Bitboard::Piece p) {
    if (p != Empty)
        return Type((p & type_mask) - 1);

    return None;
}

Bitboard::Piece Bitboard::merge(Color c, Type t) {
    return Piece((c << 3) | (t + 1));
}

void Bitboard::setBitAt(Bitboard::U64 &s, size_t i) {
    s ^= (0x1L << (63 - i));
}

void Bitboard::removeBitAt(Bitboard::U64 &s, size_t i) {
    s &= ~(0x1L << (63 - i));
}

void Bitboard::setCrc(U32& s, Piece p, size_t x) {
    s &= ~((0xf) << (28 - 4 * x));
    s ^= (p << (28 - 4 * x));
}

size_t Bitboard::indexOf(size_t x, size_t y) {
    return x + (y << 3); // x + 8 * y
}

Bitboard::Wrapper Bitboard::operator[](size_t y) const {
    return crc[y];
}

template<>
Bitboard::U64 Bitboard::fixedAttackingSquares<Bitboard::King>(Bitboard::Color c) {
    //TODO test et + refactor
    U64 kings = bitboards[King] & occupies[c];
    U64 l = (kings >> 1) & 0x7f7f7f7f7f7f7f7f;
    U64 r = (kings << 1) & 0xfefefefefefefefe;
    U64 attacks = l | r;
    kings |= attacks;
    U64 u = kings >> 8;
    U64 d = kings << 8;
    return attacks | u | d;
}

template<>
Bitboard::U64 Bitboard::fixedAttackingSquares<Bitboard::Knight>(Bitboard::Color c) {
    U64 knights = bitboards[Knight] & occupies[c];
    U64 l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7f;
    U64 l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3f;
    U64 r1 = (knights << 1) & 0xfefefefefefefefe;
    U64 r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfc;
    U64 h1 = l1 | r1;
    U64 h2 = l2 | r2;
    return (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);
}

template<>
Bitboard::U64 Bitboard::fixedAttackingSquares<Bitboard::Pawn>(Bitboard::Color c) {
    return 0;
}

template<Bitboard::Direction d, Bitboard::Type t>
Bitboard::U64 Bitboard::slidingAttackingSquares(Color c) {

    // TODO Vectorize

    U64 empty = ~(occupies[White] | occupies[Black]);
    U64 gen = (bitboards[Queen] | bitboards[t]) & occupies[c]; // Queen and rooks of color

    U64 avoidMask;

    if constexpr(d == UP || d == DOWN)
        avoidMask = 0xffffffffffffffff;
    else if constexpr(d == LEFT || d == UP_LEFT || d == DOWN_LEFT)
        avoidMask = 0xfefefefefefefefe;
    else if constexpr(d == RIGHT || d == UP_RIGHT || d == DOWN_RIGHT)
        avoidMask = 0x7f7f7f7f7f7f7f7f;

    empty &= avoidMask;
    if constexpr(d > 0) {
        gen   |= empty & (gen   >> d);
        empty  = empty & (empty >> d);
        gen   |= empty & (gen   >> 2*d);
        empty  = empty & (empty >> 2*d);
        gen   |= empty & (gen   >> 4*d);
        return             (gen >> d) & avoidMask;
    } else {
        gen   |= empty & (gen   << -d);
        empty  = empty & (empty << -d);
        gen   |= empty & (gen   << -2*d);
        empty  = empty & (empty << -2*d);
        gen   |= empty & (gen   << -4*d);
        return             (gen << -d) & avoidMask;
    }
}

Bitboard::Piece Bitboard::Wrapper::operator[](size_t x) const {
    return Piece((s >> (28 - 4 * x)) & 0xf);
}

void printBits(uint32_t a, bool b) {
//    for (int i = 0; i < 8; ++i) {
//        for (int j = 0; j < 8; ++j) {
//            cout << ((a & (0x80L >> j)) >> (7 -j));
//        }
//        if (b) cout << endl;
//        a >>= 8;
//    }

    for (int i = 0; i < 32; ++i) {
        cout << ((a & 0x80000000) >> 31);
        a <<= 1;
    }

    cout << endl;
}

int main() {
    Bitboard b("8/8/8/8/8/8/8/2nk4");
//    uint64_t attacks = b.fixedAttackingSquares<Bitboard::Type::Knight>(Bitboard::Color::Black);
//    uint64_t attacks = b.bitboards[1];
//    printBits(attacks, true);
    cout << b[0][4] << endl;
}
