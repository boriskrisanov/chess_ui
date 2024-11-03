#pragma once

#include <QApplication>
#include <QPushButton>
#include <QtSvg/QtSvg>
#include <QSvgWidget>
#include "Board.hpp"
#include <cmath>

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        board.loadFen(STARTING_POSITION_FEN);

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                const int index = 8 * i + j;
                if (board.isSquareEmpty(index))
                {
                    continue;
                }
                QString fileName = QString::fromStdString(board[index].toString()).toLower() + ".svg";
                if (board[index].color == WHITE)
                {
                    fileName.prepend("_");
                }
                fileName.prepend("pieces/");
                auto* piece = new QSvgWidget(fileName, this);
                pieceWidgets[index] = piece;
                piece->setGeometry(j * squareSize, i * squareSize, squareSize, squareSize);
                piece->show();
            }
        }
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
            }
            squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        // Called when mouse is moved while pressed
        if (!isPieceBeingMoved)
        {
            return;
        }
        const QPoint mousePosition = event->pos();
        QSvgWidget* pieceWidget = pieceWidgets[moveStartIndex];
        // Subtract half of square size so that the cursor is in the middle of the piece
        const QPoint position = mousePosition - QPoint{squareSize / 2, squareSize / 2};
        pieceWidget->move(position);
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        if (isPieceBeingMoved)
        {
            // Probably not needed
            return;
        }
        const int index = coordinatesToBoardIndex(event->pos());
        if (board[index].isNone())
        {
            return;
        }
        isPieceBeingMoved = true;
        moveStartIndex = index;
        pieceBeingMoved = board[index];
    }

    void mouseReleaseEvent(QMouseEvent* event) override
    {
        isPieceBeingMoved = false;
        const QPoint mousePosition = event->pos();
        const int newIndex = coordinatesToBoardIndex(mousePosition);

        if (pieceWidgets[newIndex] != nullptr)
        {
            delete pieceWidgets[newIndex];
        }

        pieceWidgets[newIndex] = pieceWidgets[moveStartIndex];
        pieceWidgets[moveStartIndex] = nullptr;
        pieceWidgets[newIndex]->move(boardIndexToCoordinates(newIndex));
        // TODO: Castling, promotion, en passant, etc)
        board.makeMove(Move{static_cast<Square>(moveStartIndex), static_cast<Square>(newIndex), MoveFlag::None});
    }

private:
    int coordinatesToBoardIndex(QPoint coordinates) const
    {
        int file = std::floor(coordinates.x() / static_cast<double>(squareSize));
        int rank = std::floor(coordinates.y() / static_cast<double>(squareSize));

        return 8 * rank + file;
    }

    QPoint boardIndexToCoordinates(int index)
    {
        int file = index % 8;
        int rank = std::floor(index / 8);

        return {squareSize * file, squareSize * rank};
    }

    const QColor lightSquareColor{240, 217, 181};
    const QColor darkSquareColor{181, 136, 99};
    const int squareSize = 70;
    Board board;
    std::array<QSvgWidget*, 64> pieceWidgets;

    bool isPieceBeingMoved = false;
    int moveStartIndex;
    Piece pieceBeingMoved;
};
