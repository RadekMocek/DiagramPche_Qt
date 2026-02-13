#pragma once

#include <QColor>

constexpr QColor GetQColorFromString(const std::string& color_str)
{
    if (color_str.length() == 9 && color_str[0] == '#') {
        unsigned int r = 0, g = 0, b = 0, a = 0;
        std::sscanf(color_str.c_str(), "#%2x%2x%2x%2x", &r, &g, &b, &a);
        return {
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b),
            static_cast<unsigned char>(a)
        };
    }
    return {0, 0, 0, 0};
}
