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

    bool _pan{};
    int _panStartX{}, _panStartY{};
};
