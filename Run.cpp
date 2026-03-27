#include <QApplication>
#include <QStyleFactory>
#include <QStyleHints>

#include "App/GUI/Main/MainWindow.hpp"
#include "App/Helper/CPU.hpp"

int main(int argc, char* argv[])
{
    // Init CPU measuring for benchmarking purposes
    CPUStats::Init();

    // --- App config  --- --- --- --- --- ---
    constexpr auto FONT_SIZE_DEFAULT = 20;
    // --- --- --- --- --- --- --- --- --- ---

    QApplication app(argc, argv);

    if (QStyleFactory::keys().contains("Fusion")) {
        QApplication::setStyle(QStyleFactory::create("Fusion"));
    }
    else {
        qWarning() << "Fusion style not supported. Falling back to default style.";
    }

    // Default to light mode
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);

    QFontDatabase::addApplicationFont("./Resource/Font/Inconsolata-Medium.ttf");

    QFont default_font;
    default_font.setFamily(FONT_FAMILY_DEFAULT);
    default_font.setPixelSize(FONT_SIZE_DEFAULT);
    QApplication::setFont(default_font);

    QApplication::setWindowIcon(QIcon("./Resource/Icon/icon-256.png"));

    GUIMainWindow window;
    window.show();

    return QApplication::exec();
}
