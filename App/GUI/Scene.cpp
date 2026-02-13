#include "Scene.hpp"
#include "../Model/Node.hpp"
#include "App/Helper/Print.hpp"
#include "SceneItem/SceneNode.hpp"

GUIScene::GUIScene(QObject* parent) : QGraphicsScene(parent),
                                      m_font(QFont("Inconsolata", 18)),
                                      m_font_metrics(QFontMetrics(m_font)) {}

void GUIScene::Hobluj(std::priority_queue<Node>& pq)
{
    m_scene_nodes.clear();
    clear();

    for (; !pq.empty(); pq.pop()) {
        const auto& node = pq.top();
        auto* item = new SceneNode(node, m_font, m_font_metrics);
        addItem(item);
        m_scene_nodes.insert({node.id, item});
    }
}
