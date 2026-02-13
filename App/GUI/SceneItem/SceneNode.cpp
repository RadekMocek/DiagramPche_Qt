#include <QPainter>

#include "SceneNode.hpp"

SceneNode::SceneNode(const Node& node, const QFont& font, const QFontMetrics& font_metrics)
    : m_node(node), m_font(font), m_font_metrics(font_metrics)
{
    constexpr auto NODE_PADDING = 18;

    // Qt friendly label
    const auto label_text = QString::fromStdString(node.value);

    // This gives us the size of label if we draw it; it's used for implicit rectangle size and for determining the label position
    const auto label_rect = m_font_metrics.boundingRect(QRect(0, 0, 0, 0), // start with no size constraint
                                                        Qt::TextExpandTabs | Qt::TextDontClip,
                                                        label_text);

    // Get explicit or calculate implicit rectangle size
    const auto node_width = (node.width > 0) ? node.width : label_rect.width() + 2 * NODE_PADDING;
    const auto node_height = (node.height > 0) ? node.height : label_rect.height() + 2 * NODE_PADDING;
    const QSizeF size(node_width, node_height);

    // Get node position, this is from the line `xy = [number, number]`
    QPointF position(node.position.x, node.position.y);

    // Move node according to its parent, if the user had set some
    if (!node.position.parent_id.empty()) {
        if (const auto it = m_scene_nodes.find(QString::fromStdString(node.position.parent_id)); it != m_scene_nodes
            .end()) {
            position += it->second->pos() + it->second->GetOffsetFromPivot(node.position.parent_pivot);
        }
    }

    // Move node according to its `pivot`, if the user had set some
    switch (node.pivot) {
    case TOPLEFT:
        break;
    case TOP:
        position.rx() -= size.width() / 2;
        break;
    case TOPRIGHT:
        position.rx() -= size.width();
        break;
    case RIGHT:
        position.rx() -= size.width();
        position.ry() -= size.height() / 2;
        break;
    case BOTTOMRIGHT:
        position.rx() -= size.width();
        position.ry() -= size.height();
        break;
    case BOTTOM:
        position.rx() -= size.width() / 2;
        position.ry() -= size.height();
        break;
    case BOTTOMLEFT:
        position.ry() -= size.height();
        break;
    case LEFT:
        position.ry() -= size.height() / 2;
        break;
    case CENTER:
        position.rx() -= size.width() / 2;
        position.ry() -= size.height() / 2;
        break;
    }

    // Label position
    QPointF label_position = position + QPointF(NODE_PADDING, NODE_PADDING);

    // Custom label position?
    if (node.width > 0 || node.height > 0) {
        // Custom width/height => `text_pos` makes sense
    }
}

QRectF SceneNode::boundingRect() const
{
    return {0.0f, 0.0f, m_size.width(), m_size.height()};
}

void SceneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setFont(m_font);

    const QRectF rect(0, 0, m_size.width(), m_size.height());
    painter->fillRect(rect, m_color);

    painter->drawText(QRect(m_label_position.x(), m_label_position.y(), 0, 0),
                      Qt::TextExpandTabs | Qt::TextDontClip,
                      m_text);

    painter->drawRect(rect);
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
