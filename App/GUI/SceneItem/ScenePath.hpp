#pragma once

#include <QGraphicsItem>

#include "../../Model/ScenePathCrate.hpp"

// === === === === === === === === ===

class ScenePath : public QGraphicsItem
{
public:
    explicit ScenePath(ScenePathCrate crate);

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    ScenePathCrate m_crate;
};
