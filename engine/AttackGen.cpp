//
// Created by Mert Çorumlu on 22.08.2021.
//

#include <iostream>
#include <fstream>
#include <array>
#include "../types.h"

#define DIRECTORY "bin"

using namespace std;

class AttackGen {
public:

    AttackGen();

private:

    // Constants for calculations

    U64 _bishopMasks[64];
    U64 _rookMasks[64];

    // ================= Tables =========================================

    /*
     * 0 => KING
     * 1 => KNIGHT
     * 2 => wPAWN
     * 3 => bPawn
     */
    U64 _nonSlidingTable[5][64];

    // 2^9 * 4 + 2^7 * 12 + 2^6 * 4 + 2^5 * 44 = 2^7 (16 + 12 + 2 + 11)
    U64 _bishopTable[(1 << 7) * 41];
    U32 _bishopOffsets[64];

    // 2^10 * 36 + 2^11 * 24 + 2^12 * 4 = 2^12 (9 + 12 + 4)
    U64 _rookTable[(1 << 12) * 25];
    U32 _rookOffsets[64];


    void _initBishopMasks();
    void _initRookMasks();

    void _initNonSlidingTable();

    template<Piece::Type t>
    void _initSlidingTable();

    void _writeFiles();

    template<Piece::Type t>
    U32 _hash(U64 nonoccupied, U8 index) const;

    template<int dir>
    static U64 _generateSlidingAttacks(U64 piece, U64 nonoccupied);
};

AttackGen::AttackGen() : _bishopMasks(), _rookMasks(), _nonSlidingTable(), _bishopTable(),
                         _bishopOffsets(), _rookTable(), _rookOffsets() {
    _initBishopMasks();
    _initRookMasks();

    _initNonSlidingTable();
    _initSlidingTable<Piece::BISHOP>();
    _initSlidingTable<Piece::ROOK>();

    _writeFiles();
}

void AttackGen::_initBishopMasks() {

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

void AttackGen::_initRookMasks() {

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

void AttackGen::_initNonSlidingTable() {

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

template <Piece::Type t>
void AttackGen::_initSlidingTable() {
    U64 piece = 1L;
    U32 sum = 0;


    for (U8 i = 0; i < 64; ++i, piece <<= 1) {

        U64 mask = 0;

        if constexpr(t == Piece::BISHOP) {
            mask = _bishopMasks[i];
        } else if constexpr(t == Piece::ROOK) {
            mask = _rookMasks[i];
        }

        U16 offset = (1 << _mm_popcnt_u64(mask));

        if constexpr(t == Piece::BISHOP) {
            _bishopOffsets[i] = sum;
        } else if constexpr(t == Piece::ROOK) {
            _rookOffsets[i] = sum;
        }

        sum += offset;

        U16 j = 0;
        U64 n = 0;
        do {

            // There we go
            U64 nonoccupied = ~n;

            if constexpr(t == Piece::BISHOP) {

                _bishopTable[_hash<t>(nonoccupied, i)] =
                        _generateSlidingAttacks<7>(piece, nonoccupied) | _generateSlidingAttacks<9>(piece, nonoccupied) |
                        _generateSlidingAttacks<-7>(piece, nonoccupied) | _generateSlidingAttacks<-9>(piece, nonoccupied);

            } else if constexpr(t == Piece::ROOK) {
                _rookTable[_hash<t>(nonoccupied, i)] =
                        _generateSlidingAttacks<1>(piece, nonoccupied) | _generateSlidingAttacks<-1>(piece, nonoccupied) |
                        _generateSlidingAttacks<8>(piece, nonoccupied) | _generateSlidingAttacks<-8>(piece, nonoccupied);

            }

            n = (n - mask) & mask;
            ++j;
        } while(n);

    }
}

void AttackGen::_writeFiles() {
    ofstream file;
    if (filesystem::exists(DIRECTORY)) {
        filesystem::remove_all(DIRECTORY);
    }

    filesystem::create_directory(DIRECTORY);

    const string files[] = {
            "_nonSlidingTable",
            "_bishopMasks",
            "_rookMasks",
            "_bishopTable",
            "_bishopOffsets",
            "_rookTable",
            "_rookOffsets"
    };

    const char* arrays[] = {
            (char*)_nonSlidingTable,
            (char*)_bishopMasks,
            (char*)_rookMasks,
            (char*)_bishopTable,
            (char*)_bishopOffsets,
            (char*)_rookTable,
            (char*)_rookOffsets
    };

    const U32 sizes[] = {
            64 * 5 * 8,
            64 * 8,
            64 * 8,
            (1 << 7) * 41 * 8,
            64 * 4,
            (1 << 12) * 25 * 8,
            64 * 4
    };

    for (int i = 0; i < 7; ++i) {
        string s = DIRECTORY;
        s.append("/");
        s.append(files[i]);
        s.append(".");
        s.append(DIRECTORY);
        file.open(s, ios::in | ios::out | ios::app);
        file.write(arrays[i], sizes[i]);
        file.close();
    }
}

template<Piece::Type t>
U32 AttackGen::_hash(U64 nonoccupied, U8 index) const {

    if constexpr(t == Piece::BISHOP) {
        return _bishopOffsets[index] + _pext_u64(nonoccupied, _bishopMasks[index]);
    } else if constexpr(t == Piece::ROOK) {
        return _rookOffsets[index] + _pext_u64(nonoccupied, _rookMasks[index]);
    }

}

template<int dir>
U64 AttackGen::_generateSlidingAttacks(U64 piece, U64 nonoccupied) {

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

int main() {
    AttackGen();
}