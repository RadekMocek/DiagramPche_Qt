#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>

// === === === === === === === === === === ===

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit Highlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<HighlightingRule> m_highlighting_rules;

    QTextCharFormat m_format_keyword;
    QTextCharFormat m_format_identifier;
    QTextCharFormat m_format_number;
    QTextCharFormat m_format_comment;
    QTextCharFormat m_format_string;
};
