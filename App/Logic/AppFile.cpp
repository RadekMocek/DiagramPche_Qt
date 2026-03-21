#include <QFile>
#include <QPlainTextEdit>

#include "../GUI/Main/MainWindow.hpp"
#include "App/GUI/Main/Viewer.hpp"

// == Logic for buttons in MainMenuBar ==========================================

void GUIMainWindow::HandleRegularNew() const
{
    m_source->clear();
}

// == Underlying logic ==========================================================

void GUIMainWindow::LoadSourceFromFile(const char* filename) const
{
    if (QFile file(filename); file.open(QFile::ReadOnly)) {
        m_source->setPlainText(file.readAll());
    }

    ResetCanvasScrollingAndZoom();
}
