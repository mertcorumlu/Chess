//
// Created by Mert Çorumlu on 23.08.2021.
//

#ifndef CHESS_ATTACK_H
#define CHESS_ATTACK_H

#include "../types.h"

extern "C" const unsigned char _nonSlidingTableData[];
extern "C" const unsigned char _bishopMasksData[];
extern "C" const unsigned char _rookMasksData[];
extern "C" const unsigned char _bishopTableData[];
extern "C" const unsigned char _rookTableData[];
extern "C" const unsigned char _bishopOffsetsData[];
extern "C" const unsigned char _rookOffsetsData[];
extern "C" const unsigned char _maskBetweenData[];
extern "C" const unsigned char _lineBetweenData[];

class Attack {
public:

    static U64 nonSlidingAttacks(Piece::Type t, Square square, Piece::Color c = Piece::WHITE);
    static U64 slidingAttacks(Piece::Type t, Square Square, U64 occupied);
    static U64 xraySlidingAttacks(Square king, U64 occupied, U64 allies);

private:

    static const U64 *const _nonSlidingTable;
    static const U64 *const _bishopMasks;
    static const U64 *const _rookMasks;
    static const U64 *const _bishopTable;
    static const U64 *const _rookTable;
    static const U32 *const _bishopOffsets;
    static const U32 *const _rookOffsets;
    static const U64 *const _maskBetween;
    static const U64 *const _lineBetween;

    template<Piece::Type t>
    static U32 _hash(U64 occupied, Square Square);
};

#endif //CHESS_ATTACK_H
