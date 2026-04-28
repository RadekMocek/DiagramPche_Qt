#pragma once

// Std imports
#include <queue>

// Qt imports
#include <QGraphicsView>
#include <QPointer>

// App imports
#include "../../Helper/AABR.hpp"
#include "../SceneItem/SceneNode.hpp"

// App fwd declrs
struct Node;
struct NodePriority;
struct Path;

// === Scene config == === === === === ===
constexpr auto SCENE_FONT_SIZE_BASE = 18;
constexpr auto NODE_PADDING = 18;
// === === === === === === === === === ===

class GUIScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GUIScene(const QFont& font, QObject* parent = nullptr);

    void Redraw(std::priority_queue<NodePriority>& nodes_pq,
                const std::unordered_map<std::string, Node>& nodes_map,
                const std::vector<Path>& paths_vec);

    void GUIScenePrepareNode(const std::string& node_id, const Node& node);
    void GUIScenePreparePath(const Path& path);

    QRectF GetSceneAABR() const { return m_scene_aabr.ToQRectF(); }

    // Ghost node
    bool IsDraggingNode() const { return m_drag_state.has_value(); }
    bool IsCursorOverViewer() const;
    void UpdateGhostNodePositionAndVisibility(QPointF pos);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouse_event) override;

    QFont m_font;
    QFontMetrics m_font_metrics;

    // Store already prepared nodes, we need their data to prepare dependant nodes
    std::unordered_map<std::string, SceneNode*> m_scene_nodes;

    // For svg export
    AABRHelper m_scene_aabr;

    // Drag'n'drop buttons to add nodes to scene functionality
    std::optional<NodeType> m_drag_state;
    SceneNode m_ghost_node;

    // FPS measure
    QPointer<QTimer> m_fps_timer;
    int m_fps_counter{};
    void drawForeground(QPainter* painter, const QRectF& rect) override;

private slots:
    void FPSTimerShot();

signals:
    void EmptySpaceClicked();
    void NodeClicked(const std::string& id);
    void NodeCtrlClicked(const std::string& id);

    void NodeHoverEntered(const std::string& id);
    void NodeHoverLeft();

    void GhostNodePlaced(NodeType type, QPoint position);

    void FPSInfoTx(int fps);

public slots:
    void OnDragStateChange(std::optional<NodeType> type);
};
