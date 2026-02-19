#include <QPainter>

#include "SceneNode.hpp"

SceneNode::SceneNode(const SceneNodeCrate& crate) :
    m_aabr(crate.aabr),
    m_color(crate.color),
    m_font(crate.font),
    m_label_position(crate.label_position),
    m_label_value(crate.label_value)
{
    //
}

QRectF SceneNode::boundingRect() const
{
    return m_aabr;
}

void SceneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setFont(m_font);

    painter->fillRect(m_aabr, m_color);

    painter->drawText(QRectF(m_label_position.x(), m_label_position.y(), 0, 0),
                      Qt::TextExpandTabs | Qt::TextDontClip,
                      m_label_value);

    painter->drawRect(m_aabr);
}

QPointF SceneNode::GetExactPointFromPivot(const Pivot pivot) const
{
    return m_aabr.topLeft() + GetOffsetFromPivot(pivot);
}

QPointF SceneNode::GetOffsetFromPivot(const Pivot pivot) const
{
    switch (pivot) {
    default:
        // Unreachable (?), fallthrough
    case PIVOT_TOPLEFT:
        return {0, 0};
    case PIVOT_TOP:
        return {m_aabr.width() / 2, 0};
    case PIVOT_TOPRIGHT:
        return {m_aabr.width(), 0};
    case PIVOT_RIGHT:
        return {m_aabr.width(), m_aabr.height() / 2};
    case PIVOT_BOTTOMRIGHT:
        return {m_aabr.width(), m_aabr.height()};
    case PIVOT_BOTTOM:
        return {m_aabr.width() / 2, m_aabr.height()};
    case PIVOT_BOTTOMLEFT:
        return {0, m_aabr.height()};
    case PIVOT_LEFT:
        return {0, m_aabr.height() / 2};
    case PIVOT_CENTER:
        return {m_aabr.width() / 2, m_aabr.height() / 2};
    }
}
