#include <QFile>
#include <QPlainTextEdit>

#include "../GUI/Main/MainWindow.hpp"

// == Logic for buttons in MainMenuBar ==========================================

void GUIMainWindow::HandleRegularNew()
{
    m_source->clear();
    m_source_filename = std::nullopt;
    m_is_source_dirty = false;
}

void GUIMainWindow::HandleRegularOpen()
{
    if (const auto path = OpenTOMLDialog(); !path.isEmpty()) {
        LoadSourceFromFile(path, false);
    }
}

// == Underlying logic ==========================================================

void GUIMainWindow::LoadSourceFromFile(const QString& filename, const bool is_example)
{
    if (QFile file(filename); file.open(QFile::ReadOnly)) {
        m_source->setPlainText(file.readAll());
    }

    ResetCanvasScrollingAndZoom();

    m_source_filename = (is_example) ? std::nullopt : std::optional{filename};
    m_is_source_dirty = false;
}

bool GUIMainWindow::SaveSourceToFileFromDialog()
{
    if (const auto path = SaveTOMLDialog(); !path.isEmpty()) {
        //TODO
        return true;
    }
    return false;
}
