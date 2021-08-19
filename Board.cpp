//
// Created by Mert Çorumlu on 18.08.2021.
//

#include <utility>
#include <unordered_map>
#include "Board.h"
#include "Piece.h"

Board::Board(wxFrame* parent) : wxPanel(parent), possibleMoves() {
    parent->SetClientSize(SQUARE_SIZE * 8, SQUARE_SIZE * 8);
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
                pieces[x][y] = std::make_unique<Piece>(c, wxPoint(x, y));
                if (c == 'k') blackKing = pieces[x][y].get();
                if (c == 'K') whiteKing = pieces[x][y].get();
                ++x;
            break;

            case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
                for(; x < c - '0'; x++) pieces[x][y] = nullptr;
            break;

            case '/':
                ++y;
                x = 0;
            break;

            default:
                pieces[x][y] = nullptr;
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

        dc.SetBrush(*wxGREEN_BRUSH);

        for (auto point : *possibleMoves)
            dc.DrawRectangle(point.x * SQUARE_SIZE, point.y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
    }


    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {

            Piece* piece = pieces[x][y].get();
            if(piece && piece->bitmap)
                dc.DrawBitmap(*piece->bitmap, piece->canvasPos.x, piece->canvasPos.y);
        }
    }
}

void Board::onMouseMove(wxMouseEvent& evt) {

    if (draggedPiece) {
        int x = evt.GetPosition().x, y = evt.GetPosition().y;

        if (x >= SQUARE_SIZE / 2 && x <= 7.5 * SQUARE_SIZE) {
            draggedPiece->canvasPos = wxPoint(x - SQUARE_SIZE / 2, draggedPiece->canvasPos.y);
        }

        if (y >= SQUARE_SIZE / 2 && y <= 7.5 * SQUARE_SIZE) {
            draggedPiece->canvasPos = wxPoint(draggedPiece->canvasPos.x, y - SQUARE_SIZE / 2);
        }

        Refresh(true);
    }
}

void Board::onMouseClick(wxMouseEvent& evt) {
    int x = evt.GetPosition().x / SQUARE_SIZE, y = evt.GetPosition().y / SQUARE_SIZE;

    if (pieces[x][y]) {
        draggedPiece = pieces[x][y].get();
        possibleMoves = std::move(draggedPiece->getPossibleMoves(pieces));
    }

    Refresh(true);
}

void Board::onMouseRelease(wxMouseEvent& evt) {

    if (draggedPiece) {

        int x = (draggedPiece->canvasPos.x + SQUARE_SIZE / 2) / SQUARE_SIZE,
            y = (draggedPiece->canvasPos.y + SQUARE_SIZE / 2) / SQUARE_SIZE;

        wxPoint point(x, y);

        if (std::find(possibleMoves->begin(), possibleMoves->end(), point) != possibleMoves->end()) {

            pieces[x][y] = std::move(pieces[draggedPiece->pos.x][draggedPiece->pos.y]);
            pieces[x][y].get()->canvasPos = wxPoint(x * SQUARE_SIZE, y * SQUARE_SIZE);

            if (!(draggedPiece->pos.x == x && draggedPiece->pos.y == y))
                pieces[draggedPiece->pos.x][draggedPiece->pos.y] = nullptr;

            draggedPiece->pos = wxPoint(x, y);

        } else {
            draggedPiece->canvasPos = wxPoint(draggedPiece->pos.x * SQUARE_SIZE, draggedPiece->pos.y * SQUARE_SIZE);
        }
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
