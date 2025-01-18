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
        // TODO
    }
private:
    QCheckBox* checkbox = new QCheckBox("Show engine info", this);
};
