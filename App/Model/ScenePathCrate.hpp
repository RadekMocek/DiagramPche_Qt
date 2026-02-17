#pragma once

#include <QColor>
#include <QList>
#include <QRectF>

struct ScenePathCrate
{
    QRectF aabr;
    QList<QList<QPointF>> paths;
    QColor color;
    bool do_start_arrow{};
    bool do_end_arrow{};
};
