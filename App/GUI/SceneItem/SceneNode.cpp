#include <QPainter>

#include "SceneNode.hpp"

SceneNode::SceneNode(const SceneNodeCrate& crate) :
    m_crate(crate)
{
    //
}

QRectF SceneNode::boundingRect() const
{
    return m_crate.aabr;
}

void SceneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setFont(m_crate.font);

    painter->fillRect(m_crate.aabr, m_crate.color);

    painter->drawText(QRectF(m_crate.label_position.x(), m_crate.label_position.y(), 0, 0),
                      Qt::TextExpandTabs | Qt::TextDontClip,
                      m_crate.label_value);

    painter->drawRect(m_crate.aabr);
}

QPointF SceneNode::GetExactPointFromPivot(const Pivot pivot) const
{
    return m_crate.aabr.topLeft() + GetOffsetFromPivot(pivot);
}

QPointF SceneNode::GetOffsetFromPivot(const Pivot pivot) const
{
    switch (pivot) {
    default:
        // Unreachable (?), fallthrough
    case PIVOT_TOPLEFT:
        return {0, 0};
    case PIVOT_TOP:
        return {m_crate.aabr.width() / 2, 0};
    case PIVOT_TOPRIGHT:
        return {m_crate.aabr.width(), 0};
    case PIVOT_RIGHT:
        return {m_crate.aabr.width(), m_crate.aabr.height() / 2};
    case PIVOT_BOTTOMRIGHT:
        return {m_crate.aabr.width(), m_crate.aabr.height()};
    case PIVOT_BOTTOM:
        return {m_crate.aabr.width() / 2, m_crate.aabr.height()};
    case PIVOT_BOTTOMLEFT:
        return {0, m_crate.aabr.height()};
    case PIVOT_LEFT:
        return {0, m_crate.aabr.height() / 2};
    case PIVOT_CENTER:
        return {m_crate.aabr.width() / 2, m_crate.aabr.height() / 2};
    }
}
