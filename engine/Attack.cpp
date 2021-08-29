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
INCBIN(_maskBetween, "bin/_maskBetween.bin");
INCBIN(_lineBetween, "bin/_lineBetween.bin");

const U64 *const Attack::_nonSlidingTable = (U64 *) _nonSlidingTableData;;
const U64 *const Attack::_bishopMasks = (U64 *) _bishopMasksData;
const U64 *const Attack::_rookMasks = (U64 *) _rookMasksData;
const U64 *const Attack::_bishopTable = (U64 *) _bishopTableData;
const U64 *const Attack::_rookTable = (U64 *) _rookTableData;
const U32 *const Attack::_bishopOffsets = (U32 *) _bishopOffsetsData;
const U32 *const Attack::_rookOffsets = (U32 *) _rookOffsetsData;
const U64 *const Attack::_maskBetween = (U64 *) _maskBetweenData;
const U64 *const Attack::_lineBetween = (U64 *) _lineBetweenData;

U64 Attack::nonSlidingAttacks(Piece::Type t, Square square, Piece::Color c) {

    switch (t) {
        case Piece::KING: return _nonSlidingTable[square];
        case Piece::KNIGHT: return _nonSlidingTable[64 * 1 + square];
        case Piece::PAWN: return _nonSlidingTable[(2 + c) * 64 + square];
        default: throw std::invalid_argument("not a non-sliding piece!");
    };

}

U64 Attack::slidingAttacks(Piece::Type t, Square square, U64 occupied) {
    switch (t) {
        case Piece::BISHOP:
            return _bishopTable[_hash<Piece::BISHOP>(occupied, square)];
        case Piece::ROOK:
            return _rookTable[_hash<Piece::ROOK>(occupied, square)];
        case Piece::QUEEN:
            return _bishopTable[_hash<Piece::BISHOP>(occupied, square)] | _rookTable[_hash<Piece::ROOK>(occupied, square)];
        default:
            throw std::invalid_argument("not a sliding peace");
    }
}

template <Piece::Type t>
U64 Attack::xraySlidingAttacks(Square king, U64 occupied, U64 allies) {

    // Same idea as slidingAttacks, but will remove attackers for king and recalculate attackers,
    // so we get x-rayed attacks on king
    U64 attackers = slidingAttacks(t, king, occupied);
    allies &= attackers; // find corresponding allies

    // remove allies from occupancy and find attacks without ally blockers
    // than remove non-occupied attacks
    return attackers ^ slidingAttacks(t, king, occupied ^ allies);
}

U64 Attack::maskBetween(Square from, Square to) {
    return _maskBetween[to + from * 64];
}

U64 Attack::lineBetween(Square from, Square to) {
    return _lineBetween[from + to * 64];
}

template <Piece::Type t>
U32 Attack::_hash(U64 occupied, Square square) {

    if constexpr(t == Piece::BISHOP) {
        return _bishopOffsets[square] + _pext_u64(occupied, _bishopMasks[square]);
    } else if constexpr(t == Piece::ROOK) {
        return _rookOffsets[square] + _pext_u64(occupied, _rookMasks[square]);
    }

}

// Template explicit instantiation for library
template U32 Attack::_hash<Piece::BISHOP>(U64, Square);
template U32 Attack::_hash<Piece::ROOK>(U64, Square);

template U64 Attack::xraySlidingAttacks<Piece::BISHOP>(Square king, U64 occupied, U64 allies);
template U64 Attack::xraySlidingAttacks<Piece::ROOK>(Square king, U64 occupied, U64 allies);