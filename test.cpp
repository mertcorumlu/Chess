//
// Created by Mert Çorumlu on 22.08.2021.
//

#include<iostream>
#include <array>

#include "Attack.h"
#include "Board.h"
#include "Position.h"

using namespace std;

unsigned long long search(Position pos, int maxDepth, int currDepth) {

    unsigned long long i = 0;
    MoveList moves;
    moves.reserve(256);
    pos.generateAllLegalMoves(moves);
    if (maxDepth == currDepth) {
        return moves.size();
    }
    for (auto& move : moves) {
        pos.do_move(move);
        i += search(pos, maxDepth, currDepth + 1);
        pos.undo_move(move);
    }

    return i;
}

int main() {
    Position p("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    MoveList moves;
    moves.reserve(256);

//    cout << p._isLegal(make_move(F6,F5, Move::QUIET)) << endl;

//    cout << p << endl;
//    p.do_move(make_move(A5, B6, Move::EP_CAPTURE));
//    cout << p << endl;
//    p.undo_move(make_move(A5, B6, Move::EP_CAPTURE));
//    cout << p << endl;
//    p.do_move(make_move(A1, A4, Move::QUIET));
//    p.undo_move(make_move(A1, A4, Move::QUIET));

    p.generateAllLegalMoves(moves);
    for (auto& move : moves) {
        p.do_move(move);
        cout << move_from(move) << move_to(move) << ": " << search(p, 7, 2) << endl;
//        cout << move_from(move) << move_to(move) << ": 1" << endl;
        p.undo_move(move);;
    }

//    MoveList list;
//    list.reserve(256);
//    p.generateAllLegalMoves(list);
//    cout << list << endl;
//    cout << list.size() << endl;

}
