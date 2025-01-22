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
        connect(engineInstance, &EngineInstance::searchDone, gameControls->getEngineInfoWidget(), &EngineInfoWidget::onSearchDone);
    }

public slots:
    void gameStarted(GameOptions gameOptions)
    {
        board->newGame(gameOptions.startingFen, gameOptions.playerSide);
    }

private:
    EngineInstance* engineInstance = new EngineInstance();
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    GameControlsWidget* gameControls = new GameControlsWidget();
    MoveListWidget* moveList = new MoveListWidget();
    BoardWidget* board = new BoardWidget(moveList, engineInstance, WHITE, STARTING_POSITION_FEN, this);
};
