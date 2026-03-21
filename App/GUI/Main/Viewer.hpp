#pragma once

#include <QPointer>
#include <QScrollBar>

#include "Scene.hpp"

// ===  GUISceneViewer config  === === === === === === === === === === ===
inline QColor COLOR_CANVAS_BACKGROUND = QColor::fromRgb(240, 240, 240);
constexpr qreal GRID_STEP_BASE = 100.0;
constexpr bool DO_SHOW_GRID_INIT = true;
inline QColor COLOR_GRID_LINE = QColor::fromRgb(200, 200, 200, 40);
// === === === === === === === === === === === === === === === === === ===

class GUISceneViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GUISceneViewer(GUIScene* scene, QWidget* parent = nullptr);

    void ResetCanvasScrolling(int offset_x = 5, int offset_y = 5);

    bool m_do_show_grid = DO_SHOW_GRID_INIT;

private:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    bool m_is_panning{};
    int m_pan_start_x{}, m_pan_start_y{};

    //

    bool eventFilter(QObject* watched, QEvent* event) override;

    QPointer<GUIScene> m_scene;

signals:
    void ZoomChangeRequested(bool is_plus);
};
