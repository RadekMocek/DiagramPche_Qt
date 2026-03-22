#include <QApplication>
#include <QStyleFactory>

#include "App/GUI/Main/MainWindow.hpp"

int main(int argc, char* argv[])
{
    // --- App config  --- --- --- --- --- ---
    constexpr auto FONT_SIZE_DEFAULT = 20;
    // --- --- --- --- --- --- --- --- --- ---

    QApplication app(argc, argv);

    /*
    qDebug() << QStyleFactory::keys();
    app.setStyle(QStyleFactory::create("windowsvista"));
    */

    QFontDatabase::addApplicationFont("./Resource/Font/Inconsolata-Medium.ttf");

    QFont default_font;
    default_font.setFamily(FONT_FAMILY_DEFAULT);
    default_font.setPixelSize(FONT_SIZE_DEFAULT);
    QApplication::setFont(default_font);

    app.setWindowIcon(QIcon("./Resource/Icon/icon-256.png"));

    GUIMainWindow window;
    window.show();

    return QApplication::exec();
}
