//
// Created by Mert Çorumlu on 22.08.2021.
//

#include<iostream>
#include <array>
//#include "engine/Board.h"

#include "engine/Attack.h"

using namespace std;

void printBoard(uint64_t a) {

    for (int i = 0; i < 8; ++i) {
        uint8_t s = (a & 0xff00000000000000) >> 56;
        cout << 8 - i << "  ";
        for (int j = 0; j < 8; ++j) {
            cout << ((s & 0x1) ? '1' : '.') << "  ";
            s >>= 1;
        }
        cout << endl;
        a <<= 8;
    }

    cout << "   a  b  c  d  e  f  g  h" << endl;
    cout << endl;
}

int main() {

    constexpr Attack a;
    printBoard(a._bishopMasks[7]);
    cout << a._bishopMasks[0];
//    U64 piece = 1;
//    U64 nonoccupied = -1;
//    U64 b = a._generateSlidingAttacks(piece, nonoccupied, 7) | a._generateSlidingAttacks(piece, nonoccupied, -7) |
//    a._generateSlidingAttacks(piece, nonoccupied, 9) | a._generateSlidingAttacks(piece, nonoccupied, -9);
//    Board::printBoard(b);
//    Board::printBoard(a._rookTable[a._hash<false>(-1, 0)]);
//    cout << _mm_popcnt_u64(0b11) << endl;
}
