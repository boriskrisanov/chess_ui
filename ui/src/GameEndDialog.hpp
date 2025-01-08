#pragma once

#include <QDialog>
#include <qlabel.h>
#include <qmessagebox.h>

class GameEndDialog : public QMessageBox
{
    Q_OBJECT
public:
    explicit GameEndDialog(QWidget* parent = nullptr)
        : QMessageBox(parent)
    {
        setModal(true);
        setText("End");
    }

    int exec() override
    {
        show();
        return QMessageBox::exec();
    }
};
