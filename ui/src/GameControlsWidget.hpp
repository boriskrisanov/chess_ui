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

        vLayout->addWidget(engineInfoWidget);

        vLayout->addWidget(backButton);
        vLayout->addWidget(forwardButton);
        vLayout->addWidget(flipBoardButton);
        vLayout->addWidget(endGameButton);
        vLayout->addWidget(copyPgnButton);

        vLayout->addWidget(player2Text);
        setLayout(vLayout);

        // engineInfoWidget->hide();

        backButton->setEnabled(false);
        forwardButton->setEnabled(false);

        connect(copyPgnButton, SIGNAL(clicked(bool)), this, SLOT(copyPgnClicked()));
        connect(flipBoardButton, SIGNAL(clicked(bool)), this, SLOT(flipBoardButtonClicked()));
        connect(backButton, SIGNAL(clicked(bool)), this, SLOT(undoMoveButtonClicked()));
        connect(forwardButton, SIGNAL(clicked(bool)), this, SLOT(redoMoveButtonClicked()));
    }

    // TODO: Forward signals from engine instance?
    EngineInfoWidget* getEngineInfoWidget() const
    {
        return engineInfoWidget;
    }

public slots:
    void setUndoMoveEnabled(bool enabled) const
    {
        backButton->setEnabled(enabled);
    }

    void setRedoMoveEnabled(bool enabled) const
    {
        forwardButton->setEnabled(enabled);
    }

private:
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QLabel* player1Text = new QLabel("Computer");
    QLabel* player2Text = new QLabel("Human");
    EngineInfoWidget* engineInfoWidget = new EngineInfoWidget(this);

    // QCheckBox* engineInfoCheckbox = new QCheckBox("Show engine info", this);

    QPushButton* backButton = new QPushButton("<");
    QPushButton* forwardButton = new QPushButton(">");
    QPushButton* flipBoardButton = new QPushButton("Flip Board");
    QPushButton* endGameButton = new QPushButton("End Game");
    QPushButton* copyPgnButton = new QPushButton("Copy PGN");
signals:
    void flipBoard();
    void undoMove();
    void redoMove();

private slots:
    void flipBoardButtonClicked()
    {
        emit flipBoard();
    }

    void undoMoveButtonClicked()
    {
        emit undoMove();
    }

    void redoMoveButtonClicked()
    {
        emit redoMove();
    }

    void copyPgnClicked() const
    {
        // if (moveList->count() == 0)
        // {
        //     // Prevents clearing the clipboard when no moves have been made
        //     return;
        // }
        //
        // QString gamePgn;
        // for (int i = 0; i < moveList->count(); i++)
        // {
        //     gamePgn.push_back(moveList->item(i)->text());
        //     gamePgn.push_back(" ");
        // }
        //
        // QGuiApplication::clipboard()->setText(gamePgn.trimmed());
    }
};
