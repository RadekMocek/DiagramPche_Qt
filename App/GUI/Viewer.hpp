#pragma once

#include <QGraphicsView>
#include <QScrollBar>

#include "../Config.hpp"

class GUISceneViewer : public QGraphicsView
{
public:
    explicit GUISceneViewer(QGraphicsScene* scene, QWidget* parent = nullptr);

    bool m_do_show_grid = DO_SHOW_GRID_INIT;

private:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    bool m_is_panning{};
    int m_pan_start_x{}, m_pan_start_y{};
};
