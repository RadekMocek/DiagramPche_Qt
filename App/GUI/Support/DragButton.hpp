#pragma once

// Qt imports
#include <QPushButton>

// App imports
#include "../../Model/NodeType.hpp"

// === === === === === === === === ===

// Used for secondary canvas toolbar buttons, which you can drag'n'drop to canvas to add a new node
class DragButton : public QPushButton
{
    Q_OBJECT

public:
    explicit DragButton(NodeType type, const QIcon& icon, QWidget* parent = nullptr);

private:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    NodeType m_type;

signals:
    void DragStateChanged(std::optional<NodeType> type);
};
