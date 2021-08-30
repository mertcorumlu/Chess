//
// Created by Mert Çorumlu on 30.08.2021.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "Position.h"

using namespace std;

// Small function to test if number of scanned nodes are correct
unsigned long long search(Position pos, int maxDepth, int currDepth) {

    unsigned long long i = 0;
    MoveList moves;
    moves.reserve(256);
    pos.generateAllLegalMoves(moves);
    if (maxDepth == currDepth) {
        return moves.size();
    }
    for (auto& move : moves) {
        pos.do_move(move);
        i += search(pos, maxDepth, currDepth + 1);
        pos.undo_move(move);
    }

    return i;
}

TEST_CASE("Initial Position") {
    Position p;
    SUBCASE("Depth 1") {
        CHECK(20 == search(p, 1, 1));
    }
    SUBCASE("Depth 2") {
        CHECK(400 == search(p, 2, 1));
    }
    SUBCASE("Depth 3") {
        CHECK(8902 == search(p, 3, 1));
    }
    SUBCASE("Depth 4") {
        CHECK(197281 == search(p, 4, 1));
    }
    SUBCASE("Depth 5") {
        CHECK(4865609 == search(p, 5, 1));
    }
    SUBCASE("Depth 6") {
        CHECK(119060324 == search(p, 6, 1));
    }
}

TEST_CASE("Position 2") {
    Position p("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    SUBCASE("Depth 1") {
        CHECK(48 == search(p, 1, 1));
    }
    SUBCASE("Depth 2") {
        CHECK(2039 == search(p, 2, 1));
    }
    SUBCASE("Depth 3") {
        CHECK(97862 == search(p, 3, 1));
    }
    SUBCASE("Depth 4") {
        CHECK(4085603 == search(p, 4, 1));
    }
    SUBCASE("Depth 5") {
        CHECK(193690690 == search(p, 5, 1));
    }
//    SUBCASE("Depth 6") {
//        CHECK(8031647685 == search(p, 6, 1));
//    }
}
