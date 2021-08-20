//
// Created by Mert Çorumlu on 19.08.2021.
//

#ifndef CHESS_REPRESENTATION_H
#define CHESS_REPRESENTATION_H

#include <memory>
#include <array>
#include <unordered_map>
#include "Piece.h"

using namespace std;

class Representation {
    array<array<shared_ptr<Piece>, 8>, 8> board;
    unordered_map<uint8_t, shared_ptr<Piece>> pieces;
public:
    void add(shared_ptr<Piece>&& piece);
    void remove(uint8_t ID);
    void remove(int x, int y);
    void move(uint8_t ID, int _x, int _y);
    void move(int x, int y, int _x, int _y);
    [[nodiscard]] shared_ptr<Piece> get(uint8_t ID) const;
    [[nodiscard]] shared_ptr<Piece> get(int x, int y) const;
    array<shared_ptr<Piece>, 8>& operator[] (int x);
};


#endif //CHESS_REPRESENTATION_H
