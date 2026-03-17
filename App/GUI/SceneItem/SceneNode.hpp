#pragma once

#include <QGraphicsItem>

#include "../../Model/Pivot.hpp"
#include "../../Model/SceneNodeCrate.hpp"

// === === === === === === === === ===

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
};
