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

    Position() = default;

    U64 squareAttackedBy(U8 square, Piece::Color owner = _sideToMove);
    float evaluate();

    template <Piece::Type t>
    void generatePseudoLegalMoves();

    template <Piece::Type t>
    void generateLegalMoves();

    void generateAllPseudoLegalMoves();

    void generateAllLegalMoves();

private:
    const Board _board;
    const Piece::Color _sideToMove;
    const Piece::Piece _lastCaptured;
    const U8 _enPassantTarget;
    const Castling _castlingRights;
    const int _halfMoveCLock;
    const int _fullMoveCounter;
    MoveList _moves;
    const U64 _checkers;
    const U64 _checkBlockers;
    const U8 _kingPos[2];
};


#endif //CHESS_POSITION_H
