#pragma once

#include <QResizeEvent>
#include "BoardWidget.hpp"
#include "PromotionSelector.hpp"
#include <QListWidget>
#include <QLabel>

#include "EngineInstance.hpp"
#include "GameControlsWidget.hpp"
#include "MoveListWidget.hpp"

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        board->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        hLayout->addWidget(board);
        hLayout->addWidget(gameControls);
        board->repaint();
        setLayout(hLayout);

        connect(engineInstance, &EngineInstance::searchDone, board, &BoardWidget::onEngineSearchDone);
    }

private:
    EngineInstance* engineInstance = new EngineInstance();
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    GameControlsWidget* gameControls = new GameControlsWidget();
    BoardWidget* board = new BoardWidget(gameControls->getMoveList(), engineInstance, this);
};
