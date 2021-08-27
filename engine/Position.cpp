//
// Created by Mert Çorumlu on 24.08.2021.
//

#include "Position.h"
#include "Attack.h"
#include <string>
#include <sstream>
#include <iostream>

Position::Position(Board &&board,  Piece::Color sideToMove,  Piece::Piece lastCaptured,
                    Square enPassantTarget,  Castling castlingRights,  int halfMoveCLock,
                    int fullMoveCounter, Square kingPos[2])
        : _board{board}, _sideToMove(sideToMove), _lastCaptured(lastCaptured), _enPassantTarget(enPassantTarget),
          _castlingRights(castlingRights), _halfMoveClock(halfMoveCLock), _fullMoveCounter(fullMoveCounter), _moves(256),
          _checkers(), _pinned(), _kingPos{kingPos[0], kingPos[1]} {

    _findCheckers();
    _findPinned();
}

Position::Position(string &&fen) {

    // construct a stream from the string
    stringstream ss(fen);

    string s;
    string splits[6];
    for (int i = 0; std::getline(ss, s, ' '); ++i) {
        splits[i] = std::move(s);
    }

    // Board layout
    _board = Board(std::move(splits[0]));

    // Side To move
    if (splits[1][0] == 'w') _sideToMove = Piece::WHITE;
    if (splits[1][0] == 'b') _sideToMove = Piece::BLACK;

    // Castlings
    if (splits[2][0] == '-') _castlingRights = Castling::NO_CASTLING;
    else {
        _castlingRights = Castling::NO_CASTLING;

        for(const auto& c : splits[2]) {
            switch (c) {
                case 'k' : _castlingRights = Castling(_castlingRights | Castling::BLACK_K); break;
                case 'q' : _castlingRights = Castling(_castlingRights | Castling::BLACK_Q); break;
                case 'K' : _castlingRights = Castling(_castlingRights | Castling::WHITE_K); break;
                case 'Q' : _castlingRights = Castling(_castlingRights | Castling::WHITE_Q); break;
                default: throw std::invalid_argument("Illegal castling character");
            }
        }
    }

    // En Passant
    if (splits[3][0] == '-') _enPassantTarget = Square(-1);
    else {
        _enPassantTarget = Square(splits[3][0] - 'a' + 8 * (splits[3][1] - '1'));
    }

    // Half move
    _halfMoveClock = std::stoi(splits[4]);

    // Full move
    _fullMoveCounter = std::stoi(splits[5]);

    // Find kings;
    U64 kings = _board.getKings();

    _kingPos[0] = lsb(kings & _board.getWhite());
    _kingPos[1] = lsb(kings & _board.getBlack());

    // Find Checkers
   _findCheckers();
   _findPinned();

    // Find pieces who are avoiding being in check
}

U64 Position::squareAttackedBy(Square square, Piece::Color attacker) {
    U64 attackers = 0;
    U64 nonoccupied = ~_board.getOccupied();

    // Place that square the piece and see squares that attacks corresponds enemy occupancy;
    attackers |= (Attack::nonSlidingAttacks(Piece::KNIGHT, square) & _board.getKnights());
    attackers |= (Attack::nonSlidingAttacks(Piece::KING, square) & _board.getKings());
    attackers |= (Attack::nonSlidingAttacks(Piece::PAWN, square, Piece::Color(!attacker)) & _board.getPawns()); // Exception, place out pawn
    attackers |= (Attack::slidingAttacks(Piece::ROOK, square, nonoccupied) & _board.getSlidingOrthogonal());
    attackers |= (Attack::slidingAttacks(Piece::BISHOP, square, nonoccupied) & _board.getSlidingDiagonal());
    return attackers & (attacker ? _board.getBlack() : _board.getWhite());
}


template <Piece::Type t>
void Position::generatePseudoLegalMoves() {

    Piece::Color c = _sideToMove;
    U64 allies = c ? _board.getBlack() : _board.getWhite();
    U64 enemies = c ? _board.getWhite() : _board.getBlack();
    U64 nonoccupied = ~_board.getOccupied();
    U64 pieces;

    if constexpr(t == Piece::KING) pieces = 1 << _kingPos[c];
    else if constexpr(t == Piece::KNIGHT) pieces = _board.getKnights() & allies;
    else if constexpr(t == Piece::ROOK) pieces = _board.getRooks() & allies;
    else if constexpr(t == Piece::BISHOP) pieces = _board.getBishops() & allies;
    else if constexpr(t == Piece::QUEEN) pieces = _board.getQueens() & allies;

    while(pieces) {
        Square from = pop_lsb(pieces);

        U64 attacks;

        if constexpr(t == Piece::KING || t == Piece::KNIGHT )
            attacks = Attack::nonSlidingAttacks(t, from);
        else if constexpr(t == Piece::ROOK || t == Piece::BISHOP || t == Piece::QUEEN)
            attacks = Attack::slidingAttacks(t, from, nonoccupied);

        // We dont want attacks on friendly pieces
        attacks &= ~allies;

        while(attacks) {
            Square to = pop_lsb(attacks);
//            _moves.push_back(make_move(from, to, Move::NORMAL));
        }
    }

    // Check also castling moves for king
    if constexpr(t == Piece::KING) {
        Castling cColor = c ? Castling::BLACK : Castling::WHITE;
        Square from = c ? Square::E8 : Square::E1;
        auto enemy = Piece::Color(!c);

        if (_castlingRights & Castling::QUEEN & cColor) {
            U64 mask = shift(0b01110ULL, Direction(c ? 7 * UP : 0));

            bin(mask);

            // There is a piece between
            if (_board.getOccupied() & mask) return;

            Square to;
            if (!squareAttackedBy(pop_lsb(mask), enemy) && !squareAttackedBy(to = pop_lsb(mask), enemy) && squareAttackedBy(pop_lsb(mask), enemy)) {
                _moves.push_back(make_move(from, to, Move::CASTLING));
            }
        }

        if (_castlingRights & Castling::KING & cColor) {
            U64 mask = shift(0b0110'0000ULL, Direction(c ? 7 * UP : 0));

            // There is a piece between
            if (_board.getOccupied() & mask) return;

            Square to;
            if (!squareAttackedBy(pop_lsb(mask), enemy) && !squareAttackedBy(to = pop_lsb(mask), enemy)) {
                _moves.push_back(make_move(from, to, Move::CASTLING));
            }

        }

    }


}

template <>
void Position::generatePseudoLegalMoves<Piece::PAWN>() {

    Piece::Color c = _sideToMove;
    U64 enemies = c ? _board.getWhite() : _board.getBlack();
    U64 occupied = _board.getOccupied();
    U64 pieces = _board.getPawns() & (c ? _board.getBlack() : _board.getWhite());
    Direction pushDirection = c ? DOWN : UP;

    // Single push
    U64 single = shift(pieces, pushDirection) & ~occupied;

    // Double push
    U64 doubles = single & shift(c ? rank8 : rank1, Direction(2 * pushDirection));
    doubles = shift(doubles, pushDirection) & ~occupied;

    while(single) {
        Square square = pop_lsb(single);
        _moves.push_back(make_move(Square(square - pushDirection), square, Move::NORMAL));
    }

    while(doubles) {
        Square square = pop_lsb(doubles);
        _moves.push_back(make_move(Square(square - 2 * pushDirection), square, Move::NORMAL));
    }


    // En Passant
    U64 en_passant = Attack::nonSlidingAttacks(Piece::PAWN, _enPassantTarget, Piece::Color(!c)) & pieces;

    while(en_passant) {
        Square square = pop_lsb(en_passant);
        _moves.push_back(make_move(square, _enPassantTarget, Move::EN_PASSANT));
    }

    // Attacks
    while(pieces) {
        Square from = pop_lsb(pieces);
        U64 attacks = Attack::nonSlidingAttacks(Piece::PAWN, from, c) & enemies;

        while(attacks) {
            Square to = pop_lsb(attacks);
            _moves.push_back(make_move(from, to, Move::NORMAL));
        }

    }


}

template <Piece::Type t>
void Position::generateLegalMoves() {

}

template <>
void Position::generateLegalMoves<Piece::PAWN>() {

}

void Position::generateAllPseudoLegalMoves() {

}

void Position::generateAllLegalMoves() {

    // for single checker try to capture checker

    if (_checkers) {

        U64 checkers = _checkers;

        // Single check
        if (_checkers & (_checkers - 1)) {
            Square checker = pop_lsb(checkers);
            U64 attackers = squareAttackedBy(checker, Piece::Color(!_sideToMove));

            while (attackers) {
                Square from = pop_lsb(attackers);
                _moves.push_back(make_move(from, checker, Move::NORMAL));
            }
        }

        return;
    } else {
        // QUIET moves
        generateLegalMoves<Piece::KNIGHT>();
        generateLegalMoves<Piece::PAWN>();
        generateLegalMoves<Piece::BISHOP>();
        generateLegalMoves<Piece::ROOK>();
        generateLegalMoves<Piece::QUEEN>();

    }
    generateLegalMoves<Piece::KING>();
}

void Position::_findCheckers() {
    Piece::Color c = _sideToMove;
    _checkers =  squareAttackedBy(_kingPos[c], Piece::Color(!c));
}

void Position::_findPinned() {
    // TODO find pinned pieces
}

const Board &Position::board() const {
    return _board;
}

Piece::Color Position::sideToMove() const {
    return _sideToMove;
}

Piece::Piece Position::lastCaptured() const {
    return _lastCaptured;
}

Square Position::enPassantTarget() const {
    return _enPassantTarget;
}

Castling Position::castlingRights() const {
    return _castlingRights;
}

int Position::halfMoveClock() const {
    return _halfMoveClock;
}

int Position::fullMoveCounter() const {
    return _fullMoveCounter;
}

const MoveList &Position::moves() const {
    return _moves;
}

U64 Position::checkers() const {
    return _checkers;
}

U64 Position::pinned() const {
    return _pinned;
}

const Square *Position::kingPos() const {
    return _kingPos;
}

Position Position::move(Square from, Square to) {

    // make the move on board;
    Board tmp = _board;
    tmp.move(from, to);

    Piece::Piece movedPiece = _board.pieceAt(from);
    Piece::Piece capturedPiece = _board.pieceAt(to);
    Piece::Type type = Board::typeOf(movedPiece);
    Piece::Color c = _sideToMove;
    int halfMoves = _halfMoveClock;
    Castling castling = _castlingRights;
    Square kingPos[2] = {_kingPos[0], _kingPos[1]};

    Square en_passant = Square(-1);

    // Castling rights
    if (type == Piece::KING) {
        castling = Castling(castling & ~(c ? Castling::BLACK : Castling::WHITE));
        kingPos[c] = to;
    }

    if (type == Piece::ROOK) {
        // Queen side
        if (from == (c ? A8 : A1)) {
            Castling cc = c ? Castling::BLACK : Castling::WHITE;
            castling = Castling(castling & ~(Castling::QUEEN & cc));
        // King side
        } else if (from == (c ? H8 : H1)) {
            Castling cc = c ? Castling::BLACK : Castling::WHITE;
            castling = Castling(castling & ~(Castling::KING & cc));
        }
    }

    // Half move counter and en passant
    // Catch en passant moves
    if (type == Piece::PAWN) {
        if (to == from + 16 * (c ? -1 : 1)) {
            en_passant = Square(from + 8 * (c ? -1 : 1));
        }
        halfMoves = -1;
    } else if(capturedPiece != Piece::Empty) {
        halfMoves = -1;
    }

    return Position(std::move(tmp), Piece::Color(_sideToMove), capturedPiece, en_passant, castling, ++halfMoves, ++_fullMoveCounter, kingPos);
}

// toString method
std::ostream& operator<<(std::ostream &strm, const Position &pos) {

    strm << pos.board() << endl;

    strm << "Side to Move:      " << (pos.sideToMove() ? 'b' : 'w') << endl;

//    strm << "Last Captured:     ";
//    if (pos.lastCaptured())
//         strm << pos.lastCaptured();
//    else
//        strm << '-';

//    strm << endl;
//    strm << "En Passant Target: " << pos.enPassantTarget() << endl;
//    strm << "Castling Rights:   " << pos.castlingRights() << endl;
//    strm << "Half Move Clock:   " << pos.halfMoveClock() << endl;
//    strm << "Full Move Counter: " << pos.fullMoveCounter() << endl;
//    strm << "King Positions:    w=" << pos.kingPos()[0] << "  b=" << pos.kingPos()[1] << endl;
    strm << "Moves:             ";
    for (const auto& move : pos.moves())
        strm << move << ", ";
    strm << '\b' << '\b' << endl;
    return strm;
}


template void Position::generatePseudoLegalMoves<Piece::KING>();
template void Position::generatePseudoLegalMoves<Piece::KNIGHT>();
template void Position::generatePseudoLegalMoves<Piece::BISHOP>();
template void Position::generatePseudoLegalMoves<Piece::QUEEN>();
template void Position::generatePseudoLegalMoves<Piece::ROOK>();
