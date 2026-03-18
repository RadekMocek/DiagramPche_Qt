#pragma once

#include <QColor>
#include <QFont>
#include <QList>
#include <QRectF>

struct ScenePathCrate
{
    QRectF aabr;
    QList<QList<QPointF>> paths;
    QColor color;
    bool do_start_arrow{};
    bool do_end_arrow{};
    // Path label
    bool do_path_label;
    QFont path_label_font;
    QString path_label_value;
    QColor path_label_bg_color;
    QList<QPointF> path_label_top_lefts;
    QRect path_label_rect;
};
