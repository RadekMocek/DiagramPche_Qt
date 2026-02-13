#include "../Helper/Print.hpp"
#include "Viewer.hpp"
#include <QMouseEvent>

GUISceneViewer::GUISceneViewer(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent) {}

void GUISceneViewer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        m_is_panning = true;
        m_pan_start_x = event->position().x();
        m_pan_start_y = event->position().y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    event->ignore();
}

void GUISceneViewer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        m_is_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    event->ignore();
}

void GUISceneViewer::mouseMoveEvent(QMouseEvent* event)
{
    if (m_is_panning) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->position().x() - m_pan_start_x));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->position().y() - m_pan_start_y));
        m_pan_start_x = event->position().x();
        m_pan_start_y = event->position().y();
        event->accept();
        return;
    }
    event->ignore();
}

void GUISceneViewer::wheelEvent(QWheelEvent* event)
{
    constexpr qreal MIN_ZOOM = 0.333;
    constexpr qreal MAX_ZOOM = 1.666;
    constexpr qreal ZOOM_STEP = 0.222;

    const ViewportAnchor anchor = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    const int angle = event->angleDelta().y();
    qreal factor;
    if (angle > 0) {
        factor = 1.0 + ZOOM_STEP;
    }
    else {
        factor = 1.0 - ZOOM_STEP;
    }

    const qreal current_scale = transform().m11();
    if ((factor > 1.0 && current_scale < MAX_ZOOM) ||
        (factor < 1.0 && current_scale > MIN_ZOOM)) {
        scale(factor, factor);
    }

    setTransformationAnchor(anchor);
}
