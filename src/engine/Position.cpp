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
    _currState = _states.top();

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
    if (splits[2][0] == '-') _currState->_castlingRights = Castling::NO_CASTLING;
    else {
        _currState->_castlingRights = Castling::NO_CASTLING;

        for (const auto &c : splits[2]) {
            switch (c) {
                case 'k' :
                    _currState->_castlingRights = Castling(_currState->_castlingRights | Castling::BLACK_K);
                    break;
                case 'q' :
                    _currState->_castlingRights = Castling(_currState->_castlingRights | Castling::BLACK_Q);
                    break;
                case 'K' :
                    _currState->_castlingRights = Castling(_currState->_castlingRights | Castling::WHITE_K);
                    break;
                case 'Q' :
                    _currState->_castlingRights = Castling(_currState->_castlingRights | Castling::WHITE_Q);
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
    _currState->_halfMoveClock = std::stoi(splits[4]);

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
    return squareAttackedBy(square, _board.getOccupied(), attacker);
}

U64 Position::squareAttackedBy(Square square, U64 occupied, Piece::Color attacker) {
    U64 attackers = 0;

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
            if (_board.pieceAt(to))
                _moves.push_back(make_move(from, to, Move::CAPTURE));
            else
                _moves.push_back(make_move(from, to, Move::QUIET));
        }
    }

    // Check also castling moves for king
    if constexpr(t == Piece::KING) {
        Castling cColor = c ? Castling::BLACK : Castling::WHITE;
        Square from = c ? Square::E8 : Square::E1;
        auto enemy = Piece::Color(!c);

        if (_currState->_castlingRights & Castling::KING & cColor) {
            U64 mask = shift(0b0110'0000ULL, Direction(c ? 7 * UP : 0));

            // There is a piece between
            if (!(_board.getOccupied() & mask)) {
                Square to;
                if (!squareAttackedBy(pop_lsb(mask), enemy) && !squareAttackedBy(to = pop_lsb(mask), enemy)) {
                    _moves.push_back(make_move(from, to, Move::CASTLE_K));
                }
            }
        }

        if (_currState->_castlingRights & Castling::QUEEN & cColor) {
            U64 mask = shift(0b1110ULL, Direction(c ? 7 * UP : 0));

            // There is a piece between
            if (!(_board.getOccupied() & mask)) {
                Square to;
                // Spend one bit for nothing, because B1/8 square is not considered if attacked,
                // But could not be occupied anyway
                pop_lsb(mask);
                if (!squareAttackedBy(to = pop_lsb(mask), enemy) && !squareAttackedBy(pop_lsb(mask), enemy)) {
                    _moves.push_back(make_move(from, to, Move::CASTLE_Q));
                }
            }
        }

    }

}

template<>
void Position::generatePseudoLegalMoves<Piece::PAWN>(MoveList& _moves) {

    //TODO Implement Promotions

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
        auto from = Square(square - pushDirection);
        // If single move lands on opposite rank, than add promotions
        if (square_to_board(square) & (c ? rank1 : rank8)) {

            _moves.push_back(make_move(from, square, Move::PROMOTION_Q));
            _moves.push_back(make_move(from, square, Move::PROMOTION_R));
            _moves.push_back(make_move(from, square, Move::PROMOTION_B));
            _moves.push_back(make_move(from, square, Move::PROMOTION_N));

        } else
            _moves.push_back(make_move(from, square, Move::QUIET));
    }

    while (doubles) {
        Square square = pop_lsb(doubles);
        _moves.push_back(make_move(Square(square - 2 * pushDirection), square, Move::D_PAWN_PUSH));
    }

    // En Passant
    if (_enPassantTarget != Square::NONE) {
        U64 en_passant = Attack::nonSlidingAttacks(Piece::PAWN, _enPassantTarget, Piece::Color(!c)) & pieces;

        while (en_passant) {
            Square square = pop_lsb(en_passant);
            _moves.push_back(make_move(square, _enPassantTarget, Move::EP_CAPTURE));
        }
    }

    // Attacks
    while (pieces) {
        Square from = pop_lsb(pieces);
        U64 attacks = Attack::nonSlidingAttacks(Piece::PAWN, from, c) & enemies;

        while (attacks) {
            Square to = pop_lsb(attacks);

            if (square_to_board(to) & (c ? rank1 : rank8)) {

                _moves.push_back(make_move(from, to, Move::PROMOCAP_Q));
                _moves.push_back(make_move(from, to, Move::PROMOCAP_R));
                _moves.push_back(make_move(from, to, Move::PROMOCAP_B));
                _moves.push_back(make_move(from, to, Move::PROMOCAP_N));

            } else
                _moves.push_back(make_move(from, to, Move::CAPTURE));
        }

    }

}

void Position::generateAllLegalMoves(MoveList& _moves) {

    // For no checkers and single checker generate also moves for all other pieces;
    if (powerOfTwo(_currState->_checkers)) {
        generatePseudoLegalMoves<Piece::PAWN>(_moves);
        generatePseudoLegalMoves<Piece::KNIGHT>(_moves);
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
    _currState->_checkers = squareAttackedBy(_kingPos[c], Piece::Color(!c));
}

void Position::_findPinned() {

    Square king = _kingPos[_sideToMove];
    U64 occupied = _board.getOccupied();
    U64 allies = _sideToMove ? _board.getBlack() : _board.getWhite();
    U64 enemies = occupied ^allies;

    // Initialize 0
    _currState->_pinned = 0;

    // BISHOP + QUEEN
    U64 xrays = Attack::xraySlidingAttacks<Piece::BISHOP>(king, occupied, allies)
                & _board.getSlidingDiagonal() & enemies;

    while (xrays) {
        Square to = pop_lsb(xrays);
        _currState->_pinned |= Attack::maskBetween(king, to) & allies;
    }

    // ROOK + QUEEN
    xrays = Attack::xraySlidingAttacks<Piece::ROOK>(king, occupied, allies)
            & _board.getSlidingOrthogonal() & enemies;

    while (xrays) {
        Square to = pop_lsb(xrays);
        _currState->_pinned |= Attack::maskBetween(king, to) & allies;
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
        if (move_castle(type))
            return !_currState->_checkers;
            // to square should not be under attack
        else{
            // Excluding king from occupancy because it can block itself while testing
            // if to square is attacked by any piece
            return !squareAttackedBy(to, _board.getOccupied() ^ square_to_board(from), Piece::Color(!_sideToMove));
        }
    }

    // Check if moved piece is pinned
    if (square_to_board(from) & _currState->_pinned) {
        // Than it can only be moved towards the line between itself and king
        if (!(Attack::lineBetween(king, from) & square_to_board(to))) return false;
    }

    // moving piece not a king and king is in check, only legal move is to block checker
    // already know there will be just one checker, as it is checked in generateAllLegalMoves
    // So the moving piece should land on the line between king and checker
    return !_currState->_checkers || (Attack::maskBetween(king, lsb(_currState->_checkers)) & square_to_board(to));
}

void Position::do_move(Move::Move move) {

    // ASSUMES MOVE IS LEGAL!

    // create new state for new move
    auto prevState = _currState;
    _states.push(make_shared<State>());
    _currState = _states.top();

    Move::Type type = move_type(move);
    Square from = move_from(move);
    Square to = move_to(move);
    Piece::Piece moved = _board.pieceAt(from);
    Piece::Piece captured = _board.pieceAt(to);
    Piece::Color c = _sideToMove;
    int halfMoves = prevState->_halfMoveClock;
    Castling castling = prevState->_castlingRights;
    auto en_passant = Square(-1); // Default

    if (typeOf(moved) == Piece::KING) {

        if (type == Move::CASTLE_Q) {
            Square rook = c ? A8 : A1;
            _board.move(rook, Square(rook + 3));
        } else if(type == Move::CASTLE_K) {
            Square rook = c ? H8 : H1;
            _board.move(rook, Square(rook - 2));
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

        if (type == Move::D_PAWN_PUSH) {
            en_passant = Square(from + (c ? -1 : +1) * 8);
        } else if (type == Move::EP_CAPTURE) {
            // Remove captured piece
            captured = _board.remove(Square(to + (c ? +1 : -1) * 8));
        }

        // reset half move clock
        halfMoves = -1;
    }

    if (captured != Piece::Empty) {

        if (typeOf(captured) == Piece::ROOK) {
            // capturing rooks ends up in losing castling rights for opponent
            Square qTarget = c ? A1 : A8;
            Square kTarget = c ? H1 : H8;

            if (to == qTarget) {
                Castling rights = c ? Castling::WHITE_Q : Castling::BLACK_Q;
                castling = Castling(castling & ~rights);
            } else if (to == kTarget) {
                Castling rights = c ? Castling::WHITE_K : Castling::BLACK_K;
                castling = Castling(castling & ~rights);
            }
        }

        halfMoves = -1;
    }

    // Move piece on board
    if (move_promotion(type)) {
        _board.remove(from);
        _board.remove(to);
        switch (type) {
            case Move::PROMOTION_B: case Move::PROMOCAP_B: _board.add(merge(c, Piece::BISHOP), to); break;
            case Move::PROMOTION_R: case Move::PROMOCAP_R: _board.add(merge(c, Piece::ROOK), to); break;
            case Move::PROMOTION_Q: case Move::PROMOCAP_Q: _board.add(merge(c, Piece::QUEEN), to); break;
            case Move::PROMOTION_N: case Move::PROMOCAP_N: _board.add(merge(c, Piece::KNIGHT), to); break;
            default: break; // silence the clang-tidy...
        }
    } else {
        _board.move(from, to);
    }

    _currState->_halfMoveClock = ++halfMoves;
    _currState->_lastCaptured = captured;
    _currState->_castlingRights = castling;
    _enPassantTarget = en_passant;
    ++_fullMoveCounter;
    _sideToMove = Piece::Color(!_sideToMove);

    _findCheckers();
    _findPinned();
}

void Position::undo_move(Move::Move move) {

    // ASSUMES MOVE IS LEGAL!

    // create new state for new move
    _states.pop();
    auto prevState = _states.top();

    _sideToMove = Piece::Color(!_sideToMove);

    Move::Type type = move_type(move);
    Square from = move_from(move);
    Square to = move_to(move);
    Piece::Color c = _sideToMove;

    // Redo move on board
    if (move_promotion(type)) {
        _board.remove(to);
        _board.remove(from);
        _board.add(merge(c, Piece::PAWN), from);
    } else {
        _board.move(to, from);
    }

    // Handle special cases

    // recover king position
    if (typeOf(_board.pieceAt(from)) == Piece::KING) {
        _kingPos[c] = from;
    }

    // respawn captured piece (careful en passant)
    if (move_capture(type)) {

        if(type == Move::EP_CAPTURE) {
            _board.add(_currState->_lastCaptured, Square(to + (c ? +1 : -1) * 8));
            _enPassantTarget = to;
        }
        else
            _board.add(_currState->_lastCaptured, to);

    } else if (move_castle(type)) {
        // If castling was the case, than also rooks position must be recovered
        if (type == Move::CASTLE_Q) {
            Square rook = c ? D8 : D1;
            _board.move(rook, Square(rook - 3));
        } else {
            Square rook = c ? F8 : F1;
            _board.move(rook, Square(rook + 2));
        }
    }

    // Retrieve state
    _currState = std::move(prevState);
    --_fullMoveCounter;
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
    return _currState->_lastCaptured;
}

Square Position::enPassantTarget() const {
    return _enPassantTarget;
}

Castling Position::castlingRights() const {
    return _currState->_castlingRights;
}

int Position::halfMoveClock() const {
    return _currState->_halfMoveClock;
}

int Position::fullMoveCounter() const {
    return _fullMoveCounter;
}

U64 Position::checkers() const {
    return _currState->_checkers;
}

U64 Position::pinned() const {
    return _currState->_pinned;
}

const Square *Position::kingPos() const {
    return _kingPos;
}

template void Position::generatePseudoLegalMoves<Piece::KING>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::KNIGHT>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::BISHOP>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::QUEEN>(MoveList&);

template void Position::generatePseudoLegalMoves<Piece::ROOK>(MoveList&);
