#pragma once

#include "BoardWidget.hpp"
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>

class GameOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    GameOptionsWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        engineLevelSlider->setTickInterval(1);
        engineLevelSlider->setMinimum(1);
        engineLevelSlider->setMaximum(10);

        vLayout->addWidget(engineLevelSliderLabel);
        vLayout->addWidget(engineLevelSlider);
        vLayout->addWidget(fenInputLabel);
        vLayout->addWidget(fenInput);

        vLayout->addWidget(sideLabel);
        vLayout->addWidget(whiteSideRadioButton);
        vLayout->addWidget(blackSideRadioButton);
        vLayout->addWidget(randomSideRadioButton);


        setLayout(vLayout);
    }

private:
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QLabel* engineLevelSliderLabel = new QLabel("Engine Level");
    QSlider* engineLevelSlider = new QSlider(Qt::Horizontal);
    QLabel* fenInputLabel = new QLabel("Starting FEN (Leave empty for starting position)");
    QLineEdit* fenInput = new QLineEdit();

    QLabel* sideLabel = new QLabel("Side");
    QRadioButton* whiteSideRadioButton = new QRadioButton("White");
    QRadioButton* blackSideRadioButton = new QRadioButton("Black");
    QRadioButton* randomSideRadioButton = new QRadioButton("Random");
};