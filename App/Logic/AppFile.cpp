#include <QFile>
#include <QMessageBox>
#include <QPlainTextEdit>

#include "../GUI/Main/MainWindow.hpp"

// == Logic for buttons in MainMenuBar ==========================================

void GUIMainWindow::HandleRegularNew()
{
    m_source->clear();
    SetMSourceFilename(std::nullopt);
    setWindowModified(false);
    ResetCanvasScrollingAndZoom();
}

void GUIMainWindow::HandleRegularOpen()
{
    if (const auto path = OpenTOMLDialog(); !path.isEmpty()) {
        LoadSourceFromFile(path, false);
    }
}

bool GUIMainWindow::HandleRegularSave()
{
    if (!GetMSourceFilename().has_value()) {
        return SaveSourceToFileFromDialog();
    }
    // Else
    if (SaveSourceToFile(GetMSourceFilename().value())) {
        setWindowModified(false);
        return true;
    }
    return false;
}

void GUIMainWindow::HandleOpenExample(const QString& filename)
{
    if (!isWindowModified()) {
        LoadSourceFromFile(filename, true);
    }
    else {
        switch (UnsavedWarningDialog()) {
        case QMessageBox::Save:
            if (HandleRegularSave()) {
                LoadSourceFromFile(filename, true);
            }
            break;
        case QMessageBox::Discard:
            LoadSourceFromFile(filename, true);
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
        }
    }
}

// == Underlying logic ==========================================================

void GUIMainWindow::LoadSourceFromFile(const QString& filename, const bool is_example)
{
    if (QFile file(filename); file.open(QFile::ReadOnly)) {
        m_source->setPlainText(file.readAll());
    }

    ResetCanvasScrollingAndZoom();

    SetMSourceFilename((is_example) ? std::nullopt : std::optional{filename});
    setWindowModified(false);
}

bool GUIMainWindow::SaveSourceToFile(const QString& filename) const
{
    QFile outf;
    outf.setFileName(filename);
    if (outf.open(QIODevice::WriteOnly)) {
        QTextStream outs(&outf);
        outs << m_source->toPlainText();
        outf.flush();
        outf.close();
        return true;
    }
    return false;
}

bool GUIMainWindow::SaveSourceToFileFromDialog()
{
    if (const auto path = SaveTOMLDialog(); !path.isEmpty()) {
        if (SaveSourceToFile(path)) {
            SetMSourceFilename(path);
            setWindowModified(false);
            return true;
        }
    }
    return false;
}
