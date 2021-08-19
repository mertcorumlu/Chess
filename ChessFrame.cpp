//
// Created by Mert Çorumlu on 18.08.2021.
//

#include "ChessFrame.h"
#include "Board.h"

void ChessFrame::paintEvent(wxPaintEvent &evt) {
    wxPaintDC dc(this);
    // draw some text

    dc.SetPen(*wxTRANSPARENT_PEN);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {

            if ((x + y) % 2 == 1)
                dc.SetBrush(wxBrush(wxColour(247, 207, 164)));
            else
                dc.SetBrush(wxBrush(wxColour(200, 141, 83)));

            dc.DrawRectangle(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
        }
    }
}

BEGIN_EVENT_TABLE(ChessFrame, wxFrame)
    EVT_PAINT(ChessFrame::paintEvent)
END_EVENT_TABLE()
