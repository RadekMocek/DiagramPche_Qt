#include <QPlainTextEdit>

#include  "../GUI/Main/MainWindow.hpp"

void GUIMainWindow::ReplaceInMSource(const toml::source_region& source, const QString& new_str) const
{
    const auto block_start = m_source->document()->findBlockByLineNumber(static_cast<int>(source.begin.line) - 1);
    const auto block_end = m_source->document()->findBlockByLineNumber(static_cast<int>(source.end.line) - 1);

    QTextCursor cursor(block_start);
    cursor.setPosition(block_start.position() + static_cast<int>(source.begin.column) - 1);
    cursor.setPosition(block_end.position() + static_cast<int>(source.end.column - 1), QTextCursor::KeepAnchor);

    cursor.insertText(new_str);
}

// This method is called when user changes node color/type via toolbar, and the node TOML definition does not contain
// the line with "color=" or "type=" (so default was used). In this situation, there is nothing to replace and we need to
// insert a new line into the source instead (that explicitly sets the color/type). The `end` parameter tells the function
// where the last character of the line "[node.some_id]" is for the particular node. The parameter `new_str` is then inserted
// to that position. It is expected that `new_str` begins with newline character, so the TOML is still valid after the insert.
void GUIMainWindow::InsertNodeParameterInMSource(const toml::source_position& end, const QString& new_str) const
{
    const auto block = m_source->document()->findBlockByLineNumber(static_cast<int>(end.line) - 1);
    QTextCursor cursor(block);
    cursor.setPosition(block.position() + static_cast<int>(end.column));
    cursor.insertText(new_str);
}
