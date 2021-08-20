//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include <wx/wx.h>

class Board;
class Representation;

using namespace std;

class Piece {

    wxPoint pos;
    wxPoint canvasPos;

    shared_ptr<vector<wxPoint>> moves;

    void tryDir(int i, int x, int y, bool* dir);

    template<bool Eats = true, bool Diagonal = false>
    bool movePossible(int x, int y) const;

//    bool isCheck(Piece const * king);

    static wxImage getPieceFromSprite(int i, int j);

    [[nodiscard]] unsigned char neutralize() const;

    void kingMoves();
    void queenMoves();
    void bishopMoves();
    void rockMoves();
    void knightMoves();
    void pawnMoves();

public:
    const static wxImage* sprite;
    static uint8_t idCounter;

    Board& board;
    unique_ptr<wxBitmap> bitmap;

    const uint8_t ID;
    const unsigned char type;
    const wxPoint intPos;
    double value;

    Piece(Board& board, uint8_t type, wxPoint position);

    void move(int x, int y);
    void moveCanvasPos(int x, int y);
    [[nodiscard]] const wxPoint& getPos() const;
    [[nodiscard]] const wxPoint& getCanvasPos() const;
    shared_ptr<vector<wxPoint>> getMoves();
    void createMoves();
    [[nodiscard]] bool isEnemy(const Piece&) const;
    [[nodiscard]] bool isWhite() const;
};


#endif //CHESS_PIECE_H
