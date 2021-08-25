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

U64 Attack::nonSlidingAttacks(Piece::Type t, U8 index, Piece::Color c) {

    switch (t) {
        case Piece::King: return _nonSlidingTable[index];
        case Piece::Knight: return _nonSlidingTable[64 * 1 + index];
        case Piece::Pawn: return _nonSlidingTable[(2 + c) * 64 + index];
        default: throw std::invalid_argument("not a non-sliding piece!");
    };

}

U64 Attack::slidingAttacks(Piece::Type t, U8 index, U64 nonoccupied) {
    switch (t) {
        case Piece::Bishop:
            return _bishopTable[_hash<Piece::Bishop>(nonoccupied, index)];
        case Piece::Rook:
            return _rookTable[_hash<Piece::Rook>(nonoccupied, index)];
        case Piece::Queen:
            return _bishopTable[_hash<Piece::Bishop>(nonoccupied, index)] | _rookTable[_hash<Piece::Rook>(nonoccupied, index)];
        default:
            throw std::invalid_argument("not a sliding peace");
    }
}

template <Piece::Type t>
U32 Attack::_hash(U64 nonoccupied, U8 index) {

    if constexpr(t == Piece::Bishop) {
        return _bishopOffsets[index] + _pext_u64(nonoccupied, _bishopMasks[index]);
    } else if constexpr(t == Piece::Rook) {
        return _rookOffsets[index] + _pext_u64(nonoccupied, _rookMasks[index]);
    }

}

// Template explicit instantiation for library
template U32 Attack::_hash<Piece::Bishop>(U64 nonoccupied, U8 index);
template U32 Attack::_hash<Piece::Rook>(U64 nonoccupied, U8 index);