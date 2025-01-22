#pragma once

#include <QMainWindow>

#include "GameOptionsWidget.hpp"
#include "GameWidget.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow()
    {
        hLayout->addWidget(gameWidget);
        hLayout->addWidget(gameOptionsWidget);

        gameWidget->hide();
        gameOptionsWidget->show();

        setLayout(hLayout);

        connect(gameOptionsWidget, SIGNAL(gameStarted(GameOptions)), this, SLOT(gameStarted()));
        connect(gameOptionsWidget, SIGNAL(gameStarted(GameOptions)), gameWidget, SLOT(gameStarted(GameOptions)));
    }

private:
    GameWidget* gameWidget = new GameWidget();
    GameOptionsWidget* gameOptionsWidget = new GameOptionsWidget();
    QHBoxLayout* hLayout = new QHBoxLayout(this);

private slots:
    void gameStarted()
    {
        gameOptionsWidget->hide();
        gameWidget->show();
    }
};
