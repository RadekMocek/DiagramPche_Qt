#pragma once

#include <QColor>
#include <QList>
#include <QPointF>

struct ScenePathCrate
{
    QList<QList<QPointF>> paths;
    QColor color;
    bool do_start_arrow;
    bool do_end_arrow;
};
