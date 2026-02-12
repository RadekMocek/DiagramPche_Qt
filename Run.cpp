#include <QApplication>

#include "App/GUI/MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GUIMainWindow window;
    window.show();

    return QApplication::exec();
}
