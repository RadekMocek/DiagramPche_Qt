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
    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setPen(QPen(m_crate.color)); // Line color
    painter->setBrush(m_crate.color); // Fill color (for the arrow tips)

    for (const auto& path : m_crate.paths) {
        painter->drawLines(path);

        if (path.length() >= 2) {
            if (m_crate.do_start_arrow) {
                DrawArrowTip(painter, path[1], path[0]);
            }
            if (m_crate.do_end_arrow) {
                DrawArrowTip(painter, path[path.length() - 2], path[path.length() - 1]);
            }
        }
    }

    if (m_crate.do_path_label) {
        painter->setBrush(m_crate.path_label_bg_color);
        for (const auto& path_label : m_crate.path_label_top_lefts) {
            const QRectF rect(path_label, m_crate.path_label_rect.size());

            painter->fillRect(rect, m_crate.path_label_bg_color);

            painter->drawText(QRectF(path_label.x(), path_label.y(), 0, 0),
                              Qt::TextExpandTabs | Qt::TextDontClip,
                              m_crate.path_label_value);
        }
    }
}
