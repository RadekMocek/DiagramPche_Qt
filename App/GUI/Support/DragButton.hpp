#pragma once

// Qt imports
#include <QPushButton>

// App fwd declrs
enum NodeType : int;

// === === === === === === === === ===

class DragButton : public QPushButton
{
    Q_OBJECT

public:
    explicit DragButton(NodeType type, QWidget* parent = nullptr);

private:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    NodeType m_type;

signals:
    void DragStateChanged(std::optional<NodeType> type);
};
