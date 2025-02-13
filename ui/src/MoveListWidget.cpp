#include "MoveListWidget.hpp"

void MoveListWidget::movePlayed(const Move& move, const Board& boardBeforeMove)
{
    const size_t moveCount = moveHistory.size();

    if (!undoneMoves.empty())
    {
        // Start new move sequence from here, delete all subsequent moves
        while (!undoneMoves.empty())
        {
            undoneMoves.pop();
        }
        // ReSharper disable once CppDFALoopConditionNotUpdated
        while (count() > (moveCount + (moveCount % 2 == 1 ? 1 : 0)) / 2)
        {
            takeItem(count() - 1);
        }
        if (moveCount % 2 == 1)
        {
            QListWidgetItem* currentRow = item(count() - 1);
            QStringList s = currentRow->text().split(" ");
            currentRow->setText(s[0] + ' ' + s[1]);
        }
    }

    const std::string movePgn = move.getPgn(boardBeforeMove);

    if (moveCount % 2 == 0)
    {
        // New line
        std::string moveString = std::to_string((moveCount / 2) + 1) + ". " + movePgn;
        addItem(QString::fromStdString(moveString));
    }
    else
    {
        // Append to existing move string
        QListWidgetItem* currentRow = item(count() - 1);
        currentRow->setText(currentRow->text() + " " + QString::fromStdString(movePgn));
    }
    setCurrentRow(count() - 1);
    moveHistory.push(move);
    scrollToBottom();
}

void MoveListWidget::moveUndone()
{
    undoneMoves.push(moveHistory.top());
    moveHistory.pop();
    // Divide by 2 because a single entry stores two half moves
    setCurrentRow((moveHistory.size() - 1) / 2);
}

void MoveListWidget::moveRedone()
{
    moveHistory.push(undoneMoves.top());
    undoneMoves.pop();
    // Divide by 2 because a single entry stores two half moves
    setCurrentRow((moveHistory.size() - 1) / 2);
}

void MoveListWidget::newGame()
{
    clear();
    while (!moveHistory.empty())
    {
        moveHistory.pop();
    }
    while (!undoneMoves.empty())
    {
        undoneMoves.pop();
    }
    QListWidget::reset();
}
