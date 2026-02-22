#include <QApplication>
#include <QFontDatabase>

#include "App/Config.hpp"
#include "App/GUI/Main/MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QFontDatabase::addApplicationFont("./Resource/Font/Inconsolata-Medium.ttf");

    QFont default_font;
    default_font.setFamily("Inconsolata");
    default_font.setPixelSize(FONT_SIZE_DEFAULT);
    QApplication::setFont(default_font);

    app.setWindowIcon(QIcon("./Resource/Icon/icon-256.png"));

    GUIMainWindow window;
    window.show();

    return QApplication::exec();
}
