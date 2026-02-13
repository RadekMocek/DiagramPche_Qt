#pragma once

#include <QColor>
#include <QFont>
#include <QRect>

struct SceneNodeCrate
{
    QRectF aabr;
    QColor color;
    QFont font;
    QPointF label_position;
    QString label_value;
};
