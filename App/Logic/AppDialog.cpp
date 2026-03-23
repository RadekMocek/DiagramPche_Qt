#include <QFileDialog>
#include <QMessageBox>

#include "../GUI/Main/MainWindow.hpp"

QString GUIMainWindow::SaveTOMLDialog()
{
    return QFileDialog::getSaveFileName(this, "Save diagram to file", "diagram.toml", "Diagram (*.toml)");
}

QString GUIMainWindow::OpenTOMLDialog()
{
    return QFileDialog::getOpenFileName(this, "Open diagram file", "", "Diagram (*.toml)");
}

int GUIMainWindow::UnsavedWarningDialog()
{
    const auto filename = (GetMSourceFilename().has_value()) ? GetMSourceFilename().value() : "Untitled";

    QMessageBox dialog;
    dialog.setText("You have unsaved changes");
    dialog.setInformativeText(QString("Do you want to save changes to '%1'?").arg(filename));
    dialog.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    dialog.setDefaultButton(QMessageBox::Save);
    return dialog.exec();
}
