#include <QMouseEvent>

#include "DragButton.hpp"

DragButton::DragButton(const NodeType type, const QIcon& icon, QWidget* parent) :
    QPushButton(icon, "", parent),
    m_type(type)
{
    //
}

void DragButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit DragStateChanged(m_type);
    }
    QPushButton::mousePressEvent(event);
}

void DragButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit DragStateChanged(std::nullopt);
    }
    QPushButton::mouseReleaseEvent(event);
}
