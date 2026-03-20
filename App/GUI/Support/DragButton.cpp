#include <QMouseEvent>

#include "DragButton.hpp"
#include "../../Model/NodeType.hpp"


DragButton::DragButton(const NodeType type, QWidget* parent) :
    QPushButton(GetButtonLabelFromNodeType(type), parent),
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
