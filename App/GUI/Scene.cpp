#include "Scene.hpp"
#include "../Model/Node.hpp"

GUIScene::GUIScene(const QFont& font, QObject* parent)
    : QGraphicsScene(parent),
      m_font(font),
      m_font_metrics(QFontMetrics(m_font)) {}

void GUIScene::Redraw(std::priority_queue<Node>& pq)
{
    m_scene_nodes.clear();
    clear();

    for (; !pq.empty(); pq.pop()) {
        GUISceneDrawNode(pq.top());
    }
}
