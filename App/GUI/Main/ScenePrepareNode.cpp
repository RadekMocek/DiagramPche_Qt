#include "Scene.hpp"
#include "../../Config.hpp"
#include "../../Helper/Color.hpp"
#include "../../Model/Node.hpp"
#include "../SceneItem/SceneNode.hpp"

void GUIScene::GUIScenePrepareNode(const Node& node)
{
    // Qt friendly label
    const auto label_value = QString::fromStdString(node.value);

    // This gives us the size of label if we draw it; it's used for implicit rectangle size and for determining the label position
    const auto label_rect = m_font_metrics.boundingRect(QRect(0, 0, 0, 0), // Start with no size constraint
                                                        Qt::TextExpandTabs | Qt::TextDontClip, // Make \n, \t work
                                                        label_value);

    // Get explicit or calculate implicit rectangle size
    const auto node_width = (node.width > 0) ? node.width : label_rect.width() + 2 * NODE_PADDING;
    const auto node_height = (node.height > 0) ? node.height : label_rect.height() + 2 * NODE_PADDING;
    const QSizeF size(node_width, node_height);

    // Get node position, this is from the line `xy = [number, number]`
    QPointF position(node.position.x, node.position.y);

    // Move node according to its parent, if the user had set some
    if (!node.position.parent_id.empty()) {
        if (const auto it = m_scene_nodes.find(node.position.parent_id);
            it != m_scene_nodes.end()) {
            position += it->second->GetExactPointFromPivot(node.position.parent_pivot);
        }
    }

    // Move node according to its `pivot`, if the user had set some
    switch (node.pivot) {
    case PIVOT_TOPLEFT:
        break;
    case PIVOT_TOP:
        position.rx() -= size.width() / 2;
        break;
    case PIVOT_TOPRIGHT:
        position.rx() -= size.width();
        break;
    case PIVOT_RIGHT:
        position.rx() -= size.width();
        position.ry() -= size.height() / 2;
        break;
    case PIVOT_BOTTOMRIGHT:
        position.rx() -= size.width();
        position.ry() -= size.height();
        break;
    case PIVOT_BOTTOM:
        position.rx() -= size.width() / 2;
        position.ry() -= size.height();
        break;
    case PIVOT_BOTTOMLEFT:
        position.ry() -= size.height();
        break;
    case PIVOT_LEFT:
        position.ry() -= size.height() / 2;
        break;
    case PIVOT_CENTER:
        position.rx() -= size.width() / 2;
        position.ry() -= size.height() / 2;
        break;
    }

    // Final position rect
    const QRectF rect(position, size);

    // Label position
    QPointF label_position = position + QPointF(NODE_PADDING, NODE_PADDING);

    // Custom label position?
    if (node.width > 0 || node.height > 0) {
        // Custom width/height => `label_pos` makes sense
        switch (node.label_position) {
        case PIVOT_TOPLEFT:
            break;
        case PIVOT_TOP:
            label_position = {
                rect.center().x() - label_rect.width() / 2.0,
                label_position.y()
            };
            break;
        case PIVOT_TOPRIGHT:
            label_position = {
                rect.bottomRight().x() - label_rect.width() - NODE_PADDING,
                label_position.y()
            };
            break;
        case PIVOT_RIGHT:
            label_position = {
                rect.bottomRight().x() - label_rect.width() - NODE_PADDING,
                rect.center().y() - label_rect.height() / 2.0
            };
            break;
        case PIVOT_BOTTOMRIGHT:
            label_position = {
                rect.bottomRight().x() - label_rect.width() - NODE_PADDING,
                rect.bottomRight().y() - label_rect.height() - NODE_PADDING
            };
            break;
        case PIVOT_BOTTOM:
            label_position = {
                rect.center().x() - label_rect.width() / 2.0,
                rect.bottomRight().y() - label_rect.height() - NODE_PADDING
            };
            break;
        case PIVOT_BOTTOMLEFT:
            label_position = {
                label_position.x(),
                rect.bottomRight().y() - label_rect.height() - NODE_PADDING
            };
            break;
        case PIVOT_LEFT:
            label_position = {
                label_position.x(),
                rect.center().y() - label_rect.height() / 2.0
            };
            break;
        case PIVOT_CENTER:
            label_position = {
                rect.center().x() - label_rect.width() / 2.0,
                rect.center().y() - label_rect.height() / 2.0
            };
            break;
        }
    }

    // Create new `QGraphicsItem` of type `SceneNode` by passing `SceneNodeCrate` to ctor
    auto* item = new SceneNode({rect, GetQColorFromTuple(node.color), m_font, label_position, label_value, node.type});
    item->setZValue(DLUserChannelToRealChannel(node.z, true));
    addItem(item);
    m_scene_nodes.insert({node.id, item});

    //
    m_scene_aabr.UpdateWithQRectF(rect);
}
