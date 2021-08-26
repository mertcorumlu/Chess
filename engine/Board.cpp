//
// Created by Mert Çorumlu on 20.08.2021.
//

#include "Board.h"
#include <iostream>

Board::Board(string &&fen) : quadboard() {

    // Initialize Quadboard
    quadboard = _mm256_setzero_si256();

    int x = 0, y = 7;

    for(auto& c : fen) {
        auto color = Piece::Color(islower(c) ? 1 : 0);
        switch (c) {
        case 'p': case 'P': add(merge(color, Piece::PAWN), squareOf(x++, y)); break;
            case 'r': case 'R': add(merge(color, Piece::ROOK), squareOf(x++, y)); break;
            case 'n': case 'N': add(merge(color, Piece::KNIGHT), squareOf(x++, y)); break;
            case 'b': case 'B': add(merge(color, Piece::BISHOP), squareOf(x++, y)); break;
            case 'q': case 'Q': add(merge(color, Piece::QUEEN), squareOf(x++, y)); break;
            case 'k': case 'K': add(merge(color, Piece::KING), squareOf(x++, y)); break;

            case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
                for(int i = 0; i < c - '0'; ++i) add(Piece::Empty, squareOf(x++, y));
                break;

            case '/':
                --y;
                x = 0;
                break;

            default: continue;
        }
    }
}

void Board::add(Piece::Piece p, Square square) {
    // IMPORTANT: Only for initial adding, later use move() instead
    quadboard |= SHL(getTypeArray(p), square);
}

Piece::Piece Board::remove(Square square) {
    Piece::Piece p = pieceAt(square);

    quadboard ^= SHL(getTypeArray(p), square);
    return p;
}

void Board::move(Square from, Square to) {
    // First remove piece at (x, y)
    Piece::Piece p = remove(from);
    remove(to);
    add(p, to);
}

Piece::Piece Board::pieceAt(Square square) const {
    U256 tmp = _mm256_slli_epi64(quadboard, 63 - square);
    U32 val = _mm256_movemask_epi8(tmp);
   return Piece::Piece(_pext_u32(val, 0x80808080));
}

Piece::Color Board::colorOf(Piece::Piece p) {
    return Piece::Color(p & color_mask);
}

Piece::Type Board::typeOf(Piece::Piece p) {
    return Piece::Type(p & type_mask);
}

Piece::Piece Board::merge(Piece::Color c, Piece::Type t) {
    return Piece::Piece(t | c);
}

Square Board::squareOf(U8 x, U8 y) {
    return Square(x + (y << 3)); // x + 8 * y
}

U256 Board::getTypeArray(Piece::Piece p) {
   Piece::Type t  = typeOf(p);
   Piece::Color c = colorOf(p);
    return type_arr[c * 6 + (t>>1)];
}

U64 Board::getBlack() const {
    return quadboard[0];
}

U64 Board::getWhite() const {
    return quadboard[0] ^ getOccupied();
}

U64 Board::getOccupied() const {
    return quadboard[1] | quadboard[2] | quadboard[3];
}

U64 Board::getOdd() const {
    return quadboard[1] ^ quadboard[2] ^ quadboard[3];
}

U64 Board::getSlidingOrthogonal() const {
    // (b1 & b3) | ((b1 ^ b2 ^ b3) & b3) => b3 & ((b1 ^ b2 ^ b3) | b1)
    return quadboard[3] & (getOdd() | quadboard[1]);
}

U64 Board::getSlidingDiagonal() const {
    // (b1 & b3) | (b1 & b2) => b1 & (b2 | b3)
    return quadboard[1] & (quadboard[2] | quadboard[3]);
}

U64 Board::getBishops() const {
    return quadboard[1] & quadboard[2];
}

U64 Board::getRooks() const {
    return quadboard[3] & getOdd();
}

U64 Board::getQueens() const {
    return quadboard[1] & quadboard[3];
}

U64 Board::getKnights() const {
    return quadboard[2] & getOdd();
}

U64 Board::getKings() const {
    return quadboard[2] & quadboard[3];
}

U64 Board::getPawns() const {
    return quadboard[1] & getOdd();
}

Board::Proxy Board::operator[](const U8& x) const {
    return Proxy(*this, x);
}

Piece::Piece Board::Proxy::operator[](U8 y) const {
    return board.pieceAt(squareOf(x, y));
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
