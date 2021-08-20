//
// Created by Mert Çorumlu on 18.08.2021.
//
#include "main.h"
#include "Board.h"
#include "ChessFrame.h"
#include "Piece.h"

// Initialize
bool Chess::OnInit() {

    wxInitAllImageHandlers();

    //TODO Embed image into executable
    Piece::sprite = new wxImage("img/pieces_sprite.png");

    auto* frame = new ChessFrame(nullptr, wxID_ANY, "Chess");
    new Board(frame);

    // non-resizeable
    frame->SetMaxSize(frame->GetSize());
    frame->SetMinSize(frame->GetSize());
    frame->Center();
    frame->Show(true);

    return true;
}

// Implement main
wxIMPLEMENT_APP(Chess);