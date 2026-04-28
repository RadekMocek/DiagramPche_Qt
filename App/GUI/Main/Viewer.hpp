#pragma once

#include <QScrollBar>

#include "Scene.hpp"

// ===  GUISceneViewer config  === === === === === === === === === === === ===
inline QColor COLOR_CANVAS_BACKGROUND_LIGHT = QColor::fromRgb(240, 240, 240);
inline QColor COLOR_CANVAS_BACKGROUND_DARK = QColor::fromRgb(15, 15, 15);
constexpr qreal GRID_STEP_BASE = 100.0;
constexpr bool DO_SHOW_GRID_INIT = true;
inline QColor COLOR_GRID_LINE = QColor::fromRgb(200, 200, 200, 40);
// === === === === === === === === === === === === === === === === === === ===

class GUISceneViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GUISceneViewer(GUIScene* scene, QWidget* parent = nullptr);

    void ResetCanvasScrolling(int offset_x = 5, int offset_y = 5);

    bool IsUserDraggingCanvas() const { return m_is_panning; }

    bool m_do_show_grid = DO_SHOW_GRID_INIT;
    bool m_is_background_light{};

private:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    bool m_is_panning{};
    qreal m_pan_start_x{}, m_pan_start_y{};

    //

    bool eventFilter(QObject* watched, QEvent* event) override;

    QPointer<GUIScene> m_scene;

signals:
    void ZoomChangeRequested(bool is_plus);
};
