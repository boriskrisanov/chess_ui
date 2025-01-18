#pragma once

#include <QLabel>

#include "BoardWidget.hpp"
#include "EngineInfoWidget.hpp"
#include <iostream>

class GameControlsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameControlsWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        vLayout->addWidget(player1Text);

        vLayout->addLayout(hLayout);

        hLayout->addWidget(moveList);
        hLayout->addWidget(engineInfoWidget, 1);

        vLayout->addWidget(player2Text);
        setLayout(vLayout);
    }

    // TODO: Probably better to use signals and slots with BoardWidget
    MoveListWidget* getMoveList() const
    {
        // std::cout << moveList << "\n";
        return moveList;
    }
private:
    MoveListWidget* moveList = new MoveListWidget();
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QHBoxLayout* hLayout = new QHBoxLayout();
    QLabel* player1Text = new QLabel("Computer");
    QLabel* player2Text = new QLabel("Human");
    EngineInfoWidget* engineInfoWidget = new EngineInfoWidget(this);
};
