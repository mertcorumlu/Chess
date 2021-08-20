//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <wx/wx.h>
#include <array>
#include <memory>
#include "Representation.h"

using namespace std;

class Board : public wxPanel {

    shared_ptr<Piece> draggedPiece = nullptr;
    shared_ptr<vector<wxPoint>> possibleMoves = nullptr;

    void parseFEN(string str);
    void render(wxDC& dc);
    void paintEvent(wxPaintEvent& evt);
    void onMouseMove(wxMouseEvent& evt);
    void onMouseClick(wxMouseEvent& evt);
    void onMouseRelease(wxMouseEvent& evt);

    DECLARE_EVENT_TABLE()

public:
    const static int SQ_SIZE = 100;
    Representation board;

    explicit Board(wxFrame* parent);
};


#endif //CHESS_BOARD_H
