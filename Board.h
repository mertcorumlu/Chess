//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#define SQUARE_SIZE 100

#include <wx/wx.h>
#include <array>

class Piece;

struct Board : public wxPanel {
    Board(wxFrame* parent);
    void render(wxDC& dc);
    void paintEvent(wxPaintEvent & evt);
    void onMouseMove(wxMouseEvent & evt);
    void onMouseClick(wxMouseEvent & evt);
    void onMouseRelease(wxMouseEvent & evt);

    using arr_type = std::array<std::array<std::unique_ptr<Piece>, 8>, 8>;


private:
    Piece* whiteKing;
    Piece* blackKing;
    arr_type pieces;
    void parseFEN(std::string str);
    Piece* draggedPiece = nullptr;
    std::unique_ptr<std::vector<wxPoint>> possibleMoves;

    DECLARE_EVENT_TABLE()
};


#endif //CHESS_BOARD_H
