#include "SceneNode.hpp"
#include "../../Helper/Color.hpp"
#include "../Main/Scene.hpp"

SceneNode::SceneNode(SceneNodeCrate crate) :
    m_crate(std::move(crate))
{
    setAcceptHoverEvents(true);
}

QRectF SceneNode::boundingRect() const
{
    return m_crate.aabr;
}

void SceneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    const auto& [id, aabr, color_fill, color_edge, font, label_position, label_value, type] = m_crate;

    painter->setFont(font);
    painter->setPen(QPen(color_edge));
    painter->setBrush(color_fill);

    // Debug purposes
    //painter->drawRect(aabr);

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

    // Switch back to black text
    painter->setPen(QPen(COLOR_BLACK));
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

void SceneNode::UpdateGhostNode(const QString& label_value, const NodeType type)
{
    m_crate.label_value = label_value;
    m_crate.type = type;
}

void SceneNode::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    //DebugPrint();
    emit dynamic_cast<GUIScene*>(scene())->NodeHoverEntered(m_crate.id);
    QGraphicsItem::hoverEnterEvent(event);
}

void SceneNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    emit dynamic_cast<GUIScene*>(scene())->NodeHoverLeft();
    QGraphicsItem::hoverLeaveEvent(event);
}
