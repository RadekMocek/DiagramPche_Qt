#pragma once

#include <QColor>
#include <QFont>
#include <QRect>

#include "NodeType.hpp"

struct SceneNodeCrate
{
    std::string id;
    //
    QRectF aabr;
    QColor color_fill;
    QColor color_edge;
    QFont font;
    QPointF label_position;
    QString label_value;
    NodeType type;

    void DebugPrint() const
    {
        qDebug()
            << "id: " << id
            << ", aabr: " << aabr
            << ", color_fill: " << color_fill
            << ", color_edge: " << color_edge
            << ", font: " << font
            << ", label_position: " << label_position
            << ", label_value: " << label_value
            << ", type: " << type;
    }
};
