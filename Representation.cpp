//
// Created by Mert Çorumlu on 19.08.2021.
//

#include "Representation.h"

using namespace std;

void Representation::add(shared_ptr<Piece>&& piece) {
    int x = piece->getPos().x, y = piece->getPos().y;
    board[x][y] = piece;
    pieces.insert({piece->ID, board[x][y]});
}

void Representation::remove(uint8_t ID) {
    if (pieces.find(ID) != pieces.end()) {
        auto tmp = pieces.at(ID);
        pieces.erase(ID);
        board[tmp->getPos().x][tmp->getPos().y].reset();
    }
}

void Representation::remove(int x, int y) {
    if (board[x][y]) {
        uint8_t id = board[x][y]->ID;
        board[x][y].reset();
        pieces.erase(id);
    }
}

void Representation::move(uint8_t ID, int _x, int _y) {
    if (pieces.find(ID) != pieces.end()) {
        auto& tmp = pieces.at(ID);

        if (board[_x][_y])
            pieces.erase(board[_x][_y]->ID);

        board[_x][_y] = std::move(board[tmp->getPos().x][tmp->getPos().y]);
    }
}

void Representation::move(int x, int y, int _x, int _y) {

    if (board[x][y]) {
        auto& tmp = board[x][y];
        if (board[_x][_y])
            pieces.erase(board[_x][_y]->ID);
        board[_x][_y] = std::move(board[tmp->getPos().x][tmp->getPos().y]);
    }

}

shared_ptr<Piece> Representation::get(uint8_t ID) const {
    if (pieces.find(ID) != pieces.end())
        return pieces.at(ID);

    return nullptr;
}

shared_ptr<Piece> Representation::get(int x, int y) const {
    return board[x][y];
}

array<shared_ptr<Piece>, 8>& Representation::operator[] (int x) {
    return board[x];
}