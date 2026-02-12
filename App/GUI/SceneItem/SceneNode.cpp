#include "SceneNode.hpp"
#include "../../Model/Node.hpp"

SceneNode::SceneNode(const QFont& font, const QString& text, const QSizeF size)
    : m_font(font),
      m_text(text),
      m_size(size) {}

QRectF SceneNode::boundingRect() const
{
    return {0.0f, 0.0f, m_size.width(), m_size.height()};
}

void SceneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setFont(m_font);

    painter->drawText(QRect(0, 0, m_size.width(), m_size.height()),
                      Qt::TextExpandTabs | Qt::TextDontClip,
                      m_text);

    painter->drawRect(0, 0, m_size.width(), m_size.height());
}

QPointF SceneNode::GetOffsetFromPivot(const Pivot pivot) const
{
    switch (pivot) {
    default:
        // Unreachable (?), fallthrough
    case TOPLEFT:
        return {0, 0};
    case TOP:
        return {m_size.width() / 2, 0};
    case TOPRIGHT:
        return {m_size.width(), 0};
    case RIGHT:
        return {m_size.width(), m_size.height() / 2};
    case BOTTOMRIGHT:
        return {m_size.width(), m_size.height()};
    case BOTTOM:
        return {m_size.width() / 2, m_size.height()};
    case BOTTOMLEFT:
        return {0, m_size.height()};
    case LEFT:
        return {0, m_size.height() / 2};
    case CENTER:
        return {m_size.width() / 2, m_size.height() / 2};
    }
}
