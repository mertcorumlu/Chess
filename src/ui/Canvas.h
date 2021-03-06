//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_CANVAS_H
#define CHESS_CANVAS_H

#include <wx/wx.h>
#include <array>
#include <memory>
#include "Symbol.h"
#include "../engine/Board.h"
#include "../engine/Position.h"

using namespace std;

class Canvas : public wxPanel {
public:
    const static int SQ_SIZE = 100;

    Canvas(wxFrame* parent, string &&fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

private:

    Position _position;
    array<array<shared_ptr<Symbol>, 8> ,8> _pieces;
    shared_ptr<Symbol> _draggedPiece = nullptr;

    void _render(wxDC& dc);
    void _onPaint(wxPaintEvent& evt);
    void _onMouseMove(wxMouseEvent& evt);
    void _onMouseClick(wxMouseEvent& evt);
    void _onMouseRelease(wxMouseEvent& evt);
};


#endif //CHESS_CANVAS_H
