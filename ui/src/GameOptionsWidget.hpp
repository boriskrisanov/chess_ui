#pragma once

#include "BoardWidget.hpp"
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <random>

#include "GameOptions.hpp"

class GameOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameOptionsWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        engineLevelSlider->setTickInterval(1);
        engineLevelSlider->setMinimum(1);
        engineLevelSlider->setMaximum(10);
        engineLevelSlider->setValue(engineLevel);

        engineLevelSpinBox->setValue(engineLevel);
        engineLevelSpinBox->setMinimum(1);
        engineLevelSpinBox->setMaximum(10);

        vLayout->addWidget(engineLevelSliderLabel);
        engineLevelHLayout->addWidget(engineLevelSpinBox);
        engineLevelSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        engineLevelHLayout->addWidget(engineLevelSlider);

        vLayout->addLayout(engineLevelHLayout);

        vLayout->addWidget(fenInputLabel);
        vLayout->addWidget(fenInput);

        vLayout->addWidget(sideLabel);
        vLayout->addWidget(whiteSideRadioButton);
        vLayout->addWidget(blackSideRadioButton);
        vLayout->addWidget(randomSideRadioButton);

        randomSideRadioButton->setChecked(true);

        vLayout->addWidget(startGameButton);

        setLayout(vLayout);

        connect(engineLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(engineLevelSliderChanged(int)));
        connect(engineLevelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(engineLevelSpinBoxChanged(int)));
        connect(startGameButton, SIGNAL(clicked(bool)), this, SLOT(startButtonClicked()));
    }

private:
    QVBoxLayout* vLayout = new QVBoxLayout(this);

    QHBoxLayout* engineLevelHLayout = new QHBoxLayout();
    QLabel* engineLevelSliderLabel = new QLabel("Engine Level");
    QSpinBox* engineLevelSpinBox = new QSpinBox();
    QSlider* engineLevelSlider = new QSlider(Qt::Horizontal);

    QLabel* fenInputLabel = new QLabel("Starting FEN (Leave empty for starting position)");
    QLineEdit* fenInput = new QLineEdit();

    QLabel* sideLabel = new QLabel("Side");
    QRadioButton* whiteSideRadioButton = new QRadioButton("White");
    QRadioButton* blackSideRadioButton = new QRadioButton("Black");
    QRadioButton* randomSideRadioButton = new QRadioButton("Random");

    QPushButton* startGameButton = new QPushButton("Start Game");

    int engineLevel = 1;

    std::random_device randomDevice;
    std::mt19937 rng{randomDevice()};
    std::uniform_int_distribution<uint32_t> uniformIntDistribution{0, 1};

private slots:
    void engineLevelSliderChanged(int newLevel)
    {
        engineLevel = newLevel;
        engineLevelSpinBox->setValue(newLevel);
    }

    void engineLevelSpinBoxChanged(int newLevel)
    {
        engineLevel = newLevel;
        engineLevelSlider->setValue(newLevel);
    }

    void startButtonClicked()
    {
        PieceColor playerSide;
        if (whiteSideRadioButton->isChecked())
        {
            playerSide = WHITE;
        }
        else if (blackSideRadioButton->isChecked())
        {
            playerSide = BLACK;
        }
        else
        {
            playerSide = uniformIntDistribution(rng) == 0 ? WHITE : BLACK;
        }

        // TODO: Validate FEN

        GameOptions gameOptions{
            16 * engineLevel,
            std::chrono::milliseconds(500 * engineLevel),
            playerSide,
            fenInput->text().isEmpty() ? STARTING_POSITION_FEN : fenInput->text().toStdString()
        };

        emit gameStarted(gameOptions);
    }
signals:
    void gameStarted(GameOptions gameOptions);
};
