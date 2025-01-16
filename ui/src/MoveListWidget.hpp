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
    void addMove(Move move, Board boardBeforeMove);
private:
    uint16_t moveCount = 0;
};
