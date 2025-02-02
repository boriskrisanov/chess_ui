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

        connect(board, SIGNAL(movePlayed(Move, Board)), moveList, SLOT(movePlayed(Move, Board)));
        connect(board, SIGNAL(moveUndone()), moveList, SLOT(moveUndone()));
        connect(board, SIGNAL(moveRedone()), moveList, SLOT(moveRedone()));
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
};
