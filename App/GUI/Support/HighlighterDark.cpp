#include "HighlighterDark.hpp"

HighlighterDark::HighlighterDark(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    // Inspired by https://docs.rs/egui_code_editor/0.2.21/egui_code_editor/struct.ColorTheme.html#associatedconstant.SONOKAI
    // (https://github.com/sainnhe/sonokai)
    // (https://github.com/p4ymak/egui_code_editor)
    const QColor COLOR_KEYWORD = QColor::fromRgb(252, 93, 124, 255);
    const QColor COLOR_IDENTIFIER = QColor::fromRgb(57, 158, 230, 255);
    const QColor COLOR_COMMENT = QColor::fromRgb(127, 132, 144, 255);
    const QColor COLOR_STRING = QColor::fromRgb(231, 198, 100, 255);
    const QColor COLOR_NUMBER = QColor::fromRgb(179, 157, 243, 255);

    // = "Keywords" =
    m_format_keyword.setForeground(COLOR_KEYWORD);
    m_format_keyword.setFontWeight(QFont::Bold);
    const QString patterns_keyword[] = {
        QStringLiteral("\\bvariables\\b"),
        QStringLiteral("\\bnode\\b"),
        QStringLiteral("\\bpath\\b"),
    };
    for (const auto& pattern : patterns_keyword) {
        m_highlighting_rules.append({QRegularExpression(pattern), m_format_keyword});
    }

    // = "Identifiers" =
    m_format_identifier.setForeground(COLOR_IDENTIFIER);
    const QString patterns_identifier[] = {
        QStringLiteral("\\bcolor\\b"),
        QStringLiteral("\\bcolor_border\\b"),
        QStringLiteral("\\bend\\b"),
        QStringLiteral("\\bends\\b"),
        QStringLiteral("\\blabel\\b"),
        QStringLiteral("\\blabel_bg\\b"),
        QStringLiteral("\\blabel_pos\\b"),
        QStringLiteral("\\blabel_shift\\b"),
        QStringLiteral("\\bpivot\\b"),
        QStringLiteral("\\bpoints\\b"),
        QStringLiteral("\\bshift\\b"),
        QStringLiteral("\\bsize\\b"),
        QStringLiteral("\\bstart\\b"),
        QStringLiteral("\\btips\\b"),
        QStringLiteral("\\btype\\b"),
        QStringLiteral("\\bvalue\\b"),
        QStringLiteral("\\bxy\\b"),
        QStringLiteral("\\bz\\b"),
    };
    for (const auto& pattern : patterns_identifier) {
        m_highlighting_rules.append({QRegularExpression(pattern), m_format_identifier});
    }

    // = Numbers =
    m_format_number.setForeground(COLOR_NUMBER);
    m_highlighting_rules.append({QRegularExpression(QStringLiteral("\\b[0-9]+\\b")), m_format_number});

    // = Comment =
    m_format_comment.setForeground(COLOR_COMMENT);
    m_highlighting_rules.append({QRegularExpression(QStringLiteral("#[^\n]*")), m_format_comment});

    // = String =
    m_format_string.setForeground(COLOR_STRING);
    m_highlighting_rules.append({QRegularExpression(QStringLiteral("\".*\"")), m_format_string});
    // String must be set after comment otherwise '#' inside string would count as a comment
    // But now when we have comment with a string, the string is still green instead fo gray
}

void HighlighterDark::highlightBlock(const QString& text)
{
    for (const auto& [pattern, format] : std::as_const(m_highlighting_rules)) {
        QRegularExpressionMatchIterator matchIterator = pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(static_cast<int>(match.capturedStart()),
                      static_cast<int>(match.capturedLength()),
                      format);
        }
    }
}
