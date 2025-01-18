#pragma once

#include <QLabel>
#include <QPushButton>
#include "BoardWidget.hpp"
#include "EngineInfoWidget.hpp"
#include <QClipboard>
#include <QGuiApplication>

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

        vLayout->addLayout(buttonHLayout);
        buttonHLayout->setAlignment(Qt::AlignHCenter);
        buttonHLayout->addWidget(backButton);
        buttonHLayout->addWidget(forwardButton);
        buttonHLayout->addWidget(endGameButton);
        buttonHLayout->addWidget(copyPgnButton);

        vLayout->addWidget(player2Text);
        setLayout(vLayout);

        connect(copyPgnButton, SIGNAL (clicked(bool)), this, SLOT (copyPgnClicked()));
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
    QHBoxLayout* buttonHLayout = new QHBoxLayout();
    QLabel* player1Text = new QLabel("Computer");
    QLabel* player2Text = new QLabel("Human");
    EngineInfoWidget* engineInfoWidget = new EngineInfoWidget(this);

    QPushButton* backButton = new QPushButton("<");
    QPushButton* forwardButton = new QPushButton(">");
    QPushButton* endGameButton = new QPushButton("End Game");
    QPushButton* copyPgnButton = new QPushButton("Copy PGN");
private slots:
    void copyPgnClicked() const
    {
        if (moveList->count() == 0)
        {
            // Prevents clearing the clipboard when no moves have been made
            return;
        }

        QString gamePgn;
        for (size_t i = 0; i < moveList->count(); i++)
        {
            gamePgn.push_back(moveList->item(i)->text());
            gamePgn.push_back(" ");
        }

        QGuiApplication::clipboard()->setText(gamePgn.trimmed());
    }
};
