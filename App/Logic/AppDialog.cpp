#include <QFileDialog>

#include "../GUI/Main/MainWindow.hpp"

QString GUIMainWindow::SaveTOMLDialog()
{
    return QFileDialog::getSaveFileName(this, "Save diagram to file", "diagram.toml", "Diagram (*.toml)");
}

QString GUIMainWindow::OpenTOMLDialog()
{
    return QFileDialog::getOpenFileName(this, "Open diagram file", "", "Diagram (*.toml)");
}
