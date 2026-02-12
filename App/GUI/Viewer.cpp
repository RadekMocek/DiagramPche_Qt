#include "Viewer.hpp"
#include <QMouseEvent>

GUISceneViewer::GUISceneViewer(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent) {}

void GUISceneViewer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        _pan = true;
        _panStartX = event->x();
        _panStartY = event->y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    event->ignore();
}

void GUISceneViewer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        _pan = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    event->ignore();
}

void GUISceneViewer::mouseMoveEvent(QMouseEvent* event)
{
    if (_pan) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
        _panStartX = event->x();
        _panStartY = event->y();
        event->accept();
        return;
    }
    event->ignore();
}
