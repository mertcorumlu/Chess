//
// Created by Mert Çorumlu on 18.08.2021.
//

#include "Piece.h"
#include "Canvas.h"

const wxImage* Piece::sprite;

Piece::Piece(Board::Piece piece, wxPoint pos)
: piece{piece}, _pos{pos}, _canvasPos((pos.x) * Canvas::SQ_SIZE, (7 - pos.y) * Canvas::SQ_SIZE) {

    int h = Board::colorOf(piece) ? 1 : 0;

    int i;
    switch (Board::typeOf(piece)) {
        case Board::Type::King:  i = 0;  break;
        case Board::Type::Queen: i = 1;  break;
        case Board::Type::Bishop: i = 2; break;
        case Board::Type::Knight: i = 3; break;
        case Board::Type::Rook: i = 4;   break;
        case Board::Type::Pawn: i = 5;   break;

        default:
            _bitmap = wxBitmap();
            return;
    }

    _bitmap = wxBitmap(_getPieceFromSprite(i, h));
}

wxImage Piece::_getPieceFromSprite(int i, int h) {
    return sprite->GetSubImage(wxRect(i * 210, h * 210, 210, 210))
                    .Scale(Canvas::SQ_SIZE, Canvas::SQ_SIZE);
}

void Piece::setPos(int x, int y) {
    if (!(x == _pos.x && y == _pos.y)) {
        _pos.x = x;
        _pos.y = y;
    }
    _canvasPos.x = (x) * Canvas::SQ_SIZE;
    _canvasPos.y = (7 - y) * Canvas::SQ_SIZE;
}

void Piece::setCanvasPos(int x, int y) {
    _canvasPos.x = x;
    _canvasPos.y = y;
}

const wxPoint& Piece::pos() const {
    return _pos;
}

const wxPoint& Piece::canvasPos() const {
    return _canvasPos;
}

const wxBitmap& Piece::bitmap() const {
    return _bitmap;
}
