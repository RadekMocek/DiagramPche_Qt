#pragma once

// Qt imports
#include <QGraphicsItem>
#include <QPainter>

// App imports
#include "App/Model/Pivot.hpp"

// App fwd declrs
class Node;

// ===================================
class SceneNode : public QGraphicsItem
{
public:
    explicit SceneNode(const QFont& font, const QString& text, QSizeF size);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    [[nodiscard]] QPointF GetOffsetFromPivot(Pivot pivot) const;

private:
    QFont m_font;
    QString m_text;
    QSizeF m_size;
};
