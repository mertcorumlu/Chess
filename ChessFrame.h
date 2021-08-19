//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_CHESSFRAME_H
#define CHESS_CHESSFRAME_H


#include <wx/wx.h>

struct ChessFrame : public wxFrame {
public:
    ChessFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxASCII_STR(wxFrameNameStr)) : wxFrame(parent, id, title, pos, size, style, name) {}

    void paintEvent(wxPaintEvent & evt);

private:
    DECLARE_EVENT_TABLE()

};


#endif //CHESS_CHESSFRAME_H
