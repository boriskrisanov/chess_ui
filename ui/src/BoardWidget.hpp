#pragma once

#include <qevent.h>
#include <QListWidget>
#include <QSvgWidget>
#include <thread>

#include "Board.hpp"
#include "EngineInstance.hpp"
#include "GameOptions.hpp"
#include "MoveListWidget.hpp"
#include "PromotionSelector.hpp"
#include "search.hpp"

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(EngineInstance* engineInstance, PieceColor playerSide, std::string startingFen, QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void drawPieces();
    int heightForWidth(int w) const override
    {
        return w;
    }
    bool hasHeightForWidth() const override
    {
        return true;
    }

    void newGame(std::string fen, PieceColor playerSide)
    {
        board.loadFen(fen);
        this->playerSide = playerSide;

        if (playerSide == PieceColor::BLACK)
        {
            flipBoard = true;
        }
        if (board.sideToMove != playerSide)
        {
            engineInstance->startSearch(board);
        }
        updateLegalMoves();
        repaint();
        drawPieces();
    }

signals:
    void setUndoMoveEnabled(bool);
    void setRedoMoveEnabled(bool);
    void movePlayed(Move move, Board boardBeforeMove);
    void moveUndone();
    void moveRedone();

public slots:
    void onEngineSearchDone(SearchResult move);
    void onUndoMove();
    void onRedoMove();
    void flipBoardSlot();

private:
    int coordinatesToBoardIndex(QPoint coordinates) const;
    QPoint boardIndexToCoordinates(int index) const;
    void updateLegalMoves();
    void movePieceWidgets(const Move& move, bool animate);
    void undoMovePieceWidgets(Board boardAfterMoveUndone, const Move& move);
    Move getMoveFromIndexes(Square start, Square end, MoveFlag promotion) const;
    void addPieceWidget(Piece piece, Square position);
    void animatePieceMovement(Square pieceIndex, Square start, Square end) const;

    std::stack<Move> undoneMoves;

    const QColor lightSquareColor{240, 217, 181};
    const QColor darkSquareColor{181, 136, 99};
    const QColor lightLegalMoveSquareColor{130, 151, 105};
    const QColor darkLegalMoveSquareColor{100, 111, 65};
    const QColor checkSquareColor{255, 70, 60};
    int squareSize;
    Board board;
    std::array<QSvgWidget*, 64> pieceWidgets;
    std::array<std::vector<int>, 64> legalMoves;
    bool isPieceBeingMoved = false;
    int moveStartIndex;
    Piece pieceBeingMoved;
    EngineInstance* engineInstance;

    bool flipBoard = false;
    PieceColor playerSide = PieceColor::WHITE;

    PromotionSelector* promotionSelector = new PromotionSelector(this);
};
