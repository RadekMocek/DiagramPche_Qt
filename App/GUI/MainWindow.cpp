#include <QMenuBar>
#include <QMessageBox>

#include  "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Untitled – DiagramPche :: Qt");
    resize(1366, 768);

    InitMainMenuBar();
}

void MainWindow::InitMainMenuBar()
{
    const QPointer main_menu_bar = new QMenuBar(this);
    setMenuBar(main_menu_bar);

    // .: Help :.
    const QPointer help_menu = main_menu_bar->addMenu("Help");
    const QPointer about_action = help_menu->addAction("About");
    connect(about_action, &QAction::triggered, [this] {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("About");
        msgBox.setTextFormat(Qt::RichText); // For <a href> to work
        msgBox.setText(
            "DiagramPche :: Qt<br><a href='https://github.com/RadekMocek/DiagramPche_Qt'>https://github.com/RadekMocek/DiagramPche_Qt</a>");
        msgBox.exec();
    });
}
