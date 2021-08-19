//
// Created by Mert Çorumlu on 18.08.2021.
//

#include "Piece.h"
#include "Board.h"

Piece::Piece(char type, wxPoint pos) : type{type}, pos{pos}, initialPos(pos), canvasPos(pos.x * SQUARE_SIZE, pos.y * SQUARE_SIZE){

    int h = islower(type) ? 1 : 0;

    type = tolower(type, std::locale());
    switch (type) {
        case 'k': bitmap = new wxBitmap(getPieceFromSprite(0, h));
        break;

        case 'q': bitmap = new wxBitmap(getPieceFromSprite(1, h));
        break;

        case 'b': bitmap = new wxBitmap(getPieceFromSprite(2, h));
        break;

        case 'n': bitmap = new wxBitmap(getPieceFromSprite(3, h));
        break;

        case 'r': bitmap = new wxBitmap(getPieceFromSprite(4, h));
        break;

        case 'p': bitmap = new wxBitmap(getPieceFromSprite(5, h));
        break;

        default: bitmap = nullptr;
    }

}

wxImage Piece::getPieceFromSprite(int i, int j) {
    return img->GetSubImage(wxRect(i * 210, j * 210, 210, 210)).Scale(SQUARE_SIZE,SQUARE_SIZE);
}

bool Piece::isWhite() const {
    return !islower(type);
}

bool Piece::isEnemy(const Piece & other) const {
    return (!isWhite() && other.isWhite()) || (!other.isWhite() && isWhite());
}

template<bool Eats, bool Diagonal>
bool Piece::movePossible(int x, int y, const Board::arr_type& pieces) const {
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if constexpr(Diagonal) {
        if(!pieces[x][y]) return false;

        if (pieces[x][y]->isEnemy(*this)) return true;

        return false;
    }

    if (pieces[x][y]) {
        if constexpr(!Eats) {
            if (pieces[x][y]->isEnemy(*this)) return false;
        }
        if (!pieces[x][y]->isEnemy(*this)) return false;
    }

    return true;
}

std::unique_ptr<std::vector<wxPoint>> Piece::getPossibleMoves(const Board::arr_type& pieces) {
    auto moves = std::make_unique<std::vector<wxPoint>>();

    char tmp_type = tolower(type, std::locale());

    switch (tmp_type) {
        case 'k':
            kingMoves(*moves, pieces);
            break;

        case 'q':
            queenMoves(*moves, pieces);
            break;

        case 'b':
            bishopMoves(*moves, pieces);
            break;

        case 'n':
            knightMoves(*moves, pieces);
            break;

        case 'r':
            rockMoves(*moves, pieces);
            break;

        case 'p':
            pawnMoves(*moves, pieces);
            break;
    }

    return std::move(moves);
}

void Piece::kingMoves(std::vector<wxPoint>& moves, const Board::arr_type &pieces) {
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if (j == 0 && i == 0) continue;

            int x = j + pos.x, y = i + pos.y;

            if (!movePossible(x, y, pieces)) continue;

            moves.emplace_back(x, y);
        }
    }
}

void Piece::queenMoves(std::vector<wxPoint> &moves, const Board::arr_type &pieces) {
    int x1 = 1, x2 = -1, y1 = 1, y2 = -1;
    bool dir[8] = {true, true, true, true, true, true, true, true};

    for (int i = 0; i < 7; ++i, ++x1, --x2, ++y1, --y2) {
        tryDir(0, pos.x + x1, pos.y + y1, dir, moves, pieces);
        tryDir(1,    pos.x,      pos.y + y1, dir, moves, pieces);
        tryDir(2, pos.x + x2, pos.y + y1, dir, moves, pieces);
        tryDir(3, pos.x + x2,    pos.y,      dir, moves, pieces);
        tryDir(4, pos.x + x2, pos.y + y2, dir, moves, pieces);
        tryDir(5,    pos.x,      pos.y + y2, dir, moves, pieces);
        tryDir(6, pos.x + x1, pos.y + y2, dir, moves, pieces);
        tryDir(7, pos.x + x1,    pos.y,      dir, moves, pieces);
    }
}

void Piece::bishopMoves(std::vector<wxPoint> &moves, const Board::arr_type &pieces) {
    int x1 = 1, x2 = -1, y1 = 1, y2 = -1;
    bool dir[4] = {true, true, true, true};

    for (int i = 0; i < 8; ++i, ++x1, --x2, ++y1, --y2) {
        tryDir(0, pos.x + x1, pos.y + y1, dir, moves, pieces);
        tryDir(1, pos.x + x2, pos.y + y1, dir, moves, pieces);
        tryDir(2, pos.x + x2, pos.y + y2, dir, moves, pieces);
        tryDir(3, pos.x + x1, pos.y + y2, dir, moves, pieces);
    }
}

void Piece::rockMoves(std::vector<wxPoint>& moves, const Board::arr_type& pieces) {
    int x1 = 1, x2 = -1, y1 = 1, y2 = -1;
    bool dir[4] = {true, true, true, true};

    for (int i = 0; i < 8; ++i, ++x1, --x2, ++y1, --y2) {
        tryDir(0,    pos.x,      pos.y + y1, dir, moves, pieces);
        tryDir(1, pos.x + x2,    pos.y,      dir, moves, pieces);
        tryDir(2,    pos.x,      pos.y + y2, dir, moves, pieces);
        tryDir(3, pos.x + x1,    pos.y,      dir, moves, pieces);
    }
}

void Piece::knightMoves(std::vector<wxPoint> &moves, const Board::arr_type &pieces) {

    for (int i = -2; i < 3; ++i) {
        if (i == 0) continue;
        for (int j = -2; j < 3; ++j) {
            if (j == 0 || (i + j) % 2 == 0) continue;
            int x = pos.x + j, y = pos.y + i;

            if (!movePossible(x, y, pieces)) continue;

            moves.emplace_back(x, y);
        }
    }

}

void Piece::pawnMoves(std::vector<wxPoint> &moves, const Board::arr_type &pieces) {
    int i = isWhite() ? -1 : +1;
    int x = pos.x, y = pos.y + i;

    if (movePossible<false>(x, y, pieces)) {
        moves.emplace_back(x, y);

        if (pos.x == initialPos.x && pos.y == initialPos.y) {
            int _y = pos.y + 2 * i;
            if (movePossible<false>(x, _y, pieces)) {
                moves.emplace_back(x, _y);
            }
        }
    }

    if(movePossible<true, true>(x + 1, y, pieces))
        moves.emplace_back(x + 1, y);

    if(movePossible<true, true>(x - 1, y, pieces))
        moves.emplace_back(x - 1, y);

}

void Piece::tryDir(int i, int x, int y, bool* dir, std::vector<wxPoint>& moves, const Board::arr_type& pieces) {

    if (dir[i]) {
        if (movePossible(x, y, pieces)) {
            moves.emplace_back(x, y);

            if (pieces[x][y] && pieces[x][y]->isEnemy(*this))
                dir[i] = false;

        } else {
            dir[i] = false;
        }
    }

}

bool Piece::isCheck(Piece const * king) {

    return false;
}
