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
    Position p;//("8/2p5/8/KP1p2kr/5pP1/8/4P3/6R1 b - g3 0 3");
    p.do_move(make_move(A2, A7, Move::Type::CAPTURE));
    cout << p.score() << endl;
}
