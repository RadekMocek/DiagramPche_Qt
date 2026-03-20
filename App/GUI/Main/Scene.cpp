#include <QGraphicsSceneMouseEvent>

#include "Scene.hpp"
#include "../../Model/Node.hpp"
#include "../../Model/Path.hpp"
#include "../../Logic/TOML/NodePriority.hpp"
#include "../SceneItem/SceneNode.hpp"

// (Values for the ghost node are precalculated)
inline QColor GHOST_NODE_COLOR_EDGE = QColor::fromRgb(0, 0, 0, 128);
inline QColor GHOST_NODE_COLOR_FILL = QColor::fromRgb(255, 255, 255, 128);
constexpr auto GHOST_NODE_WIDTH = 99;
constexpr auto GHOST_NODE_HEIGHT = 54;
constexpr auto GHOST_OFFSET = QPointF(-GHOST_NODE_WIDTH / 2, -GHOST_NODE_HEIGHT / 2);
constexpr auto GHOST_NODE_AABR = QRectF(GHOST_OFFSET, QSizeF(GHOST_NODE_WIDTH, GHOST_NODE_HEIGHT));
constexpr auto GHOST_NODE_LABEL_POSITION = QPointF(18, 18) + GHOST_OFFSET;

GUIScene::GUIScene(const QFont& font, QObject* parent) :
    QGraphicsScene(parent),
    m_font(font),
    m_font_metrics(QFontMetrics(m_font)),
    m_drag_state(std::nullopt),
    m_ghost_node({
        "Zagmolol Bool",
        GHOST_NODE_AABR,
        GHOST_NODE_COLOR_FILL,
        GHOST_NODE_COLOR_EDGE,
        m_font,
        GHOST_NODE_LABEL_POSITION,
        "",
        NTYPE_RECTANGLE
    })
{
    m_ghost_node.setFlag(QGraphicsItem::ItemIsMovable, false);
}

void GUIScene::Redraw(std::priority_queue<NodePriority>& nodes_pq,
                      const std::unordered_map<std::string, Node>& nodes_map,
                      const std::vector<Path>& paths_vec)
{
    m_scene_nodes.clear();
    clear();

    m_scene_aabr.Reset();

    for (; !nodes_pq.empty(); nodes_pq.pop()) {
        const auto& [node_draw_batch_number, node_id] = nodes_pq.top();
        GUIScenePrepareNode(node_id, nodes_map.at(node_id));
    }

    for (const auto& path : paths_vec) {
        GUIScenePreparePath(path);
    }

    //m_scene_aabr.DebugPrint();
}

void GUIScene::mousePressEvent(QGraphicsSceneMouseEvent* mouse_event)
{
    if (mouse_event->button() == Qt::LeftButton) {
        // Cycle through scene items under cursor ordered by Z
        for (auto* item : items(mouse_event->scenePos())) {
            // Check if item is node
            if (const auto scene_node = dynamic_cast<SceneNode*>(item)) {
                // Handle click
                if (mouse_event->modifiers() & Qt::ControlModifier) {
                    // Ctrl + LMB
                    emit NodeCtrlClicked(scene_node->GetID());
                }
                else {
                    // LMB
                    emit NodeClicked(scene_node->GetID());
                }

                // Stop at first (topmost) node found
                return;
            }
        }

        // If we did not return, user clicked at empty space
        emit EmptySpaceClicked();
    }
    QGraphicsScene::mousePressEvent(mouse_event);
}

void GUIScene::OnDragStateChange(const std::optional<NodeType> type)
{
    m_drag_state = type;

    if (m_drag_state.has_value()) {
        m_ghost_node.UpdateGhostNode("node_xx", m_drag_state.value());
        addItem(&m_ghost_node);
    }
    else {
        //todo
        qDebug() << "Node dropped";
        removeItem(&m_ghost_node);
    }
}
