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

Square Board::squareOf(U8 x, U8 y) {
    return Square(x + (y << 3)); // x + 8 * y
}

U256 Board::getTypeArray(Piece::Piece p) {
   Piece::Type t  = typeOf(p);
   Piece::Color c = colorOf(p);
    return type_arr[c * 6 + t];
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

std::ostream& operator<<(std::ostream& strm, const Board& board) {
    strm << "  +---+---+---+---+---+---+---+---+\n";
    for (int y = 7; y >= 0; --y) {
        strm << (y + 1) << " ";
        for (int x = 0; x < 8; ++x) {
            strm <<"| " << board.pieceAt(Board::squareOf(x, y)) << " ";
        }
        strm << "|\n  +---+---+---+---+---+---+---+---+\n";;
    }
    strm << "    A   B   C   D   E   F   G   H\n";
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const Piece::Piece& piece) {
    char tmp;
    switch(Piece::Type(piece >> 1)) {
        case Piece::KING: tmp = 'k'; break;
        case Piece::KNIGHT: tmp = 'n'; break;
        case Piece::BISHOP: tmp = 'b'; break;
        case Piece::ROOK: tmp = 'r'; break;
        case Piece::PAWN: tmp = 'p'; break;
        case Piece::QUEEN: tmp = 'q'; break;
        case Piece::NONE: return strm << ' ';
    }

    return strm << ((piece & 1) ? tmp : (char)(tmp - 32));
}

std::ostream& operator<<(std::ostream& strm, const Square& square) {
    if (square < 0) return strm << '-';
    int x = square % 8;
    int y = (square - x) / 8;
    strm << (char)('A' + x) << (char)('1' + y);
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const Castling& castling) {
    if (castling & Castling::WHITE_K)  strm << 'K';
    if (castling & Castling::WHITE_Q)  strm << 'Q';
    if (castling & Castling::BLACK_K)  strm << 'k';
    if (castling & Castling::BLACK_Q)  strm << 'q';
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const Move::Move& move) {
    strm << '{';
    strm << Square(move & 0x3f);
    strm << "->";
    strm << Square((move >> 6) & 0x3f);
    strm << " | ";
    switch (Move::Type((move >> 12) & 0x3))
    {
        case Move::CASTLING: strm << "CS"; break;
        case Move::EN_PASSANT: strm << "EP"; break;
        case Move::NORMAL: strm << "NM"; break;
        case Move::PROMOTION: strm << "PM"; break;
    }
    strm << '}';
    return strm;
}