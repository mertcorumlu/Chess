//
// Created by Mert Çorumlu on 24.08.2021.
//

#ifndef CHESS_POSITION_H
#define CHESS_POSITION_H

#include <vector>
#include <stack>
#include "../types.h"
#include "Board.h"

constexpr long scores[] {
    0, 0, 1, 1, 3, 3, 3, 3, 5, 5, 9, 9, 0, 0
};

class Position {
public:

//    Position(Board board, Piece::Color sideToMove, Piece::Piece lastCaptured,
//             Square enPassantTarget, Castling castlingRights, int halfMoveCLock,
//             int fullMoveCounter, Square kingPos[2]);

    Position(string&& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    U64 squareAttackedBy(Square square, Piece::Color attacker = Piece::WHITE);
    U64 squareAttackedBy(Square square, U64 occupied, Piece::Color attacker = Piece::WHITE);

    template <Piece::Type t>
    void generatePseudoLegalMoves(MoveList& _moves);

    void generateAllLegalMoves(MoveList& _moves);

    void do_move(Move::Move move);
    void undo_move(Move::Move move);

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

    long score() const;

    const Square *kingPos() const;

private:
    struct State {
        Piece::Piece _lastCaptured;
        Castling _castlingRights;
        U64 _checkers;
        U64 _pinned;
        int _halfMoveClock;
    };
    Board _board;
    Piece::Color _sideToMove;
    Square _enPassantTarget;
    int _fullMoveCounter;
    Square _kingPos[2];
    stack<shared_ptr<State>> _states;
    shared_ptr<State> _currState;
    long _score = 0;

    void _findCheckers();
    void _findPinned();
    bool _isLegal(const Move::Move& move);

};

std::ostream& operator<<(std::ostream &strm, const Position &a);
std::ostream& operator<<(std::ostream &strm, const MoveList &a);


#endif //CHESS_POSITION_H
