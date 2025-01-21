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
        hLayout->addWidget(evalText);
    }
public slots:
    void onSearchDone(SearchResult searchResult)
    {
        evalText->setText(QString::fromStdString("Eval = " + formatEval(searchResult.standardEval())));
    }
private:
    std::string formatEval(double eval)
    {
        std::string evalString = QString::number(eval, 'g', 3).toStdString();
        if (eval > 0)
        {
            evalString = "+" + evalString;
        }
        return evalString;
    }

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    QLabel* evalText = new QLabel();
};
