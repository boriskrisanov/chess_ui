#pragma once

#include <iostream>
#include <QResizeEvent>
#include "BoardWidget.hpp"
#include "PromotionSelector.hpp"

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        hLayout->addWidget(board);
        board->repaint();
    }

private:
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    BoardWidget* board = new BoardWidget(this);
};
