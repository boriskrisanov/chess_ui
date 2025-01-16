#include "MoveListWidget.hpp"

#include <iostream>

void MoveListWidget::addMove(Move move, Board boardBeforeMove)
{
    const std::string movePgn = move.getPgn(boardBeforeMove);
    if (moveCount % 2 == 0)
    {
        std::string moveString = std::to_string((moveCount / 2) + 1) + ". " + movePgn;
        addItem(QString::fromStdString(moveString));
    }
    else
    {
        // Append to existing move string
        QListWidgetItem* currentRow = item(count() - 1);
        currentRow->setText(currentRow->text() + " " + QString::fromStdString(movePgn));
    }
    moveCount++;
}
