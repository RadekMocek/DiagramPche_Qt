#pragma once

// Qt imports
#include <QGraphicsItem>


// Qt fwd declrs
class QPainter;

// App imports
#include "App/Model/SceneNodeCrate.hpp"
#include "App/Model/Pivot.hpp"

// App fwd declrs

// ===================================
class SceneNode : public QGraphicsItem
{
public:
    explicit SceneNode(const SceneNodeCrate& crate);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    [[nodiscard]] QPointF GetExactPointFromPivot(Pivot pivot) const;

private:
    [[nodiscard]] QPointF GetOffsetFromPivot(Pivot pivot) const;

    //

    SceneNodeCrate m_crate;

    QRectF m_aabr;
    QColor m_color;
    QFont m_font;
    QPointF m_label_position;
    QString m_label_value;
};
