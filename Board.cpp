//
// Created by Mert Çorumlu on 18.08.2021.
//

#include <utility>
#include <unordered_map>
#include "Board.h"
#include "Piece.h"

Board::Board(wxFrame* parent) : wxPanel(parent), possibleMoves(), board() {
    parent->SetClientSize(Board::SQ_SIZE * 8, Board::SQ_SIZE * 8);
    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

void Board::parseFEN(std::string fen) {

    int x = 0, y = 0;

    for(auto& c : fen) {
        switch (c) {
            case 'p': case 'P':
            case 'r': case 'R':
            case 'n': case 'N':
            case 'b': case 'B':
            case 'q': case 'Q':
            case 'k': case 'K':
                board.add(make_shared<Piece>(*this, c, wxPoint(x, y)));
                ++x;
            break;

            case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
                for(int i = 0; i < c - '0'; ++i, ++x) board[x][y] = nullptr;
            break;

            case '/':
                ++y;
                x = 0;
            break;

            default:
                board[x][y] = nullptr;
        }
    }
}

void Board::paintEvent(wxPaintEvent &evt) {
    wxPaintDC dc(this);
    render(dc);
}

void Board::render(wxDC &dc) {
    // draw some text
    dc.SetPen(*wxTRANSPARENT_PEN);

    if (possibleMoves) {

        dc.SetBrush(wxBrush(wxColor(0, 255, 0, 150)));

        for (auto& point : *possibleMoves)
            dc.DrawRectangle(point.x * Board::SQ_SIZE, point.y * Board::SQ_SIZE, Board::SQ_SIZE, Board::SQ_SIZE);
    }


    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {

            auto piece = board[x][y];
            if(piece && piece != draggedPiece && piece->bitmap) {
                dc.DrawBitmap(*piece->bitmap, piece->getCanvasPos().x, piece->getCanvasPos().y);
            }
        }
    }

    if(draggedPiece && draggedPiece->bitmap)
        dc.DrawBitmap(*draggedPiece->bitmap, draggedPiece->getCanvasPos().x, draggedPiece->getCanvasPos().y);

}

void Board::onMouseMove(wxMouseEvent& evt) {

    if (draggedPiece) {
        int x = evt.GetPosition().x, y = evt.GetPosition().y;

        if (x >= Board::SQ_SIZE / 2 && x <= 7.5 * Board::SQ_SIZE) {
            draggedPiece->moveCanvasPos(x - Board::SQ_SIZE / 2, draggedPiece->getCanvasPos().y);
        }

        if (y >= Board::SQ_SIZE / 2 && y <= 7.5 * Board::SQ_SIZE) {
            draggedPiece->moveCanvasPos(draggedPiece->getCanvasPos().x, y - Board::SQ_SIZE / 2);
        }

        Refresh(true);
    }
}

void Board::onMouseClick(wxMouseEvent& evt) {
    int x = evt.GetPosition().x / Board::SQ_SIZE, y = evt.GetPosition().y / Board::SQ_SIZE;

    if (board[x][y]) {
        draggedPiece = board[x][y];
        draggedPiece->createMoves();
        possibleMoves = draggedPiece->getMoves();
    }

    Refresh(true);
}

void Board::onMouseRelease(wxMouseEvent& evt) {

    if (draggedPiece) {

        int x = (draggedPiece->getCanvasPos().x + Board::SQ_SIZE / 2) / Board::SQ_SIZE,
            y = (draggedPiece->getCanvasPos().y + Board::SQ_SIZE / 2) / Board::SQ_SIZE;

        wxPoint point(x, y);

        if (std::find(possibleMoves->begin(), possibleMoves->end(), point) != possibleMoves->end())
            draggedPiece->move(x, y);
        else
            draggedPiece->move(draggedPiece->getPos().x, draggedPiece->getPos().y);

        draggedPiece = nullptr;
        possibleMoves.reset();

        Refresh(true);
    }
}

BEGIN_EVENT_TABLE(Board, wxPanel)

                EVT_PAINT(Board::paintEvent)
                EVT_MOTION(Board::onMouseMove)
                EVT_LEFT_DOWN(Board::onMouseClick)
                EVT_LEFT_UP(Board::onMouseRelease)

END_EVENT_TABLE()
