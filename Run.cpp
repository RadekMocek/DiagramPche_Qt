#include <QApplication>

#include "App/GUI/MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return QApplication::exec();
}
