#pragma once

#include <QColor>

constexpr auto FONT_SIZE_DEFAULT = 20;
constexpr auto FONT_SIZE_MAIN_MENU_BAR = 16;

//constexpr auto SCENE_SIZE = 5000;
//constexpr auto SCENE_MID = SCENE_SIZE / 2;

inline QColor COLOR_CANVAS_BACKGROUND = QColor::fromRgb(240, 240, 240);

constexpr bool DO_SHOW_GRID_INIT = true;
constexpr qreal GRID_STEP_BASE = 100.0;
inline QColor COLOR_GRID_LINE = QColor::fromRgb(200, 200, 200, 40);
//inline QColor COLOR_GRID_LINE = QColor::fromRgb(200, 200, 200);

inline QColor COLOR_ERROR = QColor::fromRgb(211, 1, 2);

constexpr auto SCENE_FONT_SIZE_BASE = 18;

constexpr auto NODE_PADDING = 18;

constexpr int TIP_ARROW_LENGTH = 12;
constexpr int TIP_ARROW_SPAN = 4;
