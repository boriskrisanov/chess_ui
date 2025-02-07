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
        vLayout->addWidget(engineInfoWidget);

        vLayout->addWidget(backButton);
        vLayout->addWidget(forwardButton);
        vLayout->addWidget(flipBoardButton);
        vLayout->addWidget(endGameButton);
        vLayout->addWidget(copyPgnButton);
        vLayout->addWidget(gameEndMessage);

        setLayout(vLayout);

        const QFont font = {gameEndMessage->font().family(), 18};
        gameEndMessage->setFont(font);

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

    void gameEnded(std::string reason) const
    {
        gameEndMessage->setText(reason.data());
    }

private:
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    EngineInfoWidget* engineInfoWidget = new EngineInfoWidget(this);

    QPushButton* backButton = new QPushButton("<");
    QPushButton* forwardButton = new QPushButton(">");
    QPushButton* flipBoardButton = new QPushButton("Flip Board");
    QPushButton* endGameButton = new QPushButton("End Game");
    QPushButton* copyPgnButton = new QPushButton("Copy PGN");
    QLabel* gameEndMessage = new QLabel();
signals:
    void flipBoard();
    void undoMove();
    void redoMove();
    void copyPgn();

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

    void copyPgnClicked()
    {
        emit copyPgn();
    }
};
