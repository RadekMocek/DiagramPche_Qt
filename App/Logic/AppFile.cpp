#include <QFile>
#include <QPlainTextEdit>

#include "../GUI/MainWindow.hpp"

void GUIMainWindow::LoadSourceFromFile(const char* filename)
{
    if (QFile file(filename); file.open(QFile::ReadOnly)) {
        m_source->setPlainText(file.readAll());
    }
    //m_scrolling = SCROLLING_DEFAULT;
}
