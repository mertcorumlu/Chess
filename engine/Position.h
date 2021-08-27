//
// Created by Mert Çorumlu on 24.08.2021.
//

#ifndef CHESS_POSITION_H
#define CHESS_POSITION_H

#include <vector>
#include "../types.h"
#include "Board.h"

class Position {
public:

    Position(Board &&board, Piece::Color sideToMove, Piece::Piece lastCaptured,
             Square enPassantTarget, Castling castlingRights, int halfMoveCLock,
             int fullMoveCounter, Square kingPos[2]);

    Position(string&& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    U64 squareAttackedBy(Square square, Piece::Color attacker = Piece::WHITE);

    template <Piece::Type t>
    void generatePseudoLegalMoves();

    template <Piece::Type t>
    void generateLegalMoves();

    void generateAllPseudoLegalMoves();

    void generateAllLegalMoves();

    Position move(Square from, Square to);


    // getters
    const Board &board() const;

    Piece::Color sideToMove() const;

    Piece::Piece lastCaptured() const;

    Square enPassantTarget() const;

    Castling castlingRights() const;

    int halfMoveClock() const;

    int fullMoveCounter() const;

    const MoveList &moves() const;

    U64 checkers() const;

    U64 pinned() const;

    const Square *kingPos() const;

private:
    Board _board;
    Piece::Color _sideToMove;
    Piece::Piece _lastCaptured;
    Square _enPassantTarget;
    Castling _castlingRights;
    int _halfMoveClock;
    int _fullMoveCounter;
    MoveList _moves;
    U64 _checkers;
    U64 _pinned;
    Square _kingPos[2];

    void _findCheckers();
    void _findPinned();
};

std::ostream& operator<<(std::ostream &strm, const Position &a);


#endif //CHESS_POSITION_H
