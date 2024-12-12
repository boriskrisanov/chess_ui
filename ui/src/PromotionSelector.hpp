#pragma once

#include <iostream>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include "Move.hpp"

class PromotionSelector : QDialog
{
    Q_OBJECT
public:
    explicit PromotionSelector(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setModal(true);
        layout = new QVBoxLayout(this);
        queenButton = new QPushButton{"Queen"};
        rookButton = new QPushButton{"Rook"};
        bishopButton = new QPushButton{"Bishop"};
        knightButton = new QPushButton{"Knight"};
        layout->addWidget(queenButton);
        layout->addWidget(rookButton);
        layout->addWidget(bishopButton);
        layout->addWidget(knightButton);

        connect(queenButton, SIGNAL (clicked(bool)), this, SLOT (promoteToQueen()));
        connect(rookButton, SIGNAL (clicked(bool)), this, SLOT (promoteToRook()));
        connect(knightButton, SIGNAL (clicked(bool)), this, SLOT (promoteToKnight()));
        connect(bishopButton, SIGNAL (clicked(bool)), this, SLOT (promoteToBishop()));
    }

    int exec() override
    {
        show();
        return QDialog::exec();
    }

    private slots:
    void promoteToQueen()
    {
        QDialog::done(static_cast<int>(MoveFlag::PromotionQueen));
    }

    void promoteToRook()
    {
        QDialog::done(static_cast<int>(MoveFlag::PromotionRook));
    }

    void promoteToBishop()
    {
        QDialog::done(static_cast<int>(MoveFlag::PromotionBishop));
    }

    void promoteToKnight()
    {
        QDialog::done(static_cast<int>(MoveFlag::PromotionKnight));
    }

private:
    QVBoxLayout* layout;
    QPushButton* queenButton;
    QPushButton* rookButton;
    QPushButton* bishopButton;
    QPushButton* knightButton;
};
