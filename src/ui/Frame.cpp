//
// Created by Mert Çorumlu on 18.08.2021.
//

#include "Frame.h"
#include "Canvas.h"

Frame::Frame(wxWindow *parent, wxWindowID id, const wxString &title) :
wxFrame(parent, id, title) {
    Connect(wxEVT_PAINT, wxPaintEventHandler(Frame::_onPaint));
}

void Frame::_onPaint(wxPaintEvent&) {
    wxPaintDC dc(this);
    // draw some text

    dc.SetPen(*wxTRANSPARENT_PEN);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {

            if ((x + y) & 1)
                dc.SetBrush(wxBrush(wxColour(247, 207, 164)));
            else
                dc.SetBrush(wxBrush(wxColour(200, 141, 83)));

            dc.DrawRectangle(x * Canvas::SQ_SIZE, y * Canvas::SQ_SIZE, Canvas::SQ_SIZE, Canvas::SQ_SIZE);
        }
    }
}
