//
// Created by Mert Çorumlu on 18.08.2021.
//

#include <limits>
#include "Piece.h"
#include "Board.h"

const wxImage* Piece::sprite;
uint8_t Piece::idCounter = 0;

Piece::Piece(Board& board, unsigned char type, wxPoint pos)
: type{type}, pos{pos}, intPos(pos), canvasPos(pos.x * Board::SQ_SIZE, pos.y * Board::SQ_SIZE),
ID{idCounter++}, board(board), moves(make_shared<vector<wxPoint>>()) {

    int h = isWhite() ? 0 : 1;

    int i;
    switch (neutralize()) {
        case 'k': i = 0; value = std::numeric_limits<double>::infinity(); break;
        case 'q': i = 1; value = 9.50; break;
        case 'b': i = 2; value = 3.33; break;
        case 'n': i = 3; value = 3.05; break;
        case 'r': i = 4; value = 5.63; break;
        case 'p': i = 5; value = 1.00; break;

        default:
            bitmap = nullptr;
            return;
    }

    bitmap = make_unique<wxBitmap>(getPieceFromSprite(i, h));
}

wxImage Piece::getPieceFromSprite(int i, int h) {
    return sprite->GetSubImage(wxRect(i * 210, h * 210, 210, 210))
                    .Scale(Board::SQ_SIZE, Board::SQ_SIZE);
}

bool Piece::isWhite() const {
    return !(type & 0x20);
}

unsigned char Piece::neutralize() const {
    return type | 0x20;
}

bool Piece::isEnemy(const Piece& other) const {
    return (type ^ other.type) & 0x20;
}

void Piece::move(int x, int y) {
    if (!(x == pos.x && y == pos.y)) {
        board.board.move(ID, x , y);
        pos.x = x;
        pos.y = y;
    }
    canvasPos.x = x * Board::SQ_SIZE;
    canvasPos.y = y * Board::SQ_SIZE;
}

void Piece::moveCanvasPos(int x, int y) {
    canvasPos.x = x;
    canvasPos.y = y;
}

const wxPoint& Piece::getPos() const {
    return pos;
}

const wxPoint& Piece::getCanvasPos() const {
    return canvasPos;
}

shared_ptr<vector<wxPoint>> Piece::getMoves() {
    return moves;
}

void Piece::createMoves() {

    moves->clear();

    switch (neutralize()) {
        case 'k':
            kingMoves();
            break;

        case 'q':
            queenMoves();
            break;

        case 'b':
            bishopMoves();
            break;

        case 'n':
            knightMoves();
            break;

        case 'r':
            rockMoves();
            break;

        case 'p':
            pawnMoves();
            break;
    }
}

template<bool Eats, bool Diagonal>
bool Piece::movePossible(int x, int y) const {
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if constexpr(Diagonal) {
        if(!board.board[x][y]) return false;

        if (board.board[x][y]->isEnemy(*this)) return true;

        return false;
    }

    if (board.board[x][y]) {
        if constexpr(!Eats) {
            if (board.board[x][y]->isEnemy(*this)) return false;
        }
        if (!board.board[x][y]->isEnemy(*this)) return false;
    }

    return true;
}

void Piece::kingMoves() {
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if (j == 0 && i == 0) continue;

            int x = j + pos.x, y = i + pos.y;

            if (!movePossible(x, y)) continue;

            moves->emplace_back(x, y);
        }
    }
}

void Piece::queenMoves() {
    int x1 = 1, x2 = -1, y1 = 1, y2 = -1;
    bool dir[8] = {true, true, true, true, true, true, true, true};

    for (int i = 0; i < 7; ++i, ++x1, --x2, ++y1, --y2) {
        tryDir(0, pos.x + x1, pos.y + y1, dir);
        tryDir(1,    pos.x,      pos.y + y1, dir);
        tryDir(2, pos.x + x2, pos.y + y1, dir);
        tryDir(3, pos.x + x2,    pos.y,      dir);
        tryDir(4, pos.x + x2, pos.y + y2, dir);
        tryDir(5,    pos.x,      pos.y + y2, dir);
        tryDir(6, pos.x + x1, pos.y + y2, dir);
        tryDir(7, pos.x + x1,    pos.y,      dir);
    }
}

void Piece::bishopMoves() {
    int x1 = 1, x2 = -1, y1 = 1, y2 = -1;
    bool dir[4] = {true, true, true, true};

    for (int i = 0; i < 7; ++i, ++x1, --x2, ++y1, --y2) {
        tryDir(0, pos.x + x1, pos.y + y1, dir);
        tryDir(1, pos.x + x2, pos.y + y1, dir);
        tryDir(2, pos.x + x2, pos.y + y2, dir);
        tryDir(3, pos.x + x1, pos.y + y2, dir);
    }
}

void Piece::rockMoves() {
    int x1 = 1, x2 = -1, y1 = 1, y2 = -1;
    bool dir[4] = {true, true, true, true};

    for (int i = 0; i < 7; ++i, ++x1, --x2, ++y1, --y2) {
        tryDir(0,    pos.x,      pos.y + y1, dir);
        tryDir(1, pos.x + x2,    pos.y,      dir);
        tryDir(2,    pos.x,      pos.y + y2, dir);
        tryDir(3, pos.x + x1,    pos.y,      dir);
    }
}

void Piece::knightMoves() {

    for (int i = -2; i < 3; ++i) {
        if (i == 0) continue;
        for (int j = -2; j < 3; ++j) {
            if (j == 0 || (i + j) % 2 == 0) continue;
            int x = pos.x + j, y = pos.y + i;

            if (!movePossible(x, y)) continue;

            moves->emplace_back(x, y);
        }
    }

}

void Piece::pawnMoves() {
    int i = isWhite() ? -1 : +1;
    int x = pos.x, y = pos.y + i;

    if (movePossible<false>(x, y)) {
        moves->emplace_back(x, y);

        if (pos.x == intPos.x && pos.y == intPos.y) {
            int _y = pos.y + 2 * i;
            if (movePossible<false>(x, _y)) {
                moves->emplace_back(x, _y);
            }
        }
    }

    if(movePossible<true, true>(x + 1, y))
        moves->emplace_back(x + 1, y);

    if(movePossible<true, true>(x - 1, y))
        moves->emplace_back(x - 1, y);

}

void Piece::tryDir(int i, int x, int y, bool* dir) {

    if (dir[i]) {
        if (movePossible(x, y)) {
            moves->emplace_back(x, y);

            if (board.board[x][y] && board.board[x][y]->isEnemy(*this))
                dir[i] = false;

        } else {
            dir[i] = false;
        }
    }

}

//bool Piece::isCheck(Piece const * king) {
//    return false;
//}
