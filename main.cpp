//
// Created by Mert Çorumlu on 18.08.2021.
//
#include <iostream>
#include "main.h"
#include "Piece.h"

using namespace std;

const wxImage* Piece::img;

bool Chess::OnInit() {

    wxInitAllImageHandlers();

    Piece::img = new wxImage("img/pieces_sprite.png");

    frame = new ChessFrame(nullptr, wxID_ANY, "Chess", wxPoint(100, 100), wxDefaultSize);

    board = new Board(frame);

    // non-resizeable
//    frame->SetMaxSize(frame->GetSize());
    frame->SetMinSize(frame->GetSize());

    frame->Center();

    frame->Show(true);

    return true;
}

wxIMPLEMENT_APP(Chess);