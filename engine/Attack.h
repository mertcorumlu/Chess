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

class Attack {
public:

    static U64 nonSlidingAttacks(Piece::Type t, U8 index, Piece::Color c = Piece::White);
    static U64 slidingAttacks(Piece::Type t, U8 index, U64 nonoccupied);

private:

    static const U64 *const _nonSlidingTable;
    static const U64 *const _bishopMasks;
    static const U64 *const _rookMasks;
    static const U64 *const _bishopTable;
    static const U64 *const _rookTable;
    static const U32 *const _bishopOffsets;
    static const U32 *const _rookOffsets;

    template<Piece::Type t>
    static U32 _hash(U64 nonoccupied, U8 index);
};

#endif //CHESS_ATTACK_H
