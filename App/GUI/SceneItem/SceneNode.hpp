#pragma once

// Qt imports
#include <QGraphicsItem>

// Qt fwd declrs
class QPainter;

// App imports
#include "App/Model/Node.hpp"
#include "App/Model/Pivot.hpp"

// App fwd declrs

// ===================================
class SceneNode : public QGraphicsItem
{
public:
    explicit SceneNode(const Node& node, const QFont& font, const QFontMetrics& font_metrics);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    [[nodiscard]] QPointF GetOffsetFromPivot(Pivot pivot) const;

private:
    Node m_node;
    QFont m_font;
    QFontMetrics m_font_metrics;
};
