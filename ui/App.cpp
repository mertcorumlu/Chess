//
// Created by Mert Çorumlu on 18.08.2021.
//
#include "App.h"
#include "Canvas.h"
#include "Frame.h"
#include "Symbol.h"

// Initialize
bool App::OnInit() {

    wxInitAllImageHandlers();

    //TODO Embed image into executable
    Symbol::sprite = new wxImage("img/pieces_sprite.png");

    auto* frame = new Frame(nullptr, wxID_ANY, "Chess");
    new Canvas(frame);//, "kbnqp///////k b KQkq - 0 1");

    // non-resizeable
    frame->SetMaxSize(frame->GetSize());
    frame->SetMinSize(frame->GetSize());
    frame->Center();
    frame->Show(true);

    return true;
}