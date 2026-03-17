#pragma once

#include <tuple>

#include <QColor>

// Common colors
inline QColor COLOR_BLACK = QColor::fromRgb(0, 0, 0);
inline QColor COLOR_ERROR = QColor::fromRgb(211, 1, 2);
inline QColor COLOR_ERROR_HIGHLIGHT = QColor::fromRgb(211, 1, 2, 80);

// Color functions
using ColorTuple = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

inline QColor GetQColorFromTuple(const ColorTuple& tup)
{
    return QColor::fromRgb(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));
}

inline ColorTuple GetColorTupleFromString(const std::string& color_str)
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
