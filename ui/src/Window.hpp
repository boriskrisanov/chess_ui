#pragma once

#include <iostream>
#include <QApplication>
#include "BoardWidget.hpp"

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }

    void resizeEvent(QResizeEvent* event) override
    {
        board->resize(event->size());
        board->repaint();
    }

private:
    BoardWidget* board = new BoardWidget(this);
};
