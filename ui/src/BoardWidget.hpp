#pragma once

#include <QApplication>
#include <QPushButton>
#include <QtSvg/QtSvg>
#include "Board.hpp"

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        board.loadFen(STARTING_POSITION_FEN);
    }

    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        QColor squareColor = lightSquareColor;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                const int index = 8 * i + j;
                squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
                QRect square(j * squareSize, i * squareSize, squareSize, squareSize);
                painter.fillRect(square, squareColor);

                if (!board.isSquareEmpty(index))
                {
                    QSvgRenderer* pieceRenderer = getPieceRenderer(board[index]);
                    const QRect piecePosition{j * squareSize, i * squareSize, squareSize, squareSize};
                    pieceRenderer->render(&painter, piecePosition);
                }
            }
            squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
        }
    }

private:
    QSvgRenderer* getPieceRenderer(Piece piece) const
    {
        if (piece.color == WHITE)
        {
            switch (piece.kind)
            {
            case PieceKind::PAWN:
                return whitePawnRenderer;
            case PieceKind::KNIGHT:
               return whiteKnightRenderer;
            case PieceKind::BISHOP:
                return whiteBishopRenderer;
            case PieceKind::ROOK:
                return whiteRookRenderer;
            case PieceKind::QUEEN:
                return whiteQueenRenderer;
            case PieceKind::KING:
                return whiteKingRenderer;
            default:
                break;
            }
        }
        else
        {
            switch (piece.kind)
            {
            case PieceKind::PAWN:
                return blackPawnRenderer;
            case PieceKind::KNIGHT:
                return blackKnightRenderer;
            case PieceKind::BISHOP:
                return blackBishopRenderer;
            case PieceKind::ROOK:
                return blackRookRenderer;
            case PieceKind::QUEEN:
                return blackQueenRenderer;
            case PieceKind::KING:
                return blackKingRenderer;
            default:
                break;
            }
        }
        return {};
    }

    const QColor lightSquareColor{240, 217, 181};
    const QColor darkSquareColor{181, 136, 99};
    const int squareSize = 70;
    Board board;

    // Piece renderers
    // Initialising a renderer for each piece once avoids reloading the SVGs each time the board is rendered

    QSvgRenderer* whitePawnRenderer = new QSvgRenderer{QString("pieces/_p.svg")};
    QSvgRenderer* whiteKnightRenderer = new QSvgRenderer{QString("pieces/_n.svg")};
    QSvgRenderer* whiteBishopRenderer = new QSvgRenderer{QString("pieces/_b.svg")};
    QSvgRenderer* whiteRookRenderer = new QSvgRenderer{QString("pieces/_r.svg")};
    QSvgRenderer* whiteQueenRenderer = new QSvgRenderer{QString("pieces/_q.svg")};
    QSvgRenderer* whiteKingRenderer = new QSvgRenderer{QString("pieces/_k.svg")};
    QSvgRenderer* blackPawnRenderer = new QSvgRenderer{QString("pieces/p.svg")};
    QSvgRenderer* blackKnightRenderer = new QSvgRenderer{QString("pieces/n.svg")};
    QSvgRenderer* blackBishopRenderer = new QSvgRenderer{QString("pieces/b.svg")};
    QSvgRenderer* blackRookRenderer = new QSvgRenderer{QString("pieces/r.svg")};
    QSvgRenderer* blackQueenRenderer = new QSvgRenderer{QString("pieces/q.svg")};
    QSvgRenderer* blackKingRenderer = new QSvgRenderer{QString("pieces/k.svg")};
};
