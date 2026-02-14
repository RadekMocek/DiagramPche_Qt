#pragma once

#include <QRectF>

struct AABRHelper
{
    qreal x_min;
    qreal x_max;
    qreal y_min;
    qreal y_max;

    explicit AABRHelper(const QPointF point) : x_min(point.x()), x_max(point.x()), y_min(point.y()), y_max(point.y()) {}

    void Update(const QPointF point)
    {
        if (point.x() < x_min) {
            x_min = point.x();
        }
        if (point.x() > x_max) {
            x_max = point.x();
        }
        if (point.y() < y_min) {
            y_min = point.y();
        }
        if (point.y() > y_max) {
            y_max = point.y();
        }
    }

    [[nodiscard]] constexpr QRectF ToQRectF() const
    {
        return QRectF(QPointF(x_min, y_min), QPointF(x_max, y_max));
    }
};
