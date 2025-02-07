#pragma once

#include <QResizeEvent>
#include "BoardWidget.hpp"
#include "PromotionSelector.hpp"
#include <QListWidget>
#include <QLabel>

#include "EngineInstance.hpp"
#include "GameControlsWidget.hpp"
#include "GameOptions.hpp"
#include "MoveListWidget.hpp"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        board->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        hLayout->setAlignment(Qt::AlignCenter);
        hLayout->addWidget(moveList, 2);
        hLayout->addWidget(board, 5);
        hLayout->addWidget(gameControls, 2);
        board->repaint();
        setLayout(hLayout);

        connect(engineInstance, &EngineInstance::searchDone, board, &BoardWidget::onEngineSearchDone);
        connect(engineInstance, &EngineInstance::searchDone, gameControls->getEngineInfoWidget(),
                &EngineInfoWidget::onSearchDone);
        connect(gameControls, &GameControlsWidget::flipBoard, board, &BoardWidget::flipBoardSlot);

        connect(gameControls, &GameControlsWidget::undoMove, board, &BoardWidget::onUndoMove);
        connect(gameControls, SIGNAL(redoMove()), board, SLOT(onRedoMove()));

        connect(board, SIGNAL(setUndoMoveEnabled(bool)), gameControls, SLOT(setUndoMoveEnabled(bool)));
        connect(board, SIGNAL(setRedoMoveEnabled(bool)), gameControls, SLOT(setRedoMoveEnabled(bool)));

        connect(board, SIGNAL(movePlayed(Move,Board)), moveList, SLOT(movePlayed(Move, Board)));
        connect(board, SIGNAL(moveUndone()), moveList, SLOT(moveUndone()));
        connect(board, SIGNAL(moveRedone()), moveList, SLOT(moveRedone()));

        connect(board, SIGNAL(gameEnded(std::string)), gameControls, SLOT(gameEnded(std::string)));
        connect(gameControls, SIGNAL(copyPgn()), this, SLOT(copyPgn()));
    }

    void keyPressEvent(QKeyEvent* event) override
    {
        std::cout << event->key() << "\n";
        if (event->key() == Qt::Key::Key_Left)
        {
            board->onUndoMove();
        }
        else if (event->key() == Qt::Key::Key_Right)
        {
            board->onRedoMove();
        }
    }

    GameControlsWidget* getGameControls() const
    {
        return gameControls;
    }

public slots:
    void gameStarted(GameOptions gameOptions)
    {
        engineInstance->setSearchTime(gameOptions.searchTime);
        resizeTranspositionTable(gameOptions.transpositionTableSizeMB);
        board->newGame(gameOptions.startingFen, gameOptions.playerSide);
    }

private:
    EngineInstance* engineInstance = new EngineInstance();
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    GameControlsWidget* gameControls = new GameControlsWidget();
    MoveListWidget* moveList = new MoveListWidget();
    BoardWidget* board = new BoardWidget(engineInstance, PieceColor::WHITE, STARTING_POSITION_FEN, this);

private slots:
    void copyPgn() const
    {
        if (moveList->count() == 0)
        {
            // Prevents clearing the clipboard when no moves have been made
            return;
        }

        QString gamePgn;
        for (int i = 0; i < moveList->count(); i++)
        {
            gamePgn.push_back(moveList->item(i)->text());
            gamePgn.push_back(" ");
        }

        QGuiApplication::clipboard()->setText(gamePgn.trimmed());
    }
};
