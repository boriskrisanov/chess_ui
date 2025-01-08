#pragma once

#include <QSvgWidget>
#include "Board.hpp"
#include "GameEndDialog.hpp"
#include "PromotionSelector.hpp"

#include "search.hpp"

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    int coordinatesToBoardIndex(QPoint coordinates) const;
    QPoint boardIndexToCoordinates(int index) const;
    void updateLegalMoves();
    void movePieceWidgets(const Move& move);
    Move getMoveFromIndexes(Square start, Square end, MoveFlag promotion) const;
    void addPieceWidget(Piece piece, Square position);

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

    PromotionSelector* promotionSelector = new PromotionSelector(this);
    GameEndDialog* gameEndDialog = new GameEndDialog(this);
};
