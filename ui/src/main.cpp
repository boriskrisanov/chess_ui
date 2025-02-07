#include "GameWidget.hpp"
#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Chess");
    window.show();

    return QApplication::exec();
}
