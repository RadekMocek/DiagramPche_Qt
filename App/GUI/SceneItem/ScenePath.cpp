#include "ScenePath.hpp"
#include "../../Helper/Draw.hpp"

#include <QPainter>

ScenePath::ScenePath(const ScenePathCrate& crate) :
    m_crate(crate)
{
    //
}

QRectF ScenePath::boundingRect() const
{
    return m_crate.aabr;
}

void ScenePath::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(QPen(m_crate.color, 1.1)); // Line color
    painter->setBrush(m_crate.color); // Fill color

    for (const auto& path : m_crate.paths) {
        painter->drawLines(path);

        if (m_crate.do_end_arrow && path.length() >= 2) {
            DrawArrowTip(painter, path[path.length() - 2], path[path.length() - 1]);
        }
    }

    if (m_crate.do_start_arrow && !m_crate.paths.empty() && m_crate.paths[0].length() >= 2) {
        DrawArrowTip(painter, m_crate.paths[0][1], m_crate.paths[0][0]);
    }
}
