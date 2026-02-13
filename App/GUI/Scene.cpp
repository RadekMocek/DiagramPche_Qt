#include "Scene.hpp"
#include "../Model/Node.hpp"
#include "App/Helper/Print.hpp"
#include "SceneItem/SceneNode.hpp"

GUIScene::GUIScene(QObject* parent) : QGraphicsScene(parent),
                                      m_font(QFont("Inconsolata", 18)),
                                      m_font_metrics(QFontMetrics(m_font)) {}

void GUIScene::Hobluj(std::priority_queue<Node>& pq)
{
    m_scene_nodes.clear();
    clear();

    for (; !pq.empty(); pq.pop()) {
        constexpr auto NODE_PADDING = 18;
        const auto& node = pq.top();

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

        auto* item = new SceneNode({QRectF(position, size), node.color, m_font, label_position, label_value});
        addItem(item);
        m_scene_nodes.insert({node.id, item});
    }
}
