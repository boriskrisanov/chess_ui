#pragma once

#include <QListWidget>

#include "Board.hpp"
#include "Move.hpp"

class MoveListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit MoveListWidget(QWidget* parent = nullptr)
        : QListWidget(parent)
    {

    }
signals:
    void moveClicked(int moveNumber);
public slots:
    void movePlayed(const Move& move, const Board& boardBeforeMove);
    void moveUndone();
    void moveRedone();
private:
    std::stack<Move> moveHistory;
    std::stack<Move> undoneMoves;
};
