//
// Created by Mert Çorumlu on 22.08.2021.
//

#include "Attack.h"

constexpr Attack::Attack() : _bishopMasks(), _rookMasks(), _nonSlidingTable(), _bishopTable(),
                             _bishopStartIndexes(), _rookTable(), _rookStartIndexes() {
    _initBishopMasks();
    _initRookMasks();

    _initNonSlidingTable();
    _initBishopTable();
    _initRookTable();
}

constexpr void Attack::_initBishopMasks() {

    U64 mask = 1;


    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x, mask <<= 1) {
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
                ha >>= 8 * off;

            _bishopMasks[x + y * 8] = (ah | ha) & ~mask & ~AFile & ~HFile & ~rank1 & ~rank8;
        }
    }

}

constexpr void Attack::_initRookMasks() {

    U64 mask = 1;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x, mask <<= 1) {
            U64 v = AFile & ~rank1 & ~rank8;
            U64 h = rank1 & ~AFile & ~HFile;

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
            _nonSlidingTable[1][index] = (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);

            // Pawn Attacks
            U64 pawn = piece;
            _nonSlidingTable[2][index] = ((pawn << 7) & ~HFile) | ((pawn << 9) & ~AFile);
            _nonSlidingTable[3][index] = ((pawn >> 7) & ~AFile) | ((pawn >> 9) & ~HFile);

        }
    }
}

constexpr void Attack::_initBishopTable() {

    U64 piece = 1L;
    U32 sum = 0;


    for (U8 i = 0; i < 64; ++i, piece <<= 1) {
        U64 mask = _bishopMasks[i];
        U8 bits = _bishopIndexBits[i];
        U16 index = (1 << bits);

        _bishopStartIndexes[i] = sum;
        sum += index;

        for (U16 j = 0; j < index; ++j) {

            // There we go
            U64 nonoccupied = ~_traverseSubsets(j, bits, mask);

            _bishopTable[_hash<true>(nonoccupied, i)] =
                    _generateSlidingAttacks<7>(piece, nonoccupied) | _generateSlidingAttacks<9>(piece, nonoccupied) |
                    _generateSlidingAttacks<-7>(piece, nonoccupied) | _generateSlidingAttacks<-9>(piece, nonoccupied);
        }

    }

}

constexpr void Attack::_initRookTable() {

    U64 piece = 1L;
    U32 sum = 0;


    for (U8 i = 0; i < 64; ++i, piece <<= 1) {
        U64 mask = _rookMasks[i];
        U8 bits = _rookIndexBits[i];
        U16 index = (1 << bits);

        _rookStartIndexes[i] = sum;
        sum += index;

        for (U16 j = 0; j < index; ++j) {

            // There we go
            U64 nonoccupied = ~_traverseSubsets(j, bits, mask);

            _rookTable[j] = 1;
//                    _generateSlidingAttacks<1>(piece, nonoccupied) | _generateSlidingAttacks<-1>(piece, nonoccupied) |
//                    _generateSlidingAttacks<8>(piece, nonoccupied) | _generateSlidingAttacks<-8>(piece, nonoccupied);
        }

    }
}

template<typename F>
constexpr U64 Attack::_traverseSubsets(U64 bitMask) {
    U64 occ = 0L;

    auto tmp = static_cast<long long>(bitMask);

    for (U8 i = 0; i < bitCount; ++i) {
        U8 lsb = __builtin_ffsll(tmp) - 1;

        auto bit = 1L << lsb;

        if (index & (1 << i)) occ |= bit;

        tmp &= ~bit;
    }

    return occ;
}

template<bool bishop>
constexpr U32 Attack::_hash(U64 nonoccupied, U8 index) const {

    if constexpr(bishop) {
        nonoccupied &= _bishopMasks[index];
        nonoccupied *= _bishopMagics[index];
        nonoccupied >>= (64 - _bishopIndexBits[index]);
        return _bishopStartIndexes[index] + nonoccupied;
    } else {
        nonoccupied &= _rookMasks[index];
        nonoccupied *= _rookMagics[index];
        nonoccupied >>= (64 - _rookIndexBits[index]);
        return _rookStartIndexes[index] + nonoccupied;
    }

}

template<int dir>
constexpr U64 Attack::_generateSlidingAttacks(U64 piece, U64 nonoccupied) {

    U64 mask = -1;

    if constexpr(dir == 8 | dir == -8) mask = -1;
    else if constexpr(dir == 1 || dir == 9 || dir == -7) mask = ~AFile;
    else if constexpr(dir == -1 || dir == -9 || dir == 7) mask = ~HFile;

    if constexpr(dir > 0) {
        nonoccupied &= mask;
        piece |= nonoccupied & (piece << dir);
        nonoccupied &= (nonoccupied << dir);
        piece |= nonoccupied & (piece << dir * 2);
        nonoccupied &= (nonoccupied << dir * 2);
        piece |= nonoccupied & (piece << dir * 4);
        return (piece << dir) & mask;
    } else {
        nonoccupied &= mask;
        piece |= nonoccupied & (piece >> -dir);
        nonoccupied &= (nonoccupied >> -dir);
        piece |= nonoccupied & (piece >> -dir * 2);
        nonoccupied &= (nonoccupied >> -dir * 2);
        piece |= nonoccupied & (piece >> -dir * 4);
        return (piece >> -dir) & mask;
    }

}