#include <QMouseEvent>

#include "Viewer.hpp"

GUISceneViewer::GUISceneViewer(GUIScene* scene, QWidget* parent) :
    QGraphicsView(scene, parent),
    m_scene(scene)
{
    // For the grid to work
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // For getting mouse position when drag'n'dropping node
    qApp->installEventFilter(this);

    // Set large scene and hide the scrollbars for the same canvas as its IMGUI counterparts
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene->setSceneRect(-100000, -100000, 200000, 200000);
}

void GUISceneViewer::ResetCanvasScrolling(const int offset_x, const int offset_y)
{
    centerOn(size().width() / 2 - offset_x,
             size().height() / 2 - offset_y);
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
    const ViewportAnchor anchor = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    emit ZoomChangeRequested(event->angleDelta().y() > 0);
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

bool GUISceneViewer::eventFilter(QObject* watched, QEvent* event)
{
    if (m_scene->IsDraggingNode() && event->type() == QEvent::MouseMove) {
        m_scene->UpdateGhostNodePositionAndVisibility(mapToScene(mapFromGlobal(
            dynamic_cast<QMouseEvent*>(event)->globalPosition().toPoint()
        )));
    }

    return QGraphicsView::eventFilter(watched, event);
}
