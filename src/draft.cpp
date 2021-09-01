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
    Position p("8/2p5/8/KP1p2kr/5pP1/8/4P3/6R1 b - g3 0 3");

    MoveList moves;
    moves.reserve(256);

//    cout << p._isLegal(make_move(E8,C8, Move::CASTLE_Q)) << endl;

//    cout << p << endl;
//    p.do_move(make_move(G2, H1, Move::PROMOCAP_B));
//    cout << p << endl;
//    p.undo_move(make_move(G2, H1, Move::PROMOCAP_B));
//    cout << p << endl;
//    p.do_move(make_move(A1, A4, Move::QUIET));
//    p.undo_move(make_move(A1, A4, Move::QUIET));

    p.generateAllLegalMoves(moves);
//    U64 occ = p.board().getOccupied();
//    Square kingpos[2] = {p.kingPos()[0], p.kingPos()[1]};
//    for (const auto& move : moves) {
//        p.do_move(move);
////        cout << move_from(move) << move_to(move) << ": " << search(p, 2, 2) << endl;
//        cout << move_from(move) << move_to(move) << ": 1" << endl;
////        cout << move << endl;
//        p.undo_move(move);;
////        if (occ != p.board().getOccupied()) {
////            cout << move << endl;
////        }
//    }

//    MoveList list;
//    list.reserve(256);
//    p.generateAllLegalMoves(list);
//    cout << list << endl;
//    cout << list.size() << endl;

}
