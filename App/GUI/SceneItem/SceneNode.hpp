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

    //

    std::string GetID() { return m_crate.id; }

private:
    [[nodiscard]] QPointF GetOffsetFromPivot(Pivot pivot) const;

    //void mousePressEvent(QGraphicsSceneMouseEvent* mouse_event) override;

    //

    SceneNodeCrate m_crate;
};
