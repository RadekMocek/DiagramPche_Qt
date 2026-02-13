#pragma once

#include <QGraphicsView>
#include <QScrollBar>

class GUISceneViewer : public QGraphicsView
{
public:
    explicit GUISceneViewer(QGraphicsScene *scene, QWidget *parent = nullptr);

private:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    bool m_is_panning{};
    int m_pan_start_x{}, m_pan_start_y{};
};
