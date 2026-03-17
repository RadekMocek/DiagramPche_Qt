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

// === === === === === === === === ===

class GUIScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GUIScene(const QFont& font, QObject* parent = nullptr);

    void Redraw(std::priority_queue<NodePriority>& nodes_pq,
                const std::unordered_map<std::string, Node>& nodes_map,
                const std::vector<Path>& paths_vec);

    void GUIScenePrepareNode(const Node& node);
    void GUIScenePreparePath(const Path& path);

    [[nodiscard]] QRectF GetSceneAABR() const { return m_scene_aabr.ToQRectF(); }

private:
    QFont m_font;
    QFontMetrics m_font_metrics;

    // Store already prepared nodes, we need their data to prepare dependant nodes
    std::unordered_map<std::string, SceneNode*> m_scene_nodes;

    // For svg export
    AABRHelper m_scene_aabr;
};
