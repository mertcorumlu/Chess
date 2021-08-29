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

    Position p("3b4/4kp2/1p6/6P1/8/8/2PK1B2/8 w - - 0 1");
    cout << p << endl;

    p.do_move(make_move(F2, B6, Move::Type::NORMAL));

    cout << p << endl;
    p.do_move(make_move(D8, B6, Move::Type::NORMAL));

    cout << p << endl;

//    MoveList list;
//    list.reserve(256);
//    p.generateAllLegalMoves(list);
//    cout << list << endl;
//    cout << list.size() << endl;

}
