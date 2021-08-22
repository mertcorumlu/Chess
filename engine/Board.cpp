//
// Created by Mert Çorumlu on 20.08.2021.
//

#include "Board.h"
#include <iostream>

Board::Board(string &&fen) : quadboard(), attacking() {

    // Initialize Quadboard
    quadboard = _mm256_setzero_si256();

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

    updateAttackingSquares(Board::Color::White);
    updateAttackingSquares(Board::Color::Black);
}

void Board::add(Board::Piece p, U8 x, U8 y) {
    // IMPORTANT: Only for initial adding, later use move() instead
    quadboard |= SHL(getTypeArray(p), indexOf(x, y));
}

Board::Piece Board::remove(U8 x, U8 y) {
    Piece p = pieceAt(x, y);

    quadboard ^= SHL(getTypeArray(p), indexOf(x, y));
    updateAttackingSquares(Board::Color::White);
    updateAttackingSquares(Board::Color::Black);
    return p;
}

void Board::move(U8 x, U8 y, U8 _x, U8 _y) {
    // First remove piece at (x, y)
    Piece p = remove(x, y);
    remove(_x, _y);
    add(p, _x, _y);

    updateAttackingSquares(Board::Color::White);
    updateAttackingSquares(Board::Color::Black);
}

Board::Color Board::colorAt(U8 x, U8 y) const {
    return colorOf(pieceAt(x, y));
}

Board::Type Board::typeAt(U8 x, U8 y) const {
    return typeOf(pieceAt(x, y));
}

Board::Piece Board::pieceAt(U8 x, U8 y) const {
    U8 index = indexOf(x, y);
   return Piece(
           ((quadboard[0] >> index) & 1) |
       2 * ((quadboard[1] >> index) & 1) |
       4 * ((quadboard[2] >> index) & 1) |
       8 * ((quadboard[3] >> index) & 1)
    );
}

Board::Color Board::colorOf(Board::Piece p) {
    return Color(p & color_mask);
}

Board::Type Board::typeOf(Board::Piece p) {
    return Type((p & type_mask) >> 1);
}

Board::Piece Board::merge(Color c, Type t) {
    return Piece((t << 1) | c);
}

U8 Board::indexOf(U8 x, U8 y) {
    return x + (y << 3); // x + 8 * y
}

U256 Board::getTypeArray(Piece p) {
    Type t  = typeOf(p);
    Color c = colorOf(p);
    return type_arr[c * 6 + t];
}

template<>
U64 Board::fixedAttackingSquares<Board::King>(Board::Color c) {
    //TODO test et + refactor

    // No need to SIMD
    U64 kings = (c ? getBlack() : ~getBlack()) & getKings();
    U64 l = (kings >> 1) & ~GFile;
    U64 r = (kings << 1) & ~AFile;
    U64 attacks = l | r;
    kings |= attacks;
    U64 u = kings >> 8;
    U64 d = kings << 8;
    return attacks | u | d;
}

template<>
U64 Board::fixedAttackingSquares<Board::Knight>(Board::Color c) {

    constexpr static U256 qmask_east = {
            static_cast<long long>(~(AFile | BFile)),
            static_cast<long long>(~AFile),
            static_cast<long long>(~HFile),
            static_cast<long long>(~(GFile | HFile))
    };

    constexpr static U256 qmask_west = {
            static_cast<long long>(~(GFile | HFile)),
            static_cast<long long>(~HFile),
            static_cast<long long>(~AFile),
            static_cast<long long>(~(AFile | BFile))
    };

    constexpr static U256 qshift  = {10, 17, 15 , 6};

    U256 qknights = _mm256_set1_epi64x(static_cast<long long>((c ? getBlack() : ~getBlack()) & getKnights()));

    U256 ret = (qknights << qshift) & qmask_east;
    ret |= (qknights >> qshift) & qmask_west;

    return ret[0] | ret[1] | ret[2] | ret[3];
}

template<>
U64 Board::fixedAttackingSquares<Board::Pawn>(Board::Color c) {

    // No need to SIMD
    U64 kings = (c ? getBlack() : ~getBlack()) & getPawns();

    U64 l, r;
    if (c) {
        l = (kings >> 9) & ~HFile;
        r = (kings >> 7) & ~AFile;
    } else {
        l = (kings << 9) & ~AFile;
        r = (kings << 7) & ~HFile;
    }
    return l | r;
}

U64 Board::slidingAttackingSquares(Color c) {

    // Todo can be replaced with magic bitboards

    constexpr static U256 qmask_east  = {
            static_cast<long long>(~AFile),
            -1,
            static_cast<long long>(~HFile),
            static_cast<long long>(~AFile)
    };

    constexpr static U256 qmask_west  = {
            static_cast<long long>(~HFile),
            -1,
            static_cast<long long>(~AFile),
            static_cast<long long>(~HFile)
    };

    constexpr static U256 qshift = {1, 8, 7, 9};

    auto rq = static_cast<long long>(getSlidingOrthogonal());
    auto bq = static_cast<long long>(getSlidingDiagonal());

    auto color = static_cast<long long>(c ? getBlack() : ~getBlack());

    rq &= color;
    bq &= color;

    U256 gen = {rq, rq, bq, bq};

    U256 empty_east = _mm256_set1_epi64x(static_cast<long long>(~getOccupied()));
    U256 empty_west{empty_east};

    empty_east &= qmask_east;
    empty_west &= qmask_west;

    U256 gen1{gen};
    U256 qflood1 {gen1};
    qflood1 |= gen1 = (gen1 << qshift) & empty_east;
    qflood1 |= gen1 = (gen1 << qshift) & empty_east;
    qflood1 |= gen1 = (gen1 << qshift) & empty_east;
    qflood1 |= gen1 = (gen1 << qshift) & empty_east;
    qflood1 |= gen1 = (gen1 << qshift) & empty_east;
    qflood1 |=        (gen1 << qshift) & empty_east;
    qflood1  =     (qflood1 << qshift) & qmask_east;

    U256 qflood2 (gen);
    qflood2 |= (gen >> qshift) & empty_west;
    qflood2 |= gen = (gen >> qshift) & empty_west;
    qflood2 |= gen = (gen >> qshift) & empty_west;
    qflood2 |= gen = (gen >> qshift) & empty_west;
    qflood2 |= gen = (gen >> qshift) & empty_west;
    qflood2 |=       (gen >> qshift) & empty_west;
    qflood2  =   (qflood2 >> qshift) & qmask_west;

    qflood2 |= qflood1;
    return qflood2[0] | qflood2[1] | qflood2[2] | qflood2[3];
}

U64 Board::getBlack() {
    return quadboard[0];
}

U64 Board::getWhite() {
    return quadboard[0] ^ getOccupied();
}

U64 Board::getOccupied() {
    return quadboard[1] | quadboard[2] | quadboard[3];
}

U64 Board::getOdd() {
    return quadboard[1] ^ quadboard[2] ^ quadboard[3];
}

U64 Board::getSlidingOrthogonal() {
    // (b1 & b3) | ((b1 ^ b2 ^ b3) & b3) => b3 & ((b1 ^ b2 ^ b3) | b1)

    return quadboard[3] & (getOdd() | quadboard[1]);
}

U64 Board::getSlidingDiagonal() {
    // (b1 & b3) | (b1 & b2) => b1 & (b2 | b3)
    return quadboard[1] & (quadboard[2] | quadboard[3]);
}

U64 Board::getBishops() {
    return quadboard[1] & quadboard[2];
}

U64 Board::getRooks() {
    return quadboard[3] & getOdd();
}

U64 Board::getQueens() {
    return quadboard[1] & quadboard[3];
}

U64 Board::getKnights() {
    return quadboard[2] & getOdd();
}

U64 Board::getKings() {
    return quadboard[2] & quadboard[3];
}

U64 Board::getPawns() {
    return quadboard[1] & getOdd();
}

Board::Proxy Board::operator[](const U8& x) const {
    return Proxy(*this, x);
}

void Board::updateAttackingSquares(Board::Color c) {
    attacking[c] = 0;
    attacking[c] |= fixedAttackingSquares<Pawn>(c) | fixedAttackingSquares<King>(c) |
                    fixedAttackingSquares<Knight>(c) | slidingAttackingSquares(c);
    attacking[c] &= c ? ~getBlack() : ~getWhite();
}

Board::Piece Board::Proxy::operator[](U8 y) const {
    return board.pieceAt(x, y);
}

void Board::printBoard(U64 a) {

    for (int i = 0; i < 8; ++i) {
        uint8_t s = (a & 0xff00000000000000) >> 56;
        cout << 8 - i << "  ";
        for (int j = 0; j < 8; ++j) {
            cout << ((s & 0x1) ? '1' : '.') << "  ";
            s >>= 1;
        }
        cout << endl;
        a <<= 8;
    }

    cout << "   a  b  c  d  e  f  g  h" << endl;
    cout << endl;
}