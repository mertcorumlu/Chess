//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include <wx/wx.h>
#include "../engine/Board.h"

class Canvas;

using namespace std;

class Piece {

public:
    const static wxImage* sprite;
    const Board::Piece piece;

    Piece(Board::Piece piece, wxPoint position);

    void setPos(int x, int y);
    void setCanvasPos(int x, int y);
    const wxPoint& pos() const;
    const wxPoint& canvasPos() const;
    const wxBitmap& bitmap() const;

private:
    wxPoint _pos;
    wxPoint _canvasPos;
    wxBitmap _bitmap;

    static wxImage _getPieceFromSprite(int i, int j);
};


#endif //CHESS_PIECE_H
