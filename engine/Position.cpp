//
// Created by Mert Çorumlu on 24.08.2021.
//

#include "Position.h"
#include "Attack.h"
#include <string>
#include <sstream>
#include <iostream>

//Position::Position(Board board, Piece::Color sideToMove, Piece::Piece lastCaptured,
//                   Square enPassantTarget, Castling castlingRights, int halfMoveCLock,
//                   int fullMoveCounter, Square kingPos[2])
//        : _board{board}, _sideToMove(sideToMove), _enPassantTarget(enPassantTarget),
//          _halfMoveClock(halfMoveCLock), _fullMoveCounter(fullMoveCounter),
//          _moves(256), _kingPos{kingPos[0], kingPos[1]} {
//
//    _findCheckers();
//    _findPinned();
//}

Position::Position(string &&fen) {

    _states.push(make_shared<State>());
    currState = _states.top();

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
    if (splits[2][0] == '-') currState->_castlingRights = Castling::NO_CASTLING;
    else {
        currState->_castlingRights = Castling::NO_CASTLING;

        for (const auto &c : splits[2]) {
            switch (c) {
                case 'k' :
                    currState->_castlingRights = Castling(currState->_castlingRights | Castling::BLACK_K);
                    break;
                case 'q' :
                    currState->_castlingRights = Castling(currState->_castlingRights | Castling::BLACK_Q);
                    break;
                case 'K' :
                    currState->_castlingRights = Castling(currState->_castlingRights | Castling::WHITE_K);
                    break;
                case 'Q' :
                    currState->_castlingRights = Castling(currState->_castlingRights | Castling::WHITE_Q);
                    break;
                default:
                    throw std::invalid_argument("Illegal castling character");
            }
        }
    }

    // En Passant
    if (splits[3][0] == '-') _enPassantTarget = Square(-1);
    else {
        _enPassantTarget = Square(splits[3][0] - 'a' + 8 * (splits[3][1] - '1'));
    }

    // Half move
    currState->_halfMoveClock = std::stoi(splits[4]);

    // Full move
    _fullMoveCounter = std::stoi(splits[5]);

    // Find kings;
    U64 kings = _board.getKings();
    U64 whiteKing = kings & _board.getWhite();
    U64 blackKing = kings & _board.getBlack();
    _kingPos[0] = Square(whiteKing ? lsb(whiteKing) : -1);
    _kingPos[1] = Square(blackKing ? lsb(blackKing) : -1);

    // Find Checkers
    _findCheckers();

    // Find pieces who are avoiding being in check
    _findPinned();
}

U64 Position::squareAttackedBy(Square square, Piece::Color attacker) {
    U64 attackers = 0;
    U64 occupied = _board.getOccupied();

    // Place that square the piece and see squares that attacks corresponds enemy occupancy;
    attackers |= (Attack::nonSlidingAttacks(Piece::KNIGHT, square) & _board.getKnights());
    attackers |= (Attack::nonSlidingAttacks(Piece::KING, square) & _board.getKings());
    attackers |= (Attack::nonSlidingAttacks(Piece::PAWN, square, Piece::Color(!attacker)) &
                  _board.getPawns()); // Exception, place out pawn
    attackers |= (Attack::slidingAttacks(Piece::ROOK, square, occupied) & _board.getSlidingOrthogonal());
    attackers |= (Attack::slidingAttacks(Piece::BISHOP, square, occupied) & _board.getSlidingDiagonal());
    return attackers & (attacker ? _board.getBlack() : _board.getWhite());
}

template<Piece::Type t>
void Position::generatePseudoLegalMoves(MoveList& _moves) {

    Piece::Color c = _sideToMove;
    U64 allies = c ? _board.getBlack() : _board.getWhite();
    U64 pieces;

    if constexpr(t == Piece::KING) pieces = square_to_board(_kingPos[c]);
    else if constexpr(t == Piece::KNIGHT) pieces = _board.getKnights() & allies;
    else if constexpr(t == Piece::ROOK) pieces = _board.getRooks() & allies;
    else if constexpr(t == Piece::BISHOP) pieces = _board.getBishops() & allies;
    else if constexpr(t == Piece::QUEEN) pieces = _board.getQueens() & allies;

    while(pieces) {
        //TODO already know kings position
        // see if adding hat constexpr condition makes it faster
        Square from = pop_lsb(pieces);

        U64 attacks;

        if constexpr(t == Piece::KING || t == Piece::KNIGHT)
            attacks = Attack::nonSlidingAttacks(t, from);
        else if constexpr(t == Piece::ROOK || t == Piece::BISHOP || t == Piece::QUEEN)
            attacks = Attack::slidingAttacks(t, from, _board.getOccupied());

        // We dont want attacks on friendly pieces
        attacks &= ~allies;

        while (attacks) {
            Square to = pop_lsb(attacks);
            _moves.push_back(make_move(from, to, Move::NORMAL));
        }
    }

    // Check also castling moves for king
    if constexpr(t == Piece::KING) {
        Castling cColor = c ? Castling::BLACK : Castling::WHITE;
        Square from = c ? Square::E8 : Square::E1;
        auto enemy = Piece::Color(!c);

        if (currState->_castlingRights & Castling::QUEEN & cColor) {
            U64 mask = shift(0b01110ULL, Direction(c ? 7 * UP : 0));

            // There is a piece between
            if (_board.getOccupied() & mask) return;

            Square to;
            if (!squareAttackedBy(pop_lsb(mask), enemy) && !squareAttackedBy(to = pop_lsb(mask), enemy) &&
                !squareAttackedBy(pop_lsb(mask), enemy)) {
                _moves.push_back(make_move(from, to, Move::CASTLING));
            }
        }

        if (currState->_castlingRights & Castling::KING & cColor) {
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

template<>
void Position::generatePseudoLegalMoves<Piece::PAWN>(MoveList& _moves) {

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

    while (single) {
        Square square = pop_lsb(single);
        _moves.push_back(make_move(Square(square - pushDirection), square, Move::NORMAL));
    }

    while (doubles) {
        Square square = pop_lsb(doubles);
        _moves.push_back(make_move(Square(square - 2 * pushDirection), square, Move::NORMAL));
    }


    // En Passant
    if (_enPassantTarget != Square::NONE) {
        U64 en_passant = Attack::nonSlidingAttacks(Piece::PAWN, _enPassantTarget, Piece::Color(!c)) & pieces;

        while (en_passant) {
            Square square = pop_lsb(en_passant);
            _moves.push_back(make_move(square, _enPassantTarget, Move::EN_PASSANT));
        }
    }

    // Attacks
    while (pieces) {
        Square from = pop_lsb(pieces);
        U64 attacks = Attack::nonSlidingAttacks(Piece::PAWN, from, c) & enemies;

        while (attacks) {
            Square to = pop_lsb(attacks);
            _moves.push_back(make_move(from, to, Move::NORMAL));
        }

    }


}

void Position::generateAllLegalMoves(MoveList& _moves) {

    // For no checkers and single checker generate also moves for all other pieces;
    if (powerOfTwo(currState->_checkers)) {
        generatePseudoLegalMoves<Piece::KNIGHT>(_moves);
        generatePseudoLegalMoves<Piece::PAWN>(_moves);
        generatePseudoLegalMoves<Piece::BISHOP>(_moves);
        generatePseudoLegalMoves<Piece::ROOK>(_moves);
        generatePseudoLegalMoves<Piece::QUEEN>(_moves);
    }

    // For double checker you can only move king
    generatePseudoLegalMoves<Piece::KING>(_moves);

    // Eliminate illegal moves
    for (auto curr = _moves.begin(); curr != _moves.end();) {
        if (_isLegal(*curr))
            ++curr;
        else
            curr = _moves.erase(curr);
    }

}

void Position::_findCheckers() {
    Piece::Color c = _sideToMove;
    currState->_checkers = squareAttackedBy(_kingPos[c], Piece::Color(!c));
}

void Position::_findPinned() {

    Square king = _kingPos[_sideToMove];
    U64 occupied = _board.getOccupied();
    U64 allies = _sideToMove ? _board.getBlack() : _board.getWhite();
    U64 enemies = occupied ^allies;

    // Initialize 0
    currState->_pinned = 0;

    // BISHOP + QUEEN
    U64 xrays = Attack::xraySlidingAttacks<Piece::BISHOP>(king, occupied, allies)
                & _board.getSlidingDiagonal() & enemies;

    while (xrays) {
        Square to = pop_lsb(xrays);
        currState->_pinned |= Attack::maskBetween(king, to) & allies;
    }

    // ROOK + QUEEN
    xrays = Attack::xraySlidingAttacks<Piece::ROOK>(king, occupied, allies)
            & _board.getSlidingOrthogonal() & enemies;

    while (xrays) {
        Square to = pop_lsb(xrays);
        currState->_pinned |= Attack::maskBetween(king, to) & allies;
    }

}

bool Position::_isLegal(const Move::Move &move) {

    Move::Type type = move_type(move);
    Square from = move_from(move);
    Square to = move_to(move);
    Square king = _kingPos[_sideToMove];

    // Double check possibility will be catch here
    if (from == king) {

        // King cannot be under attack before castling
        if (type == Move::CASTLING)
            return !currState->_checkers;
            // to square should not be under attack
        else
            return !squareAttackedBy(to, Piece::Color(!_sideToMove));
    }

    // Check if moved piece is pinned
    if (square_to_board(from) & currState->_pinned) {
        // Than it can only be moved towards the line between itself and king
        return Attack::lineBetween(king, from) & square_to_board(to);
    }

    // moving piece not a king and king is in check, only legal move is to block checker
    // already know there will be just one checker, as it is checked in generateAllLegalMoves
    // So the moving peace should land on the line between king and checker
    return !currState->_checkers || (Attack::maskBetween(king, lsb(currState->_checkers)) & square_to_board(to));
}

void Position::do_move(Move::Move move) {

    // ASSUMES MOVE IS LEGAL!

    // create new state for new move
    auto prevState = currState;
    _states.push(make_shared<State>());
    currState = _states.top();

    Move::Type type = move_type(move);
    Square from = move_from(move);
    Square to = move_to(move);
    Piece::Piece moved = _board.pieceAt(from);
    Piece::Piece captured = _board.pieceAt(to);
    Piece::Color c = _sideToMove;
    int halfMoves = prevState->_halfMoveClock;
    Castling castling = prevState->_castlingRights;
    Square en_passant = Square(-1); // Default

    if (typeOf(moved) == Piece::KING) {

        if (type == Move::CASTLING) {

            Square qTarget = c ? C8 : C1;

            // Queen side
            if (to == qTarget) {
                Square rook = c ? A8 : A1;
                _board.move(rook, Square(rook + 3));

            // King side
            } else {
                Square rook = c ? H8 : H1;
                _board.move(rook, Square(rook - 2));
            }
        }

        // side loses castling rights
        Castling rights = c ? Castling::BLACK : Castling::WHITE;
        castling = Castling(castling & ~rights);

        // Also update king position
        _kingPos[c] = to;

    } else if (typeOf(moved) == Piece::ROOK) {
        // Moving rooks ends up in losing castling rights
        Square qTarget = c ? A8 : A1;
        Square kTarget = c ? H8 : H1;

        if (from == qTarget) {
            Castling rights = c ? Castling::BLACK_Q : Castling::WHITE_Q;
            castling = Castling(castling & ~rights);
        } else if (from == kTarget) {
            Castling rights = c ? Castling::BLACK_K : Castling::WHITE_K;
            castling = Castling(castling & ~rights);
        }
    } else if (typeOf(moved) == Piece::PAWN) {
        // Double push
        if (std::abs(from - to)  == 16) {
            en_passant = Square(from + (c ? -1 : +1) * 8);
        } else if (type == Move::EN_PASSANT) {
            // Remove captured piece
            captured = _board.remove(Square(to + (c ? +1 : -1) * 8));
        }

        // reset half move clock
        halfMoves = -1;
    }

    if (captured != Piece::Empty) halfMoves = -1;

    // Move piece on board
    _board.move(from, to);

    currState->_halfMoveClock = ++halfMoves;
    currState->_lastCaptured = captured;
    currState->_castlingRights = castling;
    _enPassantTarget = en_passant;
    ++_fullMoveCounter;
    _sideToMove = Piece::Color(!_sideToMove);

    _findCheckers();
    _findPinned();
}

void Position::undo_move(Move::Move move) {

    // TODO undo previous move and retrieve previous state

}

// toString method
std::ostream &operator<<(std::ostream &strm, const Position &pos) {

    strm << pos.board() << endl;

    strm << "Side to Move:      " << (pos.sideToMove() ? 'b' : 'w') << endl;

    strm << "Last Captured:     ";
    if (pos.lastCaptured())
        strm << pos.lastCaptured();
    else
        strm << '-';

    strm << endl;
    strm << "En Passant Target: " << pos.enPassantTarget() << endl;
    strm << "Castling Rights:   " << pos.castlingRights() << endl;
    strm << "Half Move Clock:   " << pos.halfMoveClock() << endl;
    strm << "Full Move Counter: " << pos.fullMoveCounter() << endl;
    strm << "King Positions:    w=" << pos.kingPos()[0] << "  b=" << pos.kingPos()[1] << endl;
    return strm;
}

std::ostream& operator<<(std::ostream &strm, const MoveList &a) {
    strm << "Moves:             ";
    for (const auto &move : a)
        strm << move << ", ";
    strm << '\b' << '\b' << endl;
    return strm;
}

const Board &Position::board() const {
    return _board;
}

Piece::Color Position::sideToMove() const {
    return _sideToMove;
}

Piece::Piece Position::lastCaptured() const {
    return currState->_lastCaptured;
}

Square Position::enPassantTarget() const {
    return _enPassantTarget;
}

Castling Position::castlingRights() const {
    return currState->_castlingRights;
}

int Position::halfMoveClock() const {
    return currState->_halfMoveClock;
}

int Position::fullMoveCounter() const {
    return _fullMoveCounter;
}

U64 Position::checkers() const {
    return currState->_checkers;
}

U64 Position::pinned() const {
    return currState->_pinned;
}

const Square *Position::kingPos() const {
    return _kingPos;
}

template void Position::generatePseudoLegalMoves<Piece::KING>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::KNIGHT>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::BISHOP>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::QUEEN>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::ROOK>(MoveList&);
