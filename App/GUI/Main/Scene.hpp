#pragma once

// Std imports
#include <queue>

// Qt imports
#include <QFontMetrics>
#include <QGraphicsScene>

// App imports
#include "App/Helper/AABR.hpp"

// App fwd declrs
struct Node;
struct NodePriority;
struct Path;
class SceneNode;

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

    [[nodiscard]] QRectF GetSceneAABR() const { return m_scene_aabr.ToQRectF(); }

private:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouse_event) override;

    QFont m_font;
    QFontMetrics m_font_metrics;

    // Store already prepared nodes, we need their data to prepare dependant nodes
    std::unordered_map<std::string, SceneNode*> m_scene_nodes;

    // For svg export
    AABRHelper m_scene_aabr;

signals:
    void EmptySpaceClicked();
    void NodeClicked(const std::string& id);
    void NodeCtrlClicked(const std::string& id);

    void NodeHoverEntered(const std::string& id);
    void NodeHoverLeft();
};
