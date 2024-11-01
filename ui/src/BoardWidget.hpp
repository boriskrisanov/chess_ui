#pragma once

#include <QApplication>
#include <QPushButton>
#include <QtSvg/QtSvg>
#include <QtSvgWidgets/QtSvgWidgets>
#include "Board.hpp"

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }

    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        QColor squareColor = lightSquareColor;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                const int index = 8 * i + j;
                squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
                QRect square(j * squareSize, i * squareSize, squareSize, squareSize);
                painter.fillRect(square, squareColor);
            }
            squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
        }
    }

private:
    const QColor lightSquareColor{240, 217, 181};
    const QColor darkSquareColor{181, 136, 99};
    const int squareSize = 50;
    Board board;
};
