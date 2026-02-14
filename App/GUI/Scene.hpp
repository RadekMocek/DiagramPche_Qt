#pragma once

// Std imports
#include <queue>

// Qt imports
#include <QFontMetrics>
#include <QGraphicsScene>

// App fwd declrs
struct Node;
class SceneNode;

// ===================================
class GUIScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GUIScene(const QFont& font, QObject* parent = nullptr);

    void Redraw(std::priority_queue<Node>& pq);
    void GUISceneDrawNode(const Node& node);

private:
    QFont m_font;
    QFontMetrics m_font_metrics;

    std::unordered_map<std::string, SceneNode*> m_scene_nodes;
};
