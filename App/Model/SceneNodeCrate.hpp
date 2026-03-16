#pragma once

#include <QColor>
#include <QFont>
#include <QRect>

#include "NodeType.hpp"

struct SceneNodeCrate
{
    QRectF aabr;
    QColor color_fill;
    QColor color_edge;
    QFont font;
    QPointF label_position;
    QString label_value;
    NodeType type;
};
