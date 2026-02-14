#include "ScenePath.hpp"

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
    painter->setPen(QPen(m_crate.color, 1.1));

    for (const auto& path : m_crate.paths) {
        painter->drawLines(path);
    }
}
