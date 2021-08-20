//
// Created by Mert Çorumlu on 18.08.2021.
//

#include "ChessFrame.h"
#include "Board.h"

ChessFrame::ChessFrame(wxWindow *parent, wxWindowID id, const wxString &title) :
wxFrame(parent, id, title) {}

void ChessFrame::paintEvent(wxPaintEvent &evt) {
    wxPaintDC dc(this);
    // draw some text

    dc.SetPen(*wxTRANSPARENT_PEN);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {

            if ((x + y) & 1)
                dc.SetBrush(wxBrush(wxColour(247, 207, 164)));
            else
                dc.SetBrush(wxBrush(wxColour(200, 141, 83)));

            dc.DrawRectangle(x * Board::SQ_SIZE, y * Board::SQ_SIZE, Board::SQ_SIZE, Board::SQ_SIZE);
        }
    }
}

BEGIN_EVENT_TABLE(ChessFrame, wxFrame)
    EVT_PAINT(ChessFrame::paintEvent)
END_EVENT_TABLE()
