#pragma once

#include <iostream>
#include <QResizeEvent>
#include "BoardWidget.hpp"
#include "PromotionSelector.hpp"
#include <QListWidget>

#include "EngineInstance.hpp"
#include "MoveListWidget.hpp"

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        hLayout->addWidget(board, 1);
        hLayout->addWidget(moveList, 1);
        board->repaint();

        connect(engineInstance, &EngineInstance::searchDone, board, &BoardWidget::onEngineSearchDone);
    }

private:
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    MoveListWidget* moveList = new MoveListWidget(this);
    EngineInstance* engineInstance = new EngineInstance();
    BoardWidget* board = new BoardWidget(moveList, engineInstance, this);
};
