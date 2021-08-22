//
// Created by Mert Çorumlu on 22.08.2021.
//

#include "Attack.h"

constexpr Attack::Attack() : _bishopMasks(), _rookMasks(), _nonSlidingTable(), _bishopTable(), _rookTable() {
    _initBishopMasks();
    _initRookMasks();

    _initNonSlidingTable();
//    _initBishopTable();
//    _initRookTable();
}

constexpr void Attack::_initBishopMasks() {

    U64 mask = 1;


    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x, mask<<=1) {
            U64 ah = a1h8_diagonal;
            U64 ha = h1a8_antidiagonal;

            int off = x - y;

            if (off < 0)
                ah <<= 8 * -off;
            else
                ah >>= 8 * off;

            off = 7 - x - y;

            if (off < 0)
                ha <<= 8 * -off;
            else
                ha >>= 8 *  off;

            _bishopMasks[x + y * 8] = (ah | ha) & ~mask & ~AFile & ~HFile & ~rank1 & ~rank8;
        }
    }

}

constexpr void Attack::_initRookMasks() {

    U64 mask = 1;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x, mask<<=1) {
            U64 v = AFile & ~rank1 & ~rank8;
            U64 h = rank1 & ~AFile & ~HFile ;

            v <<= x;
            h <<= 8 * y;

            _rookMasks[x + y * 8] = (v | h) & ~mask;
        }
    }
}

constexpr void Attack::_initNonSlidingTable() {

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int index = x + 8 * y;

            U64 piece = 1L << index;

            // King Attacks
            U64 king = piece;
            U64 l = (piece >> 1) & ~GFile;
            U64 r = (piece << 1) & ~AFile;
            U64 attacks = l | r;
            king |= attacks;
            U64 u = king >> 8;
            U64 d = king << 8;
            _nonSlidingTable[0][index] = attacks | u | d;

            // Knight Attacks
            U64 knight = piece;
            U64 l1 = (knight >> 1) & ~HFile;
            U64 l2 = (knight >> 2) & ~(GFile | HFile);
            U64 r1 = (knight << 1) & ~AFile;
            U64 r2 = (knight << 2) & ~(AFile | BFile);
            U64 h1 = l1 | r1;
            U64 h2 = l2 | r2;
            _nonSlidingTable[1][index] = (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);

            // Pawn Attacks
            U64 pawn = piece;
            _nonSlidingTable[2][index] = ((pawn << 7) & ~HFile) | ((pawn << 9) & ~AFile);
            _nonSlidingTable[3][index] = ((pawn >> 7) & ~AFile) | ((pawn >> 9) & ~HFile);

        }
    }
}

constexpr void Attack::_initBishopTable() {

}

constexpr void Attack::_initRookTable() {

}
