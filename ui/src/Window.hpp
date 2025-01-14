#pragma once

#include <iostream>
#include <QResizeEvent>
#include "BoardWidget.hpp"
#include "PromotionSelector.hpp"
#include <QListWidget>

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
    }

private:
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    QListWidget* moveList = new QListWidget(this);
    BoardWidget* board = new BoardWidget(moveList, this);
};
