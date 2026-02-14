#include <QApplication>
#include <QFontDatabase>

#include "App/Config.hpp"
#include "App/GUI/MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QFontDatabase::addApplicationFont(":Resource/Font/Inconsolata-Medium.ttf");

    QFont default_font;
    default_font.setFamily("Inconsolata Medium");
    default_font.setPixelSize(FONT_SIZE_DEFAULT);
    app.setFont(default_font);

    GUIMainWindow window;
    window.show();

    return QApplication::exec();
}
