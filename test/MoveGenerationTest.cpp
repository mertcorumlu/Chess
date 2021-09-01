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

TEST_CASE("Position 3") {
    Position p("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    SUBCASE("Depth 1") {
        CHECK(14 == search(p, 1, 1));
    }
    SUBCASE("Depth 2") {
        CHECK(191 == search(p, 2, 1));
    }
    SUBCASE("Depth 3") {
        CHECK(2812 == search(p, 3, 1));
    }
    SUBCASE("Depth 4") {
        CHECK(43238 == search(p, 4, 1));
    }
    SUBCASE("Depth 5") {
        CHECK(674624 == search(p, 5, 1));
    }
    SUBCASE("Depth 6") {
        CHECK(11030083 == search(p, 6, 1));
    }
    SUBCASE("Depth 7") {
        CHECK(178633661 == search(p, 7, 1));
    }
}

TEST_CASE("Position 4") {
    Position p("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    SUBCASE("Depth 1") {
        CHECK(6 == search(p, 1, 1));
    }
    SUBCASE("Depth 2") {
        CHECK(264 == search(p, 2, 1));
    }
    SUBCASE("Depth 3") {
        CHECK(9467 == search(p, 3, 1));
    }
    SUBCASE("Depth 4") {
        CHECK(422333 == search(p, 4, 1));
    }
    SUBCASE("Depth 5") {
        CHECK(15833292 == search(p, 5, 1));
    }
    SUBCASE("Depth 6") {
        CHECK(706045033 == search(p, 6, 1));
    }
}

TEST_CASE("Position 5") {
    Position p("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    SUBCASE("Depth 1") {
        CHECK(44 == search(p, 1, 1));
    }
    SUBCASE("Depth 2") {
        CHECK(1486 == search(p, 2, 1));
    }
    SUBCASE("Depth 3") {
        CHECK(62379 == search(p, 3, 1));
    }
    SUBCASE("Depth 4") {
        CHECK(2103487 == search(p, 4, 1));
    }
    SUBCASE("Depth 5") {
        CHECK(89941194 == search(p, 5, 1));
    }
}

TEST_CASE("Position 6") {
    Position p("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    SUBCASE("Depth 1") {
        CHECK(46 == search(p, 1, 1));
    }
    SUBCASE("Depth 2") {
        CHECK(2079 == search(p, 2, 1));
    }
    SUBCASE("Depth 3") {
        CHECK(89890 == search(p, 3, 1));
    }
    SUBCASE("Depth 4") {
        CHECK(3894594 == search(p, 4, 1));
    }
    SUBCASE("Depth 5") {
        CHECK(164075551 == search(p, 5, 1));
    }
}
