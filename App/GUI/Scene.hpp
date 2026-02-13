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
    explicit GUIScene(QObject* parent = nullptr);

    void Hobluj(std::priority_queue<Node>& pq);

private:
    QFont m_font;
    QFontMetrics m_font_metrics;

    std::unordered_map<std::string, SceneNode*> m_scene_nodes;
};
