//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include <wx/wx.h>
#include "Board.h"

struct Piece {
    const static wxImage* img;
    const wxBitmap* bitmap;
    const char type;

    const wxPoint initialPos;
    wxPoint canvasPos;
    wxPoint pos;

    static wxImage getPieceFromSprite(int i, int j);
    std::unique_ptr<std::vector<wxPoint>> getPossibleMoves(const Board::arr_type& pieces);

    void kingMoves(std::vector<wxPoint>& moves, const Board::arr_type& pieces);
    void queenMoves(std::vector<wxPoint>& moves, const Board::arr_type& pieces);
    void bishopMoves(std::vector<wxPoint>& moves, const Board::arr_type& pieces);
    void rockMoves(std::vector<wxPoint>& moves, const Board::arr_type& pieces);
    void knightMoves(std::vector<wxPoint>& moves, const Board::arr_type& pieces);
    void pawnMoves(std::vector<wxPoint>& moves, const Board::arr_type& pieces);


    void tryDir(int i, int x, int y, bool* dir, std::vector<wxPoint> &moves, const Board::arr_type &pieces);

    template<bool Eats = true, bool Diagonal = false>
    bool movePossible(int x, int y, const Board::arr_type&) const;
    bool isCheck(Piece const * king);

    bool isEnemy(const Piece&) const;
    bool isWhite() const;
    Piece(char type, wxPoint position);
};


#endif //CHESS_PIECE_H
