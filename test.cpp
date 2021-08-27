//
// Created by Mert Çorumlu on 22.08.2021.
//

#include<iostream>
#include <array>

#include "Attack.h"
#include "Board.h"
#include "Position.h"

using namespace std;

int main() {

    Position p("6b//p/////R3K2R w KQkq - 0 1");
    p.generatePseudoLegalMoves<Piece::KING>();
    cout << p << endl;
//    Attack a;
//    Board::printBoard(a.slidingAttacks(Queen, 11, -1));
//    Board b;
//    cout << (int) b.pieceAt(0,0) << endl;

}
