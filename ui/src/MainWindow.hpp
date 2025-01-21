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

        gameWidget->show();
        gameOptionsWidget->hide();

        setLayout(hLayout);
    }
private:
    GameWidget* gameWidget = new GameWidget();
    GameOptionsWidget* gameOptionsWidget = new GameOptionsWidget();
    QHBoxLayout* hLayout = new QHBoxLayout(this);
};
