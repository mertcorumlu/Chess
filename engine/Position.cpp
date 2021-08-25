//
// Created by Mert Çorumlu on 24.08.2021.
//

#include "Position.h"
#include "Attack.h"

U64 Position::squareAttackedBy(U8 square, Piece::Color owner) {
    U64 attackers = 0;

    // Place that square the piece and see squares that attacks corresponds enemy occupancy;
    attackers |= Attack::nonSlidingAttacks(Piece::Knight, square);
    attackers |= Attack::nonSlidingAttacks(Piece::King, square);
    attackers |= Attack::nonSlidingAttacks(Piece::Pawn, square, owner); // Exception, place out pawn
    attackers |= Attack::slidingAttacks(Piece::Queen, square, ~_board.getOccupied());
    return attackers & (owner ? _board.getWhite() : _board.getBlack());
}


// TODO move generation could be really better

template <Piece::Type t>
void Position::generatePseudoLegalMoves() {

    Piece::Color c = _sideToMove;
    U64 allies = c ? _board.getBlack() : _board.getWhite();
    U64 enemies = c ? _board.getWhite() : _board.getBlack();
    U64 nonoccupied = ~_board.getOccupied();
    U64 pieces;

    if constexpr(t == Piece::King) pieces = _kingPos[c];
    else if constexpr(t == Piece::Knight) pieces = _board.getKnights() & allies;
    else if constexpr(t == Piece::Rook) pieces = _board.getRooks() & allies;
    else if constexpr(t == Piece::Bishop) pieces = _board.getBishops() & allies;
    else if constexpr(t == Piece::Queen) pieces = _board.getQueens() & allies;

    while(pieces) {
        U8 from = pop_lsb(pieces);

        U64 attacks;

        if constexpr(t == Piece::King || t == Piece::Knight )
            attacks = Attack::nonSlidingAttacks(t, from);
        else if constexpr(t == Piece::Rook || t == Piece::Bishop || t == Piece::Queen)
            attacks = Attack::slidingAttacks(t, from, nonoccupied);

        // We dont want attacks on friendly pieces
        attacks &= enemies;

        while(attacks) {
            U8 to = pop_lsb(attacks);
            _moves.push_back(make_move(from, to, Move::Normal));
        }
    }

    // Check also castling moves for king
    if constexpr(t == Piece::King) {
        Castling cColor = c ? Castling::BLACK : Castling::WHITE;
        if (_castlingRights & cColor) {
            U8 from = c ? 4 : 60 ;

            if (_castlingRights & Castling::QUEEN & cColor) {
                U64 mask = c ? 0xeULL : 0xeULL << 56;

                // There is a piece between
                if (_board.getOccupied() & mask) return;

                U8 to;
                if (!squareAttackedBy(pop_lsb(mask)) && !squareAttackedBy(to = pop_lsb(mask)) && squareAttackedBy(pop_lsb(mask))) {
                    _moves.push_back(make_move(from, to, Move::Castling));
                }
            } else {

                U64 mask = c ? 0x60ULL : 0x60ULL << 56;

                // There is a piece between
                if (_board.getOccupied() & mask) return;

                U8 to;
                if (!squareAttackedBy(pop_lsb(mask)) && !squareAttackedBy(to = pop_lsb(mask))) {
                    _moves.push_back(make_move(from, to, Move::Castling));
                }

            }
        }
    }


}

template <>
void Position::generatePseudoLegalMoves<Piece::Pawn>() {

    Piece::Color c = _sideToMove;
    U64 enemies = c ? _board.getWhite() : _board.getBlack();
    U64 occupied = _board.getOccupied();
    U64 pieces = _board.getPawns();

    // Single push
    U64 single = (c ? (pieces >> 8) : (pieces << 8)) & occupied;

    // Double push
    U64 doubles = single & (c ? (0xffL << 40) : (0xffL << 16));
    doubles = (c ? (doubles >> 8) : (doubles << 8)) & occupied;

    while(single) {
        U8 index = pop_lsb(single);
        _moves.push_back(make_move(c ? (index << 8) : (index >> 8), index, Move::Normal));
    }

    while(doubles) {
        U8 index = pop_lsb(doubles);
        _moves.push_back(make_move(c ? (index << 16) : (index >> 16), index, Move::Normal));
    }


    // Attacks
    while(pieces) {
        U8 from = pop_lsb(pieces);
        U64 attacks = Attack::nonSlidingAttacks(Piece::Pawn, from, c) & enemies;

        while(attacks) {
            U8 to = pop_lsb(attacks);
            _moves.push_back(make_move(from, to, Move::Normal));
        }

    }

    // En Passant
    U64 enpassant = Attack::nonSlidingAttacks(Piece::Pawn, _enPassantTarget, Piece::Color(!c));

    while(enpassant) {
        U8 index = pop_lsb(enpassant);
        _moves.push_back(make_move(index, _enPassantTarget, Move::En_Passant));
    }


}

template <Piece::Type t>
void Position::generateLegalMoves() {

}

template <>
void Position::generateLegalMoves<Piece::Pawn>() {

}

void Position::generateAllPseudoLegalMoves() {

}

void Position::generateAllLegalMoves() {

    // for single checker try to capture checker

    if (_checkers) {

        U64 checkers = _checkers;

        // Single check
        if (_checkers & (_checkers - 1)) {
            U8 checker = pop_lsb(checkers);
            U64 attackers = squareAttackedBy(checker, Piece::Color(!_sideToMove));

            while (attackers) {
                U8 from = pop_lsb(attackers);
                _moves.push_back(make_move(from, checker, Move::Normal));
            }
        }

        return;
    } else {
        // QUIET moves
        generateLegalMoves<Piece::Knight>();
        generateLegalMoves<Piece::Pawn>();
        generateLegalMoves<Piece::Bishop>();
        generateLegalMoves<Piece::Rook>();
        generateLegalMoves<Piece::Queen>();

    }
    generateLegalMoves<Piece::King>();
}
