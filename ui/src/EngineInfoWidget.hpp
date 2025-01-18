#pragma once

#include <QCheckBox>
#include <QLabel>

class EngineInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EngineInfoWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        // checkbox->show();
        label->setVisible(true);
    }
private:
    QLabel* label = new QLabel("a", this);
    QCheckBox* checkbox = new QCheckBox("Show engine info", this);
};
