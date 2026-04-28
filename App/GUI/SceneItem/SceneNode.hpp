#pragma once

#include <QGraphicsItem>

#include "../../Model/Pivot.hpp"
#include "../../Model/SceneNodeCrate.hpp"

// === === === === === === === === ===

class SceneNode : public QGraphicsItem
{
public:
    explicit SceneNode(SceneNodeCrate crate);
    QRectF boundingRect() const override;
    QPointF GetExactPointFromPivot(Pivot pivot) const;
    std::string GetID() const { return m_crate.id; }

    void DebugPrint() const { m_crate.DebugPrint(); }

    // Ghost node
    void UpdateGhostNode(const QString& label_value, NodeType type);

private:
    QPointF GetOffsetFromPivot(Pivot pivot) const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    //

    SceneNodeCrate m_crate;
};
