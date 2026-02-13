#pragma once

#include <vector>

#include <QColor>

#include "Pathpoint.hpp"
#include "Point.hpp"

struct Path
{
    Point start{};
    std::vector<Point> ends{};
    std::vector<Pathpoint> pathpoints{};

    int shift{};

    QColor color = {0, 0, 0, 255};

    bool do_start_arrow = false;
    bool do_end_arrow = true;
};
