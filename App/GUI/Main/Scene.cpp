#include <QGraphicsSceneMouseEvent>

#include "Scene.hpp"
#include "../../Model/Node.hpp"
#include "../../Model/Path.hpp"
#include "../../Logic/TOML/NodePriority.hpp"
#include "../SceneItem/SceneNode.hpp"

// (Values for the ghost node are precalculated)
inline QColor GHOST_NODE_COLOR_EDGE = QColor::fromRgb(0, 0, 0, 128);
inline QColor GHOST_NODE_COLOR_FILL = QColor::fromRgb(255, 255, 255, 128);
constexpr auto GHOST_NODE_WIDTH = 108;
constexpr auto GHOST_NODE_HEIGHT = 54;
constexpr auto GHOST_OFFSET = QPoint(-GHOST_NODE_WIDTH / 2, -GHOST_NODE_HEIGHT / 2);
constexpr auto GHOST_NODE_AABR = QRect(GHOST_OFFSET, QSize(GHOST_NODE_WIDTH, GHOST_NODE_HEIGHT));
constexpr auto GHOST_NODE_LABEL_POSITION = QPoint(18, 18) + GHOST_OFFSET;

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

bool GUIScene::IsCursorOverViewer() const
{
    for (const auto* view : views()) {
        // EDIT: this was resolved by making the scene/view large and hiding the scrollbars
        /*
        // Padding so ghost node disappears when it would not 100% fit into the currently visible canvas area
        // The main motivation for doing this was that the scene view does not move, but it still moves...
        const qreal scale = view->transform().m11();
        const auto pad_value_horizontal = GHOST_NODE_WIDTH / 2 * scale;
        const auto pad_value_vertical = GHOST_NODE_HEIGHT / 2 * scale;
        const auto padding = QMargins(pad_value_horizontal, pad_value_vertical,
                                      pad_value_horizontal, pad_value_vertical);
        */
        if (view->rect().contains(view->mapFromGlobal(QCursor::pos()))) {
            return true;
        }
    }
    return false;
}

void GUIScene::UpdateGhostNodePositionAndVisibility(const QPointF pos)
{
    if (IsCursorOverViewer()) {
        m_ghost_node.setPos(pos);
        m_ghost_node.setVisible(true);
    }
    else {
        m_ghost_node.setVisible(false);
    }
}

void GUIScene::OnDragStateChange(const std::optional<NodeType> type)
{
    // Type has value => dragging the button has started
    if (type.has_value()) {
        m_drag_state = type;
        m_ghost_node.UpdateGhostNode("new_node", m_drag_state.value());
        addItem(&m_ghost_node);
    }
    // Type is nullopt but we have the "drag start type" stored in `m_drag_state` => LMB was released
    else if (m_drag_state.has_value()) {
        // We store this before changing to nullopt; it will be send with the signal
        const auto drag_start_type = m_drag_state.value();
        // Change to nullopt
        m_drag_state = type;
        // Remove ghost node BEFORE sending the signal
        // (the signal will change textedit and that will clear the whole scene, can't remove something from an empty scene)
        removeItem(&m_ghost_node);
        // Emit signal to add node to diagram
        if (IsCursorOverViewer()) {
            QPointF cursor_position_in_scene;
            for (const auto* view : views()) {
                cursor_position_in_scene = view->mapToScene(view->mapFromGlobal(QCursor::pos()));
            }
            const QPoint final_position = cursor_position_in_scene.toPoint() + GHOST_OFFSET;

            emit GhostNodePlaced(drag_start_type, final_position);
        }
    }
    // This should not happen, but if it does, I will know
    else {
        qDebug() << "GUIScene::OnDragStateChange unreachable!?";
    }
}
