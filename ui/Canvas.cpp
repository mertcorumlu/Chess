//
// Created by Mert Çorumlu on 18.08.2021.
//

#include <utility>
#include <unordered_map>
#include "Canvas.h"
#include "Piece.h"

Canvas::Canvas(wxFrame* parent) : wxPanel(parent), _board(), _pieces() {
    parent->SetClientSize(Canvas::SQ_SIZE * 8, Canvas::SQ_SIZE * 8);

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Board::Piece p = _board[x][y];
            if (p)
                _pieces[x][y] = make_shared<Piece>(p, wxPoint(x, y));
        }
    }

    Connect(wxEVT_PAINT, wxPaintEventHandler(Canvas::_onPaint));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(Canvas::_onMouseClick));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(Canvas::_onMouseRelease));
    Connect(wxEVT_MOTION, wxMouseEventHandler(Canvas::_onMouseMove));
}

void Canvas::_onPaint(wxPaintEvent &) {
    wxPaintDC dc(this);
    _render(dc);
}

void Canvas::_render(wxDC &dc) {
    // draw some text
    dc.SetPen(*wxTRANSPARENT_PEN);

    dc.SetBrush(wxBrush(wxColor(255, 0, 0, 150)));

    uint64_t attacks = 0;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (attacks & 1)
                dc.DrawRectangle(x * Canvas::SQ_SIZE, (7 - y) * Canvas::SQ_SIZE, Canvas::SQ_SIZE, Canvas::SQ_SIZE);
            attacks >>= 1;
        }
    }

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            auto piece = _pieces[x][y];
            if(piece && piece != _draggedPiece && piece->bitmap().IsOk()) {
                dc.DrawBitmap(piece->bitmap(), piece->canvasPos().x, piece->canvasPos().y);
            }
        }
    }

    if(_draggedPiece && _draggedPiece->bitmap().IsOk())
        dc.DrawBitmap(_draggedPiece->bitmap(), _draggedPiece->canvasPos().x, _draggedPiece->canvasPos().y);

}

void Canvas::_onMouseMove(wxMouseEvent& evt) {

    if (_draggedPiece) {
        int x = evt.GetPosition().x, y = evt.GetPosition().y;

        if (x >= Canvas::SQ_SIZE / 2 && x <= 7.5 * Canvas::SQ_SIZE) {
            _draggedPiece->setCanvasPos(x - Canvas::SQ_SIZE / 2, _draggedPiece->canvasPos().y);
        }

        if (y >= Canvas::SQ_SIZE / 2 && y <= 7.5 * Canvas::SQ_SIZE) {
            _draggedPiece->setCanvasPos(_draggedPiece->canvasPos().x, y - Canvas::SQ_SIZE / 2);
        }

        Refresh(true);
    }
}

void Canvas::_onMouseClick(wxMouseEvent& evt) {
    int x =  evt.GetPosition().x / Canvas::SQ_SIZE, y = 7 - evt.GetPosition().y / Canvas::SQ_SIZE;
    if (_pieces[x][y]) {
        _draggedPiece = _pieces[x][y];
    }

    Refresh(true);
}

void Canvas::_onMouseRelease(wxMouseEvent&) {

    if (_draggedPiece) {

        int x = (_draggedPiece->canvasPos().x + Canvas::SQ_SIZE / 2) / Canvas::SQ_SIZE,
            y = 7 - (_draggedPiece->canvasPos().y + Canvas::SQ_SIZE / 2) / Canvas::SQ_SIZE;

        _pieces[x][y] = std::move(_pieces[_draggedPiece->pos().x][_draggedPiece->pos().y]);
        _board.move(_draggedPiece->pos().x, _draggedPiece->pos().y, x, y);
        _draggedPiece->setPos(x, y);
        _draggedPiece = nullptr;

        Refresh(true);
    }
}
