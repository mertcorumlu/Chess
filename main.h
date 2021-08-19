//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_MAIN_H
#define CHESS_MAIN_H

#include <iostream>
#include <wx/wx.h>
#include "Board.h"
#include "ChessFrame.h"

struct Chess : public wxApp {
    bool OnInit() override;
    ChessFrame* frame = nullptr;
    Board* board = nullptr;
};

#endif //CHESS_MAIN_H
