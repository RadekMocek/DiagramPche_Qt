#pragma once

// Std imports
#include <queue>
#include <vector>

// Qt imports
#include <QFontMetrics>
#include <QGraphicsScene>

// App fwd declrs
struct Node;
struct Path;
class SceneNode;

// ===================================
class GUIScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GUIScene(const QFont& font, QObject* parent = nullptr);

    void Redraw(std::priority_queue<Node>& nodes_pq, const std::vector<Path>& paths_vec);
    void GUIScenePrepareNode(const Node& node);
    void GUIScenePreparePath(const Path& path);

private:
    QFont m_font;
    QFontMetrics m_font_metrics;

    std::unordered_map<std::string, SceneNode*> m_scene_nodes;
};
