#pragma once

#include <iostream>
#include <QApplication>
#include <QPushButton>
#include <QtSvg/QtSvg>
#include <QtSvgWidgets/QtSvgWidgets>

#include "BoardWidget.hpp"

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent = nullptr)
    : QWidget(parent)
    {
        // svg->load(QString("pieces/b.svg"));
        // svg->setGeometry(0, 0, 50, 50);

        // board->show();
        // board->setFixedSize(200, 200);
    }

    void resizeEvent(QResizeEvent* event) override
    {
        board->resize(event->size());
        board->repaint();
    }
private:
    // QSvgWidget* svg = new QSvgWidget(this);
    BoardWidget* board = new BoardWidget(this);
};
