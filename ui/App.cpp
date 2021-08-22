//
// Created by Mert Çorumlu on 18.08.2021.
//
#include "App.h"
#include "Canvas.h"
#include "Frame.h"
#include "Piece.h"

// Initialize
bool App::OnInit() {

    wxInitAllImageHandlers();

    //TODO Embed image into executable
    Piece::sprite = new wxImage("img/pieces_sprite.png");

    auto* frame = new Frame(nullptr, wxID_ANY, "Chess");
    new Canvas(frame);

    // non-resizeable
    frame->SetMaxSize(frame->GetSize());
    frame->SetMinSize(frame->GetSize());
    frame->Center();
    frame->Show(true);

    return true;
}