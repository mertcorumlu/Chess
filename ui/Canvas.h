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

using namespace std;

class Canvas : public wxPanel {
public:
    const static int SQ_SIZE = 100;

    explicit Canvas(wxFrame* parent);

private:

    Board _board;
    array<array<shared_ptr<Symbol>, 8> ,8> _pieces;
    shared_ptr<Symbol> _draggedPiece = nullptr;

    void _render(wxDC& dc);
    void _onPaint(wxPaintEvent& evt);
    void _onMouseMove(wxMouseEvent& evt);
    void _onMouseClick(wxMouseEvent& evt);
    void _onMouseRelease(wxMouseEvent& evt);
};


#endif //CHESS_CANVAS_H
