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
    painter->setRenderHint(QPainter::Antialiasing, true);

    const auto& [aabr, color, font, label_position, label_value, type] = m_crate;

    painter->setFont(font);
    painter->setBrush(color); // Fill color

    switch (type) {
    case NTYPE_RECTANGLE:
        painter->drawRect(aabr);
        break;
    case NTYPE_ELLIPSE:
        painter->drawEllipse(aabr);
        break;
    case NTYPE_DIAMOND:
        {
            QPolygon polygon;
            polygon << GetExactPointFromPivot(PIVOT_TOP).toPoint()
                << GetExactPointFromPivot(PIVOT_RIGHT).toPoint()
                << GetExactPointFromPivot(PIVOT_BOTTOM).toPoint()
                << GetExactPointFromPivot(PIVOT_LEFT).toPoint();
            painter->drawConvexPolygon(polygon);
        }
        break;
    case NTYPE_TEXT:
        // Do nothing
        break;
    }

    painter->drawText(QRectF(label_position.x(), label_position.y(), 0, 0),
                      Qt::TextExpandTabs | Qt::TextDontClip,
                      label_value);
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
