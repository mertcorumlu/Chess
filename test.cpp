//
// Created by Mert Çorumlu on 22.08.2021.
//

#include<iostream>
#include "engine/Attack.h"
#include "engine/Board.h"

using namespace std;

int main() {

    constexpr Attack a;
    Board::printBoard(a.get(10));
}
