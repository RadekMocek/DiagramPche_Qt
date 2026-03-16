#include "Scene.hpp"
#include "../../Model/Node.hpp"
#include "../../Model/Path.hpp"
#include "../../Logic/TOML/NodePriority.hpp"

GUIScene::GUIScene(const QFont& font, QObject* parent) :
    QGraphicsScene(parent),
    m_font(font),
    m_font_metrics(QFontMetrics(m_font))
{
    //
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
        GUIScenePrepareNode(nodes_map.at(node_id));
    }

    for (const auto& path : paths_vec) {
        GUIScenePreparePath(path);
    }

    //m_scene_aabr.DebugPrint();
}
