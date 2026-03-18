#include <QMouseEvent>

#include "Viewer.hpp"

GUISceneViewer::GUISceneViewer(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent)
{
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // For the grid to work
}

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

    // Pass further for LMB to select nodes to work
    QGraphicsView::mousePressEvent(event);
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

    // Pass further for mouse hover on nodes to work
    QGraphicsView::mouseMoveEvent(event);
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

    if (const qreal current_scale = transform().m11();
        (factor > 1.0 && current_scale < MAX_ZOOM) || (factor < 1.0 && current_scale > MIN_ZOOM)) {
        scale(factor, factor);
    }

    setTransformationAnchor(anchor);
}

void GUISceneViewer::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, COLOR_CANVAS_BACKGROUND);
    if (!m_do_show_grid) return;

    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen pen(COLOR_GRID_LINE);
    pen.setWidth(1);
    painter->setPen(pen);

    qreal x1, y1, x2, y2;
    rect.getCoords(&x1, &y1, &x2, &y2);

    const int left_gridline_index = static_cast<int>(std::ceil(x1 / GRID_STEP_BASE));
    const int right_gridline_index = static_cast<int>(std::floor(x2 / GRID_STEP_BASE));
    for (auto i = left_gridline_index; i <= right_gridline_index; ++i) {
        const auto x = i * GRID_STEP_BASE;
        painter->drawLine(x, y1, x, y2);
    }

    const int top_gridline_index = static_cast<int>(std::ceil(y1 / GRID_STEP_BASE));
    const int bottom_gridline_index = static_cast<int>(std::floor(y2 / GRID_STEP_BASE));
    for (auto i = top_gridline_index; i <= bottom_gridline_index; ++i) {
        const auto y = i * GRID_STEP_BASE;
        painter->drawLine(x1, y, x2, y);
    }
}
