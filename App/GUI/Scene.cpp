#include "Scene.hpp"
#include "../Model/Node.hpp"
#include "../Model/Path.hpp"

GUIScene::GUIScene(const QFont& font, QObject* parent) :
    QGraphicsScene(parent),
    m_font(font),
    m_font_metrics(QFontMetrics(m_font))
{
    //
}

void GUIScene::Redraw(std::priority_queue<Node>& nodes_pq, const std::vector<Path>& paths_vec)
{
    m_scene_nodes.clear();
    clear();

    for (; !nodes_pq.empty(); nodes_pq.pop()) {
        GUIScenePrepareNode(nodes_pq.top());
    }

    for (const auto& path : paths_vec) {
        GUIScenePreparePath(path);
    }
}
