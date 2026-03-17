#pragma once

#include <QPainter>
#include <QPointF>

inline QPointF QPointFNormalized(const QPointF p)
{
    const auto magnitude = sqrtf((p.x() * p.x()) + (p.y() * p.y()));
    //qDebug() << "In:" << p << ", Out:" << QPointF(p.x() / magnitude, p.y() / magnitude);
    return {p.x() / magnitude, p.y() / magnitude};
}

constexpr QPointF QPointFOrthogonalized(const QPointF p)
{
    return {-p.y(), p.x()};
}

inline void DrawArrowTip(
    QPainter* painter,
    const QPointF p1,
    const QPointF p2
)
{
    constexpr int TIP_ARROW_LENGTH = 12;
    constexpr int TIP_ARROW_SPAN = 4;

    const auto p2_to_p1 = QPointFNormalized(p1 - p2);
    const auto point_slightly_before_p2 = p2 + p2_to_p1 * TIP_ARROW_LENGTH;
    const auto p2_orthogonal_addition = QPointFOrthogonalized(p2_to_p1) * TIP_ARROW_SPAN;

    QPolygonF arrow_tip;
    arrow_tip
        << p2
        << point_slightly_before_p2 - p2_orthogonal_addition
        << point_slightly_before_p2 + p2_orthogonal_addition;

    painter->drawConvexPolygon(arrow_tip);
}
