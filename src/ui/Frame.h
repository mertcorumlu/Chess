//
// Created by Mert Çorumlu on 18.08.2021.
//

#ifndef CHESS_FRAME_H
#define CHESS_FRAME_H


#include <wx/wx.h>

struct Frame : public wxFrame {
public:

    Frame(wxWindow *parent, wxWindowID id, const wxString& title);

private:
    void _onPaint(wxPaintEvent & evt);

};


#endif //CHESS_FRAME_H
