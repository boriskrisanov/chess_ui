#pragma once

#include <QApplication>
#include <QPushButton>
#include <QtSvg/QtSvg>
#include <QSvgWidget>
#include "Board.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

#include "search.hpp"

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        board.loadFen(STARTING_POSITION_FEN);
        updateLegalMoves();

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
                const auto legalMovesForSquare = legalMoves[moveStartIndex];
                if (isPieceBeingMoved && std::find(legalMovesForSquare.begin(), legalMovesForSquare.end(), index) !=
                    legalMovesForSquare.end())
                {
                    painter.fillRect(square, legalMoveSquareColor);
                }
                if ((board.whiteKing & bitboards::withSquare(index)) != 0 && board.isSideInCheck(WHITE)
                    || (board.blackKing & bitboards::withSquare(index)) != 0 && board.isSideInCheck(BLACK))
                {
                    painter.fillRect(square, checkSquareColor);
                }
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
        repaint();
    }

    void mouseReleaseEvent(QMouseEvent* event) override
    {
        isPieceBeingMoved = false;
        const QPoint mousePosition = event->pos();
        const int newIndex = coordinatesToBoardIndex(mousePosition);

        const auto legalMovesForSquare = legalMoves[moveStartIndex];
        const bool isIllegalMove = std::ranges::find(legalMovesForSquare, newIndex) == legalMovesForSquare.end();
        if (isIllegalMove)
        {
            // Ignore move
            pieceWidgets[moveStartIndex]->move(boardIndexToCoordinates(moveStartIndex));
            isPieceBeingMoved = false;
            repaint();
            return;
        }

        const Move move = getMoveFromIndexes(moveStartIndex, newIndex);
        movePieceWidgets(move);
        board.makeMove(move);
        updateLegalMoves();
        repaint();

        // TODO: Do this is in a separate thread so the app doesn't freeze
        SearchResult searchResult = timeLimitedSearch(board, static_cast<std::chrono::milliseconds>(1000));
        std::cout << "Engine move: " << static_cast<std::string>(searchResult.bestMove) << "\n";
        movePieceWidgets(searchResult.bestMove);
        board.makeMove(searchResult.bestMove);
        updateLegalMoves();
        repaint();
    }

private:
    int coordinatesToBoardIndex(QPoint coordinates) const
    {
        int file = std::floor(coordinates.x() / static_cast<double>(squareSize));
        int rank = std::floor(coordinates.y() / static_cast<double>(squareSize));

        return 8 * rank + file;
    }

    QPoint boardIndexToCoordinates(int index) const
    {
        int file = index % 8;
        int rank = std::floor(index / 8);

        return {squareSize * file, squareSize * rank};
    }

    void updateLegalMoves()
    {
        for (int i = 0; i < 64; i++)
        {
            legalMoves[i].clear();
        }
        for (Move move : board.getLegalMoves())
        {
            legalMoves[move.start()].push_back(move.end());
        }
    }

    void movePieceWidgets(const Move& move)
    {
        // Remove piece if it was captured
        if (pieceWidgets[move.end()] != nullptr)
        {
            delete pieceWidgets[move.end()];
        }
        if (move.moveFlag() == MoveFlag::EnPassant)
        {
            delete pieceWidgets[board.sideToMove == WHITE ? move.end() + 8 : move.end() - 8];
        }

        pieceWidgets[move.end()] = pieceWidgets[move.start()];
        pieceWidgets[move.start()] = nullptr;
        pieceWidgets[move.end()]->move(boardIndexToCoordinates(move.end()));
        // Move rook if castling

        if (board.sideToMove == WHITE)
        {
            const Square a1 = square::fromString("a1");
            const Square d1 = square::fromString("d1");
            const Square f1 = square::fromString("f1");
            const Square h1 = square::fromString("h1");
            if (move.moveFlag() == MoveFlag::ShortCastling)
            {
                pieceWidgets[f1] = pieceWidgets[h1];
                pieceWidgets[h1]->move(boardIndexToCoordinates(f1));
                pieceWidgets[h1] = nullptr;
            }
            else if (move.moveFlag() == MoveFlag::LongCastling)
            {
                pieceWidgets[d1] = pieceWidgets[a1];
                pieceWidgets[a1]->move(boardIndexToCoordinates(d1));
                pieceWidgets[a1] = nullptr;
            }
        }
        else
        {
            const Square a8 = square::fromString("a8");
            const Square d8 = square::fromString("d8");
            const Square f8 = square::fromString("f8");
            const Square h8 = square::fromString("h8");
            if (move.moveFlag() == MoveFlag::ShortCastling)
            {
                pieceWidgets[f8] = pieceWidgets[h8];
                pieceWidgets[h8]->move(boardIndexToCoordinates(f8));
                pieceWidgets[h8] = nullptr;
            }
            else if (move.moveFlag() == MoveFlag::LongCastling)
            {
                pieceWidgets[d8] = pieceWidgets[a8];
                pieceWidgets[a8]->move(boardIndexToCoordinates(d8));
                pieceWidgets[a8] = nullptr;
            }
        }

        // TODO: Promotion
    }

    Move getMoveFromIndexes(Square start, Square end) const
    {
        MoveFlag moveFlag = MoveFlag::None;
        // En Passant
        if (board[start].kind == PieceKind::PAWN && end == board.getEnPassantTargetSquare())
        {
            moveFlag = MoveFlag::EnPassant;
        }
        // Castling
        else if (board[start].kind == PieceKind::KING)
        {
            if (board.sideToMove == WHITE)
            {
                const Square c1 = square::fromString("c1");
                const Square g1 = square::fromString("g1");
                if (board.canWhiteShortCastle() && end == g1)
                {
                    moveFlag = MoveFlag::ShortCastling;
                }
                else if (board.canWhiteLongCastle() && end == c1)
                {
                    moveFlag = MoveFlag::LongCastling;
                }
            }
            else
            {
                const Square c8 = square::fromString("c8");
                const Square g8 = square::fromString("g8");
                if (board.canBlackShortCastle() && end == g8)
                {
                    moveFlag = MoveFlag::ShortCastling;
                }
                else if (board.canBlackLongCastle() && end == c8)
                {
                    moveFlag = MoveFlag::LongCastling;
                }
            }
        }
        return Move{start, end, moveFlag};
    }

    const QColor lightSquareColor{240, 217, 181};
    const QColor darkSquareColor{181, 136, 99};
    const QColor legalMoveSquareColor{66, 135, 245};
    const QColor checkSquareColor{255, 70, 60};
    const int squareSize = 70;
    Board board;
    std::array<QSvgWidget*, 64> pieceWidgets;
    std::array<std::vector<int>, 64> legalMoves;

    bool isPieceBeingMoved = false;
    int moveStartIndex;
    Piece pieceBeingMoved;
};
