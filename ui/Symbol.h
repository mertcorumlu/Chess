//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_SYMBOL_H
#define CHESS_SYMBOL_H

#include <wx/wx.h>
#include "../engine/Board.h"
#include "../types.h"

class Canvas;

using namespace std;

class Symbol {

public:
    const static wxImage* sprite;
    const Piece::Piece piece;

    Symbol(Piece::Piece piece, wxPoint position);

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


#endif //CHESS_SYMBOL_H
