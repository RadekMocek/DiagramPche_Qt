#include <QFile>
#include <QPlainTextEdit>

#include "../GUI/Main/MainWindow.hpp"
#include "App/GUI/Main/Viewer.hpp"

void GUIMainWindow::LoadSourceFromFile(const char* filename) const
{
    if (QFile file(filename); file.open(QFile::ReadOnly)) {
        m_source->setPlainText(file.readAll());
    }

    m_viewer->ResetCanvasScrollingAndZoom();
}
