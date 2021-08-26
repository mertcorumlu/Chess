//
// Created by Mert Çorumlu on 23.08.2021.
//

#include <stdexcept>

#include "Attack.h"
#include "../incbin.h"

#pragma clang diagnostic ignored "-Wmacro-redefined"
#define INCBIN_PREFIX
INCBIN(_nonSlidingTable, "bin/_nonSlidingTable.bin");
INCBIN(_bishopMasks, "bin/_bishopMasks.bin");
INCBIN(_rookMasks, "bin/_rookMasks.bin");
INCBIN(_bishopTable, "bin/_bishopTable.bin");
INCBIN(_rookTable, "bin/_rookTable.bin");
INCBIN(_bishopOffsets, "bin/_bishopOffsets.bin");
INCBIN(_rookOffsets, "bin/_rookOffsets.bin");

const U64 *const Attack::_nonSlidingTable = (U64 *) _nonSlidingTableData;;
const U64 *const Attack::_bishopMasks = (U64 *) _bishopMasksData;
const U64 *const Attack::_rookMasks = (U64 *) _rookMasksData;
const U64 *const Attack::_bishopTable = (U64 *) _bishopTableData;
const U64 *const Attack::_rookTable = (U64 *) _rookTableData;
const U32 *const Attack::_bishopOffsets = (U32 *) _bishopOffsetsData;
const U32 *const Attack::_rookOffsets = (U32 *) _rookOffsetsData;

U64 Attack::nonSlidingAttacks(Piece::Type t, Square square, Piece::Color c) {

    switch (t) {
        case Piece::KING: return _nonSlidingTable[square];
        case Piece::KNIGHT: return _nonSlidingTable[64 * 1 + square];
        case Piece::PAWN: return _nonSlidingTable[(2 + c) * 64 + square];
        default: throw std::invalid_argument("not a non-sliding piece!");
    };

}

U64 Attack::slidingAttacks(Piece::Type t, Square square, U64 nonoccupied) {
    switch (t) {
        case Piece::BISHOP:
            return _bishopTable[_hash<Piece::BISHOP>(nonoccupied, square)];
        case Piece::ROOK:
            return _rookTable[_hash<Piece::ROOK>(nonoccupied, square)];
        case Piece::QUEEN:
            return _bishopTable[_hash<Piece::BISHOP>(nonoccupied, square)] | _rookTable[_hash<Piece::ROOK>(nonoccupied, square)];
        default:
            throw std::invalid_argument("not a sliding peace");
    }
}

template <Piece::Type t>
U32 Attack::_hash(U64 nonoccupied, Square square) {

    if constexpr(t == Piece::BISHOP) {
        return _bishopOffsets[square] + _pext_u64(nonoccupied, _bishopMasks[square]);
    } else if constexpr(t == Piece::ROOK) {
        return _rookOffsets[square] + _pext_u64(nonoccupied, _rookMasks[square]);
    }

}

// Template explicit instantiation for library
template U32 Attack::_hash<Piece::BISHOP>(U64, Square);
template U32 Attack::_hash<Piece::ROOK>(U64, Square);