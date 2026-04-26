#pragma once

#include <QDebug>
#include <QRectF>

// You feed it points and it gives smallest Axis Aligned Bounding Rectangle for them
struct AABRHelper
{
    qreal x_min{};
    qreal x_max{};
    qreal y_min{};
    qreal y_max{};

    // Ctor with an initial point
    explicit AABRHelper(const QPointF point) : x_min(point.x()), x_max(point.x()), y_min(point.y()), y_max(point.y()) {}

    // Add a new point
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

    // Get the AABR for given points
    constexpr QRectF ToQRectF() const
    {
        return {QPointF(x_min, y_min), QPointF(x_max, y_max)};
    }

    // --- --- --- --- --- --- --- --- --- ---

    // Functionality for SVG export (determining document size)

    AABRHelper()
    {
        Reset();
    }

    void Reset()
    {
        x_min = std::numeric_limits<qreal>::max();
        y_min = std::numeric_limits<qreal>::max();
        x_max = std::numeric_limits<qreal>::min();
        y_max = std::numeric_limits<qreal>::min();
    }

    void UpdateWithQRectF(const QRectF& rect)
    {
        if (rect.left() < x_min) {
            x_min = rect.left();
        }
        if (rect.right() > x_max) {
            x_max = rect.right();
        }
        if (rect.top() < y_min) {
            y_min = rect.top();
        }
        if (rect.bottom() > y_max) {
            y_max = rect.bottom();
        }
    }

    void DebugPrint() const
    {
        qDebug() << "x_min" << x_min << ", x_max" << x_max << ", y_min" << y_min << ", y_max" << y_max;
    }
};
