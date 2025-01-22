#pragma once

#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>

class EngineInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EngineInfoWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        groupBoxVLayout->addWidget(evalText);
        groupBoxVLayout->addWidget(depthText);
        groupBoxVLayout->addWidget(positionsEvaluatedText);
        groupBox->setLayout(groupBoxVLayout);
        vLayout->addWidget(groupBox);
    }
public slots:
    void onSearchDone(SearchResult searchResult) const
    {
        evalText->setText(QString::fromStdString("Eval: " + formatEval(searchResult.standardEval())));
        depthText->setText(QString::fromStdString("Depth: " + std::to_string(searchResult.depthSearched)));
        positionsEvaluatedText->setText(QString::fromStdString("Positions Evaluated: " + std::to_string(searchResult.debugStats.positionsEvaluated)));
    }
private:
    static std::string formatEval(double eval)
    {
        std::string evalString = QString::number(eval, 'g', 3).toStdString();
        if (eval > 0)
        {
            evalString = "+" + evalString;
        }
        return evalString;
    }

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QVBoxLayout* groupBoxVLayout = new QVBoxLayout();
    QGroupBox* groupBox = new QGroupBox("Engine Info");
    QLabel* evalText = new QLabel();
    QLabel* depthText = new QLabel();
    QLabel* positionsEvaluatedText = new QLabel();
};
