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
    [[nodiscard]] QPointF GetExactPointFromPivot(Pivot pivot) const;
    std::string GetID() { return m_crate.id; }

private:
    [[nodiscard]] QPointF GetOffsetFromPivot(Pivot pivot) const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    //

    SceneNodeCrate m_crate;
};
