//
// Created by Mert Çorumlu on 18.08.2021.
//

#include "Symbol.h"
#include "Canvas.h"

const wxImage* Symbol::sprite;

Symbol::Symbol(Piece::Piece piece, wxPoint pos)
: piece{piece}, _pos{pos}, _canvasPos((pos.x) * Canvas::SQ_SIZE, (7 - pos.y) * Canvas::SQ_SIZE) {

    int h = Board::colorOf(piece) ? 1 : 0;

    int i;
    switch (Board::typeOf(piece)) {
        case Piece::Type::King:  i = 0;  break;
        case Piece::Type::Queen: i = 1;  break;
        case Piece::Type::Bishop: i = 2; break;
        case Piece::Type::Knight: i = 3; break;
        case Piece::Type::Rook: i = 4;   break;
        case Piece::Type::Pawn: i = 5;   break;

        default:
            _bitmap = wxBitmap();
            return;
    }

    _bitmap = wxBitmap(_getPieceFromSprite(i, h));
}

wxImage Symbol::_getPieceFromSprite(int i, int h) {
    return sprite->GetSubImage(wxRect(i * 210, h * 210, 210, 210))
                    .Scale(Canvas::SQ_SIZE, Canvas::SQ_SIZE);
}

void Symbol::setPos(int x, int y) {
    if (!(x == _pos.x && y == _pos.y)) {
        _pos.x = x;
        _pos.y = y;
    }
    _canvasPos.x = (x) * Canvas::SQ_SIZE;
    _canvasPos.y = (7 - y) * Canvas::SQ_SIZE;
}

void Symbol::setCanvasPos(int x, int y) {
    _canvasPos.x = x;
    _canvasPos.y = y;
}

const wxPoint& Symbol::pos() const {
    return _pos;
}

const wxPoint& Symbol::canvasPos() const {
    return _canvasPos;
}

const wxBitmap& Symbol::bitmap() const {
    return _bitmap;
}
