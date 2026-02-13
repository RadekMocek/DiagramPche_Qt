#pragma once

#include <vector>

#include "Pathpoint.hpp"
#include "Point.hpp"
#include "App/Helper/DrawLayer.hpp"

struct Path
{
    Point start{};
    std::vector<Point> ends{};
    std::vector<Pathpoint> pathpoints{};

    int shift{};

    std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> color = {0, 0, 0, 255};

    bool do_start_arrow = false;
    bool do_end_arrow = true;

    int z = DL_USER_CHANNEL_DEFAULT_PATH;
};
