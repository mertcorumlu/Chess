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

    Position p("4r3///4b3//4R3//4K3 w Kkq - 0 1");
    p.generatePseudoLegalMoves<Piece::KING>();
    cout << p.board() << endl;
    bin(Attack::xraySlidingAttacks(p.kingPos()[0], p.board().getOccupied(), p.board().getWhite()));


//    Board b("/4R/4r/1b/2B1R//RrR1K1R/");
//    cout << b << endl;

}
